/*************************************************************************************************
 *
 *   PatternMatching.cpp
 *
 *   Created by dmitry
 *   18.03.2021
 *
 ***/


//
/// PatternMatching on GPU
///======================================================================================
/// this file implements an algorithm for searching for patterns in a line on a GPU.
///
/// briefly about the data structures used:
/// 1.pettern table
/// In fact, it is a three-dimensional array with height and width = 256 (max char value)
/// and depth = maximum number of patterns with the same prefix.
///
/// 2. Signature table
/// Corresponds to the current level (depth) in the signature table.
/// under the coordinates (first character, second) are stored the following 4
/// characters. If there is no pattern with such a prefix, then nons are there.
///
/// What's happening:
///  Having received a string and patterns as input, we build a table of patterns (see
/// the buildPatternsTable function). Then we build the first signature table (see the
/// buildSignatureTable function).
/// This table is used by the GPU to filter matches.
/// That is, each video card thread takes 6 characters from the base string (in fact,
/// 7 because it checks two matches at once), the first two are used for indexing in the
/// signature table. The next four characters are compared with the contents of the
/// table.
/// If the contents match, then the coordinates in the signature table where the match
/// occurred are written to the output buffer, under the index corresponding to the
/// character being checked in the main line.
/// (here, while the video card is busy checking, the CPU starts building a new signature
/// table)
/// Further, all possible matches are checked on the central processor and the result is
/// written into a hash table.
///
/// These actions are repeated in a loop until we pass all the levels of the pattern
/// table.
///======================================================================================
///======================================================================================
//

#include "PatternMatching.hpp"

#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cmath>

#ifdef DEBUG
#define DEBUG_ACTION(act) { act }

#else
#define DEBUG_ACTION(act) {}

#endif //DEBUG


namespace ezg
{
    cl_float PatternMatchingGPU::nons = 666;


    PatternMatchingGPU::PatternMatchingGPU(const cl::Device& device)
            : m_context(device)
            , m_commandQueue(m_context, device, CL_QUEUE_PROFILING_ENABLE)
    {
        std::ifstream kernel_in(m_kernelSourceName);
        if (!kernel_in.is_open()) {
            throw std::runtime_error("cant open file: " + m_kernelSourceName);
        }

        std::ostringstream str_out;
        str_out << kernel_in.rdbuf();
        std::string source_kernel = str_out.str();

        m_program = cl::Program(m_context, source_kernel);

        // workgroup size information is needed at compile(build) time
        // to allocate local memory of the desired size.
        m_maxWorkGroupSize = device.getInfo< CL_DEVICE_MAX_WORK_GROUP_SIZE >();
        try {
            std::ostringstream options;
            options << " -D MAX_GROUP_SIZE=" << m_maxWorkGroupSize;
            options << " -D NON_SYMBOL_CODE=" << nons;

            m_program.build(options.str().c_str());
        }
        catch (const std::exception& ex) {
            std::cerr << ex.what() << std::endl;
            std::cerr << m_program.getBuildInfo<CL_PROGRAM_BUILD_LOG>().data()->second << std::endl;
            throw ex;
        }
    }



