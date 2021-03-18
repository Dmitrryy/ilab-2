/*************************************************************************************************
 *
 *   PatternMatching.cpp
 *
 *   Created by dmitry
 *   18.03.2021
 *
 ***/

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
                                + "-D MAX_GROUP_SIZE=" + std::to_string(m_maxWorkGroupSize));

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

        auto&& signature_table = buildSignatureTable(table);

        size_t im_weight = 1 << sizeof(char) * 8;
        size_t im_height = 1 << sizeof(char) * 8;
        cl::ImageFormat iFormat(CL_FLOAT, CL_RGBA);
        cl::Image2D cl_signature_table(m_context, CL_MEM_READ_WRITE
                                    , iFormat, im_weight, im_height);


        m_commandQueue.enqueueWriteImage(cl_signature_table, CL_TRUE
                                         , {0, 0, 0}, {im_weight, im_height, 1}
                                         , 0, 0, signature_table.data());

        //TODO
    }

}//namespace ezg