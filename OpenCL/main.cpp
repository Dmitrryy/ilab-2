#define CL_HPP_ENABLE_EXCEPTIONS
#include <CL/cl2.hpp>

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <cstring>
#include <cmath>

#include "gen_test.h"


std::string readFile(const std::string &fileName) {
    std::ifstream f(fileName);
    if (!f) {
        throw std::runtime_error("cant open file: " + fileName);
    }
    std::stringstream ss;
    ss << f.rdbuf();
    return ss.str();
}


template< typename T, typename U >
void gen_test(std::basic_ostream< T >& outTest, std::basic_ostream< U >& outAns, size_t size)
{
    std::vector< int > test;
    test.reserve(size);

    Random rand(-10000, 10000);
    outTest << size << std::endl;
    for (size_t k = 0; k < size; k++) {
        test.push_back(rand());
        outTest << test.back() << ' ';
    }

    std::sort(test.begin(), test.end());

    for (size_t k = 0; k < size; k++) {
        outAns << test[k] << ' ';
    }
}

#ifdef DEBUG
#define DEBUG_ACTION(act) { act }

#else
#define DEBUG_ACTION(act) {}

#endif //DEBUG

//#define GEN_TESTS_

int main() {
#ifdef GEN_TESTS_
    std::ofstream test("../../OpenCL/tests/1.txt");
    std::ofstream ans("../../OpenCL/tests/1a.txt");

    gen_test(test, ans, 2100);
    return 1;
#endif

    freopen("tests/1.txt", "r", stdin);
    freopen("tests/my.txt", "w", stdout);

    size_t size_vec = 0;
    std::cin >> size_vec;

    size_t extended_size = 2;
    while(extended_size < size_vec) {
        extended_size <<= 1;
    }

    //DEBUG_ACTION(std::cout << "natural vector size: " << extended_size << std::endl;);

    std::vector<int> data(extended_size, std::numeric_limits< int >::max());
    for (size_t k = 0; k < size_vec; k++) {
        std::cin >> data[k];
    }

    cl::Platform platform;

    std::vector< cl::Device > devices;
    platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);

    cl::Context context(devices[0]);

    cl::CommandQueue commandQueue(context, devices.at(0), CL_QUEUE_PROFILING_ENABLE);

    cl::Buffer a_buff(context, CL_MEM_READ_WRITE, extended_size * sizeof(int));
    cl::Buffer cmp_buff(context, CL_MEM_READ_WRITE, extended_size * sizeof(bool));
    commandQueue.enqueueWriteBuffer(a_buff, CL_TRUE, 0, extended_size * sizeof(int), data.data());

    std::string source_kernel = readFile("kernel.cl");
    cl::Program program(context, source_kernel);

    try {
        std::string options(" -D DATA_TYPE=int ");

        //DEBUG_ACTION(options += " -D DEBUG ";);

        program.build(options.data());
    }
    catch (const std::exception& ex) {
        std::cerr << ex.what() << std::endl;
        std::cerr << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>().data()->second << std::endl;
        return 1;
    }

    cl::Kernel kernel_default(program, "bitonic_sort_kernel_default");
    cl::Kernel kernel_cmp(program, "bitonic_sort_compare_kernel");
    cl::Kernel kernel_swap(program, "bitonac_sort_swap_kernel");

    size_t global_size = extended_size / 2;
    size_t local_size = std::min(global_size, devices[0].getInfo< CL_DEVICE_MAX_WORK_GROUP_SIZE >());
    size_t num_of_work_groups = global_size / local_size;

    unsigned int stage = 0, passOfStage = 0, numStages = 0, temp = 0;
    for (temp = extended_size; temp > 1; temp >>= 1)
        ++numStages;

    for (stage = 0; stage < numStages; ++stage) {

        for (passOfStage = 0; passOfStage < stage + 1; ++passOfStage)
        {
/*            DEBUG_ACTION(std::cout << "Stage " << stage << ", Pass no " << passOfStage
            << ": global size " << global_size << ", local size " << local_size <<  std::endl;
            );*/

/*            kernel_cmp.setArg(0, a_buff);
            kernel_cmp.setArg(1, cmp_buff);
            kernel_cmp.setArg(2, stage);
            kernel_cmp.setArg(3, passOfStage);

            cl::Event event;
            commandQueue.enqueueNDRangeKernel(kernel_cmp, 0, global_size, local_size, nullptr, &event);
            event.wait();


            cl::Event event2;
            kernel_swap.setArg(0, a_buff);
            kernel_swap.setArg(1, cmp_buff);
            kernel_swap.setArg(2, stage);
            kernel_swap.setArg(3, passOfStage);

            commandQueue.enqueueNDRangeKernel(kernel_swap, 0, global_size, local_size, nullptr, &event2);
            event2.wait();*/
    /*            DEBUG_ACTION(auto map_data = (int*)commandQueue.enqueueMapBuffer(a_buff, CL_TRUE, CL_MAP_READ, 0, extended_size * sizeof(int));
                    for(size_t i = 0; i < extended_size; i++)
                        std::cout << map_data[i] << ' ';
                    std::cout << std::endl << std::endl;
                    commandQueue.enqueueUnmapMemObject(a_buff, map_data);*/
    //            );
            kernel_default.setArg(0, a_buff);
            kernel_default.setArg(1, stage);
            kernel_default.setArg(2, passOfStage);
            cl::Event event;
            commandQueue.enqueueNDRangeKernel(kernel_default, 0, global_size, local_size, nullptr, &event);
            event.wait();
        }//end of for passStage = 0:stage-1
    }//end of for stage = 0:numStage-1*/

    int* res_data = (int*)commandQueue.enqueueMapBuffer(a_buff, CL_TRUE, CL_MAP_READ, 0, extended_size * sizeof(int));
    for(size_t i = 0; i < size_vec; i++)
        std::cout << res_data[i] << ' ';
    std::cout << std::endl;
    commandQueue.enqueueUnmapMemObject(a_buff, res_data);

    return 0;
}