    std::unordered_map< size_t, std::vector< size_t > >
    PatternMatchingGPU::match(const std::string& string
          , const std::vector< std::string >& patterns)
    {
        if (string.empty() || patterns.empty()) {
            return {};
        }

        std::unordered_map< size_t, std::vector< size_t > > result_of_func;
        result_of_func = checkSmallPatterns(string, patterns);

        auto&& table = buildPatternsTable(patterns);
        const size_t maxDepthTable = getMaxDepthPatternTable(table);
        const size_t numFrames = std::min(m_maxFramesInFly, maxDepthTable);
        const size_t length = string.size();



        cl::Buffer cl_buff_string(m_context, CL_MEM_READ_ONLY,
                                  length * sizeof(std::string::value_type));
        m_commandQueue.enqueueWriteBuffer(cl_buff_string, CL_TRUE, 0
                                          , length * sizeof(std::string::value_type)
                                          , string.data());



        size_t im_weight = 1 << sizeof(char) * 8;
        size_t im_height = 1 << sizeof(char) * 8;
        cl::ImageFormat iFormat(CL_RGBA, CL_FLOAT);
        std::vector< cl::Image2D > cl_signature_tables;
        cl_signature_tables.reserve(numFrames);
        for (size_t i = 0; i < numFrames; ++i) {
            cl_signature_tables.emplace_back(m_context, CL_MEM_READ_ONLY, iFormat, im_weight, im_height);
        }

        std::vector< cl::Buffer > cl_res_buffers;
        cl_res_buffers.reserve(numFrames);
        for (size_t i = 0; i < numFrames; ++i) {
            cl_res_buffers.emplace_back(m_context, CL_MEM_WRITE_ONLY, length * 2 * sizeof(cl_float));
        }


        const size_t global_size = length / 2 + length % 2;
        m_lastTime = 0;

        std::vector< cl_float2 > result(length);
        std::vector< cl::Event > events(numFrames);
        std::vector< size_t >    depths(numFrames, 0);


        // first launch
        for (size_t st = 0; st < numFrames; ++st)
        {
            auto&& signature_table = buildSignatureTable(table, st);
            depths.at(st) = st;
            assert(!signature_table.empty());

            m_commandQueue.enqueueWriteImage(cl_signature_tables.at(st), CL_TRUE
                                             , {0, 0, 0}, {im_weight, im_height, 1}
                                             , 0, 0, signature_table.data());

            cl::Kernel kernel(m_program, "signature_match");
            kernel.setArg(0, cl_buff_string);
            kernel.setArg(1, cl_res_buffers.at(st));
            kernel.setArg(2, static_cast<cl_uint>(length));
            kernel.setArg(3, cl_signature_tables.at(st));
            m_commandQueue.enqueueNDRangeKernel(kernel, 0, global_size, cl::NullRange, nullptr, &events.at(st));
        }


        // midl stage
        for(size_t st = numFrames; st < maxDepthTable; ++st)
        {
            const size_t curFrame = st % numFrames;

            auto&& signature_table = buildSignatureTable(table, st);
            assert(!signature_table.empty());


            events.at(curFrame).wait();
            cl_ulong start_time = events.at(curFrame).getProfilingInfo<CL_PROFILING_COMMAND_START>();
            cl_ulong end_time = events.at(curFrame).getProfilingInfo<CL_PROFILING_COMMAND_END>();
            m_lastTime += (end_time - start_time) / 1000000000.0;


            cl::copy(m_commandQueue, cl_res_buffers.at(curFrame), result.begin(), result.end());


            m_commandQueue.enqueueWriteImage(cl_signature_tables.at(curFrame), CL_TRUE
                                             , {0, 0, 0}, {im_weight, im_height, 1}
                                             , 0, 0, signature_table.data());

            cl::Kernel kernel(m_program, "signature_match");
            kernel.setArg(0, cl_buff_string);
            kernel.setArg(1, cl_res_buffers.at(curFrame));
            kernel.setArg(2, static_cast<cl_uint>(length));
            kernel.setArg(3, cl_signature_tables.at(curFrame));
            m_commandQueue.enqueueNDRangeKernel(kernel, 0, global_size, cl::NullRange, nullptr, &events.at(curFrame));


            for (size_t i = 0; i < length; ++i) {
                auto&& cur_item = result.at(i);
                if(cur_item.x != nons && cur_item.y != nons)
                {
                    auto&& pat = table.at(static_cast<u_char>(static_cast<char>(cur_item.x))
                                          , static_cast<u_char>(static_cast<char>(cur_item.y))).at(depths.at(curFrame));
                    if (checkMatch(string, i, pat.second))
                    {
                        result_of_func[pat.first].push_back(i);
                    }
                }
            }
            depths.at(curFrame) = st;
        }


        // closing circle
        for(size_t st = maxDepthTable; st < maxDepthTable + numFrames; ++st)
        {
            const size_t curFrame = st % numFrames;

            events.at(curFrame).wait();
            cl_ulong start_time = events.at(curFrame).getProfilingInfo<CL_PROFILING_COMMAND_START>();
            cl_ulong end_time = events.at(curFrame).getProfilingInfo<CL_PROFILING_COMMAND_END>();
            m_lastTime += (end_time - start_time) / 1000000000.0;

            cl::copy(m_commandQueue, cl_res_buffers.at(curFrame), result.begin(), result.end());

            for (size_t i = 0; i < length; ++i) {
                auto&& cur_item = result.at(i);
                if(cur_item.x != nons && cur_item.y != nons)
                {
                    auto&& pat = table.at(static_cast<u_char>(static_cast<char>(cur_item.x))
                                          , static_cast<u_char>(static_cast<char>(cur_item.y))).at(depths.at(curFrame));
                    if (checkMatch(string, i, pat.second))
                    {
                        result_of_func[pat.first].push_back(i);
                    }
                }
            }
        }

        return result_of_func;
    }




