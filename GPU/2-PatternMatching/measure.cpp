/*************************************************************************************************
 *
 *   measure.cpp
 *
 *   Created by dmitry
 *   20.03.2021
 *
 ***/

#include "scan_input.hpp"
#include "../../OtherLibs/timer.h"
#include "native_cpu/native_cpy.hpp"
#include "filter_GPU/PatternMatching.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>


namespace ezg
{

    void measure_main(const cl::Device& device)
    {
        std::string tests_dir_name = "tests";
        std::string extension = ".txt";

        std::vector< std::string > test_files;
        for (auto &p : std::filesystem::directory_iterator(tests_dir_name))
        {
            if (!std::filesystem::is_regular_file(p.status()))
                continue;

            std::string name(p.path().filename());

            bool match = !name.compare(name.size() - extension.size(), extension.size(), extension);

            if(match)
            {
                test_files.emplace_back(tests_dir_name + '/' + name);
            }
        }


        for(auto&& test : test_files)
        {
            std::cout << "[start test]=====================================================" << std::endl;
            std::cout << "file name is \'" << test << '\'' << std::endl;

            std::fstream input(test);
            if (!input.is_open()) {
                std::cout << "cant open file!" << std::endl;
            }
            else {
                auto &&data = getData2(input);
                size_t average_size_pattern = 0;
                std::for_each(data.second.begin(), data.second.end(), [&average_size_pattern](const std::string &vec)
                {
                    average_size_pattern += vec.size();
                });
                average_size_pattern /= data.second.size();
                std::cout << "string size          = " << data.first.size() << std::endl;
                std::cout << "num patterns         = " << data.second.size() << std::endl;
                std::cout << "average pattern size = " << average_size_pattern << std::endl;

                ezg::PatternMatchingGPU pmg(device);
                ezg::Timer timer;
                auto &&res1 = pmg.match(data.first, data.second);
                double timeGPU = timer.elapsed();

                timer.reset();
                auto &&res2 = ezg::countPatternMatches(data.first, data.second);
                double timeCPU = timer.elapsed();

                std::cout << "Time results: " << std::endl;
                std::cout << "with GPU filter = " << timeGPU << "sec\n";
                std::cout << "native CPU      = " << timeCPU << "sec\n";

                for(size_t i = 1, mi = data.second.size(); i < mi; ++i) {
                    if(res1[i].size() != res2[i]) {
                        std::cerr << "hmmmmmmmmmmmmmmmm\n";
                        std::cerr << '[' << i << ']' << res1[i].size() << ' ' << res2[i] << std::endl;
                    }
                }
            }
            std::cout << "[end test]=======================================================" << std::endl << std::endl;
        }
    }

}// namespace ezg