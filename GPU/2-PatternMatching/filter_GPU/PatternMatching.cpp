/*************************************************************************************************
 *
 *   PatternMatching.cpp
 *
 *   Created by dmitry
 *   18.03.2021
 *
 ***/

#include "PatternMatching.hpp"
#include "../native_cpu/native_cpy.hpp"

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
            std::string options(std::string(" -D DATA_TYPE=int ")
                                + "-D MAX_GROUP_SIZE=" + std::to_string(m_maxWorkGroupSize)
                                + " -D NON_SYMBOL_CODE=" + std::to_string(static_cast<int>(nons)));

            //DEBUG_ACTION(options += " -D DEBUG ";);

            m_program.build(options.c_str());
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
        auto&& table = buildPatternsTable(patterns);
        const size_t length = string.size();


        size_t im_weight = 1 << sizeof(char) * 8;
        size_t im_height = 1 << sizeof(char) * 8;
        cl::ImageFormat iFormat(CL_RGBA, CL_FLOAT);

        cl::Image2D cl_signature_table(m_context, CL_MEM_READ_ONLY
                                    , iFormat, im_weight, im_height);

        cl::Buffer cl_buff_string(m_context, CL_MEM_READ_ONLY,
                                  length * sizeof(std::string::value_type));
        m_commandQueue.enqueueWriteBuffer(cl_buff_string, CL_TRUE, 0
                                          , length * sizeof(std::string::value_type)
                                          , string.data());

        cl::Buffer cl_buff_res(m_context, CL_MEM_WRITE_ONLY,
                                 length * 2 * sizeof(cl_float));

        size_t global_size = length / 2 + length % 2;



        struct res_float2 { float x= 0, y = 0; };
        std::vector< res_float2 > res(length);
        std::unordered_map< size_t, std::vector< size_t > > result_of_func;
        auto&& signature_table = buildSignatureTable(table, 0);
        for(size_t step = 0; !signature_table.empty(); ++step)
        {
            m_commandQueue.enqueueWriteImage(cl_signature_table, CL_TRUE
                                             , {0, 0, 0}, {im_weight, im_height, 1}
                                             , 0, 0, signature_table.data());

            cl::Kernel kernel(m_program, "image_test");
            kernel.setArg(0, cl_buff_string);
            kernel.setArg(1, cl_buff_res);
            kernel.setArg(2, static_cast<cl_uint>(length));
            kernel.setArg(3, cl_signature_table);

            cl::Event event;
            m_commandQueue.enqueueNDRangeKernel(kernel, 0, global_size, cl::NullRange, nullptr, &event);

            signature_table = buildSignatureTable(table, step + 1);

            event.wait();
            cl::copy(m_commandQueue, cl_buff_res, res.begin(), res.end());

            for (size_t i = 0; i < length; ++i) {
                auto&& cur_res = res.at(i);
                if(cur_res.x != nons && cur_res.y != nons)
                {
                    auto&& pat = table.at(static_cast<u_char>(cur_res.x), static_cast<u_char>(cur_res.y)).at(step);
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
            if (p.size() >= 2) {
                res.at(static_cast<u_char>(p[0]), static_cast<uint8_t>(p[1])).emplace_back(i, p);
            }
        }

        return res;
    }

    matrix::Matrix< PatternMatchingGPU::vec4 >
            PatternMatchingGPU::buildSignatureTable(const matrix::Matrix< std::vector< std::pair< size_t, std::string > > > &patTable
                                                    , size_t step)
    {
        matrix::Matrix< vec4 > res(256, 256);

        size_t count = 0;
        for(size_t l = 0; l < res.getLines(); l++) {
            for (size_t c = 0; c < res.getColumns(); c++) {
                auto&& cPatterns = patTable.at(l, c);
                auto &&cVec = res.at(l, c);

                if (cPatterns.size() > step)
                {
                    auto &&cur_pat = cPatterns.at(step).second;
                    const size_t pat_size = cur_pat.size();

                    cVec = vec4(nons);
                    cVec.x = cur_pat.at(2);
                    cVec.y = cur_pat.at(3);
                    cVec.z = cur_pat.at(4);
                    cVec.w = cur_pat.at(5);
                    count++;
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
            for (size_t i = 0; i < length_pat; ++i)
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


}//namespace ezg