    matrix::Matrix< std::vector< std::pair< size_t, std::string > > >
    PatternMatchingGPU::buildPatternsTable(const std::vector< std::string > &patterns)
    {
        matrix::Matrix< std::vector< std::pair< size_t, std::string > > > res(256, 256);

        for (size_t i = 0, mi = patterns.size(); i < mi; ++i)
        {
            auto&& p = patterns.at(i);
            if (p.size() >= 6) {
                res.at(static_cast<u_char>(p[0]), static_cast<u_char>(p[1])).emplace_back(i, p);
            }
        }

        return res;
    }


    size_t PatternMatchingGPU::getMaxDepthPatternTable(const matrix::Matrix< std::vector< std::pair< size_t, std::string > > >& table) const
    {
        const size_t lines = table.getLines();
        const size_t columns = table.getColumns();

        size_t result = 0;
        for (size_t l = 0; l < lines; ++l)
        {
            for (size_t c = 0; c < columns; ++c)
            {
                result = std::max(result, table.at(l, c).size());
            }
        }
        return result;
    }


    matrix::Matrix< cl_float4 >
            PatternMatchingGPU::buildSignatureTable(const matrix::Matrix< std::vector< std::pair< size_t, std::string > > > &patTable
                                                    , size_t step)
    {
        const size_t res_lines = 256, res_columns = 256;
        matrix::Matrix< cl_float4 > res(res_lines, res_columns);

        size_t count = 0;
        for(size_t l = 0; l < res_lines; ++l) {
            for (size_t c = 0; c < res_columns; ++c) {
                auto&& cPatterns = patTable.at(l, c);
                auto &&cVec = res.at(l, c);

                if (cPatterns.size() > step)
                {
                    auto &&cur_pat = cPatterns.at(step).second;
                    if (cur_pat.size() > 5) {
                        cVec.x = cur_pat.at(2);
                        cVec.y = cur_pat.at(3);
                        cVec.z = cur_pat.at(4);
                        cVec.w = cur_pat.at(5);
                        count++;
                    }
                }
            }
        }
        if (count)
            return res;

        return {};
    }



    bool PatternMatchingGPU::checkMatch(const std::string& str, size_t pos, const std::string& pattern)
    {
        const size_t length_str = str.size();
        const size_t length_pat = pattern.size();
        bool res = true;
        if (pos + length_pat <= length_str)
        {
            for (size_t i = 6; i < length_pat; ++i)
            {
                if (str[pos + i] != pattern[i]) {
                    res = false;
                    break;
                }

            }
        }
        else {
            res = false;
        }

        return res;
    }



    std::unordered_map< size_t, std::vector< size_t > >
            PatternMatchingGPU::checkSmallPatterns(const std::string& str, const std::vector< std::string >& patterns)
    {
        std::unordered_map< size_t, std::vector< size_t > > result;

        const size_t num_patterns = patterns.size();
        result.reserve(num_patterns);
        for (size_t i = 0; i < num_patterns; ++i) {
            auto&& pattern = patterns.at(i);
            if (pattern.size() > 5)
                continue;

            size_t cur_pos = str.find(pattern);
            while(cur_pos != std::string::npos)
            {
                result[i].push_back(cur_pos);
                cur_pos = str.find(pattern, cur_pos + 1);
            }

        }
        return result;
    }


}//namespace ezg