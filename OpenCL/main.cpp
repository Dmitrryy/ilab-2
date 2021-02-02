
//#include "Wrapper.h"

#define CL_HPP_ENABLE_EXCEPTIONS
#include <CL/cl2.hpp>

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <memory>
#include <cstring>

#include <algorithm>
#include <cmath>

#ifdef DEBUG

#define LOG_OCL_COMPILER_ERROR(PROGRAM, DEVICE)\
{\
 cl_int logStatus;\
 char * buildLog = NULL;\
 size_t buildLogSize = 0;\
 logStatus = clGetProgramBuildInfo(PROGRAM,\
 DEVICE,\
 CL_PROGRAM_BUILD_LOG,\
 buildLogSize,\
 buildLog,\
 &buildLogSize);\
 if(logStatus != CL_SUCCESS)\
 {\
 std::cout << "Error # "<< logStatus\
 <<":: clGetProgramBuildInfo<CL_PROGRAM_BUILD_LOG>failed.";\
 exit(1);\
 }\
\
 buildLog = (char*)malloc(buildLogSize);\
 if(buildLog == NULL)\
 {\
 std::cout << "Failed to allocate host memory.(buildLog)\n";\
 return -1;\
 }\
 memset(buildLog, 0, buildLogSize);\
 logStatus = clGetProgramBuildInfo(PROGRAM,\
 DEVICE,\
 CL_PROGRAM_BUILD_LOG,\
 buildLogSize,\
 buildLog,\
 NULL);\
 if(logStatus != CL_SUCCESS)\
 {\
 std::cout << "Error # "<< logStatus\
 <<":: clGetProgramBuildInfo<CL_PROGRAM_BUILD_LOG>failed.";\
 exit(1);\
 }\
\
std::cout << " \n\t\t\tBUILD LOG\n";\
 std::cout << " ************************************************\n"; \
 std::cout << buildLog << std::endl;\
 std::cout << " ************************************************\n";\
 free(buildLog);\
}
#else

#define LOG_OCL_COMPILER_ERROR(PROGRAM, DEVICE) {}

#endif


#define DATA_TYPE int


std::string readFile(const std::string &fileName) {
    std::ifstream f(fileName);
    if (!f) {
        throw std::runtime_error("cant open file: " + fileName);
    }
    std::stringstream ss;
    ss << f.rdbuf();
    return ss.str();
}


int main() {
    size_t size_vec = 0;
    std::cin >> size_vec;

    size_t extended_size = std::pow(2, std::ceil(std::pow(size_vec, 0.5)));

#ifdef DEBUG
    std::cout << "natural vector size: " << extended_size << std::endl;
#endif

    std::vector<int> data(extended_size);

    for (size_t k = 0; k < size_vec; k++) {
        data[k] = size_vec - k;
    }
    for (size_t k = size_vec; k < extended_size; k++) {
        data[k] = std::numeric_limits< int >::max();
    }

    cl::Platform platform;

    std::vector< cl::Device > devices;
    platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);

    cl::Context context(devices);

    cl::CommandQueue commandQueue(context, devices.at(0), CL_QUEUE_PROFILING_ENABLE);

    cl::Buffer a_buff(context, CL_MEM_READ_WRITE, extended_size * sizeof(int));

    commandQueue.enqueueWriteBuffer(a_buff, CL_TRUE, 0, extended_size * sizeof(int), data.data());


    std::string source_kernel = readFile("kernel.cl");
    cl::Program program(context, source_kernel, true);

    cl::Kernel kernel(program, "bitonic_sort_kernel");




    size_t global_size = extended_size / 2;
    // This is the size of the work group.
    size_t local_size = std::min(global_size, devices[0].getInfo< CL_DEVICE_MAX_WORK_GROUP_SIZE >());
    // Calculate the Number of work groups.
    size_t num_of_work_groups = global_size / local_size;


    unsigned int stage = 0, passOfStage = 0, numStages = 0, temp = 0;
    for (temp = extended_size; temp > 1; temp >>= 1)
        ++numStages;
    global_size = extended_size / 2;
    for (stage = 0; stage < numStages; ++stage) {
        //global_size = std::pow(2, stage + 1);
        //local_size = extended_size / global_size;
        // stage of the algorithm
        // Every stage has stage + 1 passes
        for (passOfStage = 0; passOfStage < stage + 1; ++passOfStage) {
            // pass of the current stage
            std::cout << "Pass no " << passOfStage << std::endl;
            //
            // Enqueue a kernel run call.
            // Each thread writes a sorted pair.
            // So, the number of threads (global) should be half the length of the input buffer.

            kernel.setArg(0, a_buff);
            kernel.setArg(1, stage);
            kernel.setArg(2, passOfStage);
            cl::Event event;
            commandQueue.enqueueNDRangeKernel(kernel, 0, global_size, local_size, nullptr, &event);

            event.wait();
            //clEnqueueNDRangeKernel(commandQueue.get(), kernel.get(), 1, nullptr, &global_size, &local_size, 0, NULL, NULL);

            auto map_data = (int*)commandQueue.enqueueMapBuffer(a_buff, CL_TRUE, CL_MAP_READ, 0, extended_size * sizeof(int));
            for(size_t i = 0; i < extended_size; i++)
                std::cout << map_data[i] << ' ';
            std::cout << std::endl;
            commandQueue.enqueueUnmapMemObject(a_buff, map_data);

            clFinish(commandQueue.get());

        }//end of for passStage = 0:stage-1
    }//end of for stage = 0:numStage-1


    auto map_data = (int*)commandQueue.enqueueMapBuffer(a_buff, CL_TRUE, CL_MAP_READ, 0, extended_size * sizeof(int));
    for(size_t i = 0; i < extended_size; i++)
        std::cout << map_data[i] << ' ';
    std::cout << std::endl;
    commandQueue.enqueueUnmapMemObject(a_buff, map_data);


    return 0;
}