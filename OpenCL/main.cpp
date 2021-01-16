#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <CL/cl.h>

const char* saxpy_kernel = {
        "__kernel                                    \n"
        "void saxpy_kernel(float alpha,                 \n"
        "                   __global float *A,          \n"
        "                   __global float *B,          \n"
        "                   __global float *C)          \n"
        "{                                              \n"
        "   int index = get_global_id(0);               \n"
        "   //printf(\"%f + %f\", A[index], B[index]);  \n"
        "   C[index] = alpha * A[index] + B[index];     \n"
        "}                                              \n"
};

int main()
{
    size_t size_vec = 0;
    std::cin >> size_vec;

    float alpha = 2.0;
    std::vector< float > A(size_vec);
    std::vector< float > B(size_vec);
    std::vector< float > C(size_vec);

    for (size_t k = 0; k < size_vec; k++) {
        A[k] = k;
        B[k] = size_vec - k;
    }

    // Get platform and device information
    cl_uint num_platforms = {};
    cl_int clStatus = clGetPlatformIDs(0, nullptr, &num_platforms);
    std::vector< cl_platform_id > platforms(num_platforms);
    clStatus = clGetPlatformIDs(num_platforms, platforms.data(), nullptr);

    //Get the devices list and choose the device you want to run on
    cl_uint num_device = {};
    clStatus = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_GPU, 0, nullptr, &num_device);
    std::vector< cl_device_id > device_list(num_device);
    clStatus = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_GPU, num_device, device_list.data(), nullptr);

    // Create one OpenCL context for each device in the platform
    cl_context context = clCreateContext(nullptr, num_device, device_list.data(),
                                         nullptr, nullptr, &clStatus);

    // Create a command queue
    cl_command_queue  commandQueue = clCreateCommandQueue(context, device_list[0], 0, &clStatus);

    // Create memory buffers on the device for each vector
    cl_mem A_clmem = clCreateBuffer(context, CL_MEM_READ_ONLY, size_vec * sizeof(float), nullptr, &clStatus);
    cl_mem B_clmem = clCreateBuffer(context, CL_MEM_READ_ONLY, size_vec * sizeof(float), nullptr, &clStatus);
    cl_mem C_clmem = clCreateBuffer(context, CL_MEM_WRITE_ONLY, size_vec * sizeof(float), nullptr, &clStatus);

    // Copy the Buffer A and B to the device
    clStatus = clEnqueueWriteBuffer(commandQueue, A_clmem, CL_TRUE, 0, size_vec * sizeof(float), A.data(), 0, nullptr, nullptr);
    clStatus = clEnqueueWriteBuffer(commandQueue, B_clmem, CL_TRUE, 0, size_vec * sizeof(float), B.data(), 0, nullptr, nullptr);

    // Create a program from the kernel source
    cl_program program = clCreateProgramWithSource(context, 1, &saxpy_kernel, nullptr, &clStatus);

    // Build the program
    clStatus = clBuildProgram(program, 1, device_list.data(), nullptr, nullptr, nullptr);

    // Create the OpenCL kernel
    cl_kernel kernel = clCreateKernel(program, "saxpy_kernel", &clStatus);

    // Set the arguments of the kernel
    clStatus = clSetKernelArg(kernel, 0, sizeof(float), &alpha);
    clStatus = clSetKernelArg(kernel, 1, sizeof(cl_mem), &A_clmem);
    clStatus = clSetKernelArg(kernel, 2, sizeof(cl_mem), &B_clmem);
    clStatus = clSetKernelArg(kernel, 3, sizeof(cl_mem), &C_clmem);

    // Execute the OpenCL kernel on the list
    size_t global_size = size_vec;  // Process the entire lists
    size_t local_size = std::min(size_vec, 64ul);         // Process one item at a time
    clStatus = clEnqueueNDRangeKernel(commandQueue, kernel, 1, nullptr, &global_size, &local_size, 0, nullptr, nullptr);

    // Read the cl memory C_clmem on device to the host variable C
    clStatus = clEnqueueReadBuffer(commandQueue, C_clmem, CL_TRUE, 0, size_vec * sizeof(float), C.data(), 0, nullptr, nullptr);

    // Clean up and wait for all the comands to complete.
    clStatus = clFlush(commandQueue);
    clStatus = clFinish(commandQueue);

    for(size_t i = 0; i < size_vec; i++) {
        std::cout << alpha << " * " << A[i] << " + " << B[i] << " = " << C[i] << std::endl;
    }

    // Finally release all OpenCL allocated objects and host buffers.
    clStatus = clReleaseKernel(kernel);
    clStatus = clReleaseProgram(program);
    clStatus = clReleaseMemObject(A_clmem);
    clStatus = clReleaseMemObject(B_clmem);
    clStatus = clReleaseMemObject(C_clmem);
    clStatus = clReleaseCommandQueue(commandQueue);
    clStatus = clReleaseContext(context);

    return 0;
}