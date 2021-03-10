/*************************************************************************************************
 *
 *   BitonicSort.cpp
 *
 *   Created by dmitry
 *   10.03.2021
 *
 ***/

//
/// BitonicSort
///======================================================================================
/// this file implements bitonic sorting (https://en.wikipedia.org/wiki/Bitonic_sorter).
///
/// The following optimizations apply:
/// wherever possible the kernel is used on local memory.
/// The condition for its use: the distance between the compared elements does not exceed
/// the size of the workgroup.
/// Then you can save on two things:
/// preparing hardware for calling the kernel
/// and accessing memory (access to local memory is faster than to global memory).
///======================================================================================
///======================================================================================
//

#include "BitonicSort.hpp"

#include <fstream>
#include <sstream>
#include <iostream>

#ifdef DEBUG
#define DEBUG_ACTION(act) { act }

#else
#define DEBUG_ACTION(act) {}

#endif //DEBUG

namespace ezg
{

    BitonicSort_t::BitonicSort_t(const cl::Device& device)
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

        m_maxWorkGroupSize = device.getInfo< CL_DEVICE_MAX_WORK_GROUP_SIZE >();
        try {
            std::string options(std::string(" -D DATA_TYPE=int ")
                                + "-D GROUP_SIZE=" + std::to_string(m_maxWorkGroupSize));

            //DEBUG_ACTION(options += " -D DEBUG ";);

            m_program.build(options.c_str());
        }
        catch (const std::exception& ex) {
            std::cerr << ex.what() << std::endl;
            std::cerr << m_program.getBuildInfo<CL_PROGRAM_BUILD_LOG>().data()->second << std::endl;
            throw ex;
        }
    }


    void BitonicSort_t::sort(std::vector< int >& source) const
    {
        const size_t size_vec = source.size();

        size_t extended_size = 2;
        while(extended_size < size_vec) {
            extended_size <<= 1;
        }

        std::vector<int> data(extended_size, std::numeric_limits< int >::max());
        for (size_t k = 0; k < size_vec; k++) {
            data[k] = source[k];
        }

        cl::Buffer cl_buff(m_context, CL_MEM_READ_WRITE, extended_size * sizeof(int));

        m_commandQueue.enqueueWriteBuffer(cl_buff, CL_TRUE, 0,
                                          extended_size * sizeof(int),
                                          data.data());

        size_t global_size = extended_size / 2;
        size_t local_size = std::min(global_size, m_maxWorkGroupSize);

        cl::Kernel kernel_default(m_program, "bitonic_sort_kernel_default");
        cl::Kernel kernel_local  (m_program, "bitonic_sort_kernel_local");


        unsigned int stage = 0, passOfStage = 0, numStages = 0, temp = 0;
        for (temp = extended_size; temp > 1; temp >>= 1)
            ++numStages;

        size_t maxDifferenceStageAndPass = 0;
        for( ; 1u << maxDifferenceStageAndPass < local_size; maxDifferenceStageAndPass++) {}



        if(numStages <= maxDifferenceStageAndPass)
        {
            // the size of the array is less than the size of the workgroup * 2
            // and can be processed by the kernel running on local memory
            kernel_local.setArg(0, cl_buff);
            kernel_local.setArg(1, 0);
            kernel_local.setArg(2, numStages);
            kernel_local.setArg(3, 0);

            cl::Event event;
            m_commandQueue.enqueueNDRangeKernel(kernel_local, 0,
                                                global_size, local_size,
                                                nullptr, &event);
            event.wait();
        }
        else
        {
            for (stage = 0; stage < numStages; ++stage) {

                for (passOfStage = 0; passOfStage < stage + 1; ++passOfStage)
                {
                    if (stage - passOfStage <= maxDifferenceStageAndPass)
                    {
                        // several sub-passes can be processed on a kernel with local memory,
                        // since the distance between compared objects fits into the local
                        // memory of the kernel. (optimization)
                        kernel_local.setArg(0, cl_buff);
                        kernel_local.setArg(1, stage);
                        kernel_local.setArg(2, stage + 1);
                        kernel_local.setArg(3, passOfStage);

                        cl::Event event;
                        m_commandQueue.enqueueNDRangeKernel(kernel_local, 0,
                                                            global_size, local_size,
                                                            nullptr, &event);
                        event.wait();
                        break;
                    }

                    kernel_default.setArg(0, cl_buff);
                    kernel_default.setArg(1, stage);
                    kernel_default.setArg(2, passOfStage);

                    cl::Event event;
                    m_commandQueue.enqueueNDRangeKernel(kernel_default, 0,
                                                      global_size, local_size,
                                                      nullptr, &event);
                    event.wait();
                }//end of for passStage = 0:stage-1
            }//end of for stage = 0:numStage-1*/
        }

        int* res_data = (int*)m_commandQueue.enqueueMapBuffer(cl_buff,
                                                              CL_TRUE, CL_MAP_READ, 0,
                                                              size_vec * sizeof(int));
        for(size_t i = 0; i < size_vec; i++)
            source[i] = res_data[i];
        m_commandQueue.enqueueUnmapMemObject(cl_buff, res_data);

    }

}//namespace ezg