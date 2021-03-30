/*************************************************************************
 *
 * helpa.cpp
 *
 * Created by dmitry
 * 22.02.2021
 *
 ***/

#include "helpa.h"


namespace ezg
{

    std::vector<char> readFile(const std::string& filename) 
    {
        std::ifstream file(filename, std::ios::ate | std::ios::binary);

        if (!file.is_open()) {
            throw std::runtime_error("failed to open file with name: " + filename);
        }

        size_t fileSize = file.tellg();
        std::vector< char > buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);

        file.close();

        return buffer;
    }

}