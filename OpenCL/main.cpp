#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <memory>
#include <cstring>

#include <CL/cl.h>

#include <CL/opencl.h>
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

    float alpha = 2.0;
    std::vector<int> data(size_vec);

    for (size_t k = 0; k < size_vec; k++) {
        data[k] = size_vec - k;
    }

    // Get platform and device information
    cl_uint num_platforms = {};
    cl_int clStatus = clGetPlatformIDs(0, nullptr, &num_platforms);
    std::vector<cl_platform_id> platforms(num_platforms);
    clStatus = clGetPlatformIDs(num_platforms, platforms.data(), nullptr);

    //Get the devices list and choose the device you want to run on
    cl_uint num_device = {};
    clStatus = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_GPU, 0, nullptr, &num_device);
    std::vector<cl_device_id> device_list(num_device);
    clStatus = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_GPU, num_device, device_list.data(), nullptr);

    // Create one OpenCL context for each device in the platform
    cl_context context = clCreateContext(nullptr, num_device, device_list.data(), nullptr, nullptr, &clStatus);

    // Create a command queue
    cl_command_queue commandQueue = clCreateCommandQueue(context, device_list[0], 0, &clStatus);

    // Create memory buffers on the device for each vector
    cl_mem A_clmem = clCreateBuffer(context, CL_MEM_READ_ONLY, size_vec * sizeof(float), nullptr, &clStatus);

    // Copy the Buffer A and B to the device
    clStatus = clEnqueueWriteBuffer(commandQueue, A_clmem, CL_TRUE, 0, size_vec * sizeof(float), data.data(), 0,
                                    nullptr, nullptr);

    std::string sax_kernel = readFile("kernel.cl");
    const char *pSaxKernel = sax_kernel.data();
    // Create a program from the kernel source
    cl_program program = clCreateProgramWithSource(context, 1, &pSaxKernel, nullptr, &clStatus);

    // Build the program
    clStatus = clBuildProgram(program, 1, device_list.data(), nullptr, nullptr, nullptr);
    LOG_OCL_COMPILER_ERROR(program, device_list[0]);
    // Create the OpenCL kernel
    cl_kernel kernel = clCreateKernel(program, "bitonic_sort_kernel", &clStatus);

    size_t global_size = size_vec / 2;
    // This is the size of the work group.
    size_t local_size = global_size;
    // Calculate the Number of work groups.
    size_t num_of_work_groups = global_size / local_size;


    DATA_TYPE *mapped_input_buffer = (DATA_TYPE *) clEnqueueMapBuffer(commandQueue, A_clmem, true, CL_MAP_READ, 0,
                                                                      sizeof(DATA_TYPE) * size_vec, 0, NULL, NULL,
                                                                      &clStatus);
    // Display the Sorted data on the screen
    for (int i = 0; i < size_vec; i++)
        printf("%d ", mapped_input_buffer[i]);
    fflush(stdout);
    clEnqueueUnmapMemObject(commandQueue, A_clmem, mapped_input_buffer, 0, nullptr, nullptr);


    clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *) &A_clmem);
    unsigned int stage, passOfStage, numStages, temp;
    stage = passOfStage = numStages = 0;
    for (temp = size_vec; temp > 1; temp >>= 1)
        ++numStages;
    global_size = size_vec >> 1;
    local_size = global_size;
    for (stage = 0; stage < numStages; ++stage) {
        // stage of the algorithm
        clSetKernelArg(kernel, 1, sizeof(int), (void *) &stage);
        // Every stage has stage + 1 passes
        for (passOfStage = 0; passOfStage < stage + 1; ++passOfStage) {
            // pass of the current stage
            std::cout << "Pass no " << passOfStage << std::endl;
            clStatus = clSetKernelArg(kernel, 2, sizeof(int), (void *) &passOfStage);
            //
            // Enqueue a kernel run call.
            // Each thread writes a sorted pair.
            // So, the number of threads (global) should be half the length of the input buffer.

            clEnqueueNDRangeKernel(commandQueue, kernel, 1, nullptr, &global_size, &local_size, 0, NULL, NULL);

            clFinish(commandQueue);

            DATA_TYPE *mapped_input_buffer = (DATA_TYPE *) clEnqueueMapBuffer(commandQueue, A_clmem, true, CL_MAP_READ, 0,
                                                                              sizeof(DATA_TYPE) * size_vec, 0, NULL, NULL,
                                                                              &clStatus);
            // Display the Sorted data on the screen
            for (int i = 0; i < size_vec; i++)
                printf("%d ", mapped_input_buffer[i]);
            fflush(stdout);

            clEnqueueUnmapMemObject(commandQueue, A_clmem, mapped_input_buffer, 0, nullptr, nullptr);
        }//end of for passStage = 0:stage-1
    }//end of for stage = 0:numStage-1


    // Finally release all OpenCL allocated objects and host buffers.
    clStatus = clReleaseKernel(kernel);
    clStatus = clReleaseProgram(program);
    clStatus = clReleaseMemObject(A_clmem);
    clStatus = clReleaseCommandQueue(commandQueue);
    clStatus = clReleaseContext(context);

    return 0;
}