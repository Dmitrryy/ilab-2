/*************************************************************************************************
 *
 *   main.cpp
 *
 *   Created by dmitry
 *   10.03.2021
 *
 ***/

//
/// BitonicSort
///======================================================================================
/// This is the first level for working with API OpenCL.
/// Challenge: sort the array using the bitonic sort algorithm and compare the results
/// with the standard library sort.
///======================================================================================
///======================================================================================
//

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <cassert>


#include "gen_test.h"
#include "BitonicSort.hpp"
#include "../OtherLibs/timer.h"


//#define GEN_TESTS_

int main() {
#ifdef GEN_TESTS_
    std::ofstream test("../../OpenCL/tests/6.txt");
    std::ofstream ans("../../OpenCL/tests/6a.txt");

    gen_test(test, ans, 200000);
    return 1;
#endif

#ifdef NDEBUG
    auto&& in = std::cin;
    auto&& out = std::cout;
#else
    auto&& in = std::ifstream("tests/4.txt");
    auto&& out = std::ofstream("tests/my.txt");
    //auto&& out = std::cout;

#endif //NDEBUG


    size_t size_vec = 0;
    in >> size_vec;


    std::vector<int> data(size_vec);
    for (size_t k = 0; k < size_vec; k++) {
        in >> data[k];
    }
    auto data1 = data;
    auto data2 = data;

    std::vector< cl::Platform > platforms;
    cl::Platform::get(&platforms);
    if(platforms.empty()) {
        throw std::runtime_error("cant find any platform!");
    }

    cl::Device res_device;
    for(auto&& pl : platforms) {
        try {
            std::vector< cl::Device > devices;
            pl.getDevices(CL_DEVICE_TYPE_ALL, &devices);

            for (auto&& dev : devices) {
                if (dev.getInfo< CL_DEVICE_COMPILER_AVAILABLE >()) {
                    res_device = dev;
                    break;
                }
            }
        }
        catch (std::exception& ex) {
            ex.what();
        }
        if (res_device != cl::Device{}) {
            break;
        }
    }

    if (res_device == cl::Device{}) {
        throw std::runtime_error("cant find any device");
    }

    ezg::BitonicSort_t driver(res_device);

    ezg::Timer timer;
    timer.reset();
    driver.sort(data1);
    const double bsTime = timer.elapsed();

#ifdef MEASUREMENTS
    std::cout << "bitonic sort time: " << bsTime << "sec" << std::endl;


    timer.reset();
    std::sort(data2.begin(), data2.end());
    const double stdTime = timer.elapsed();

    std::cout << "std sort time: " << stdTime << "sec\n" << std::endl;

    for (size_t i = 0; i < size_vec; ++i) {
        assert(data1[i] == data2[i]);
    }
#endif

    for(size_t i = 0; i < size_vec; ++i) {
        out << data1[i] << ' ';
    }
    out << std::endl;

    return 0;

}