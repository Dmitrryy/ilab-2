/*************************************************************************************************
 *
 *   PatternMatching.hpp
 *
 *   Created by dmitry
 *   18.03.2021
 *
 ***/

#pragma once


#define CL_HPP_ENABLE_EXCEPTIONS
#include <CL/cl2.hpp>

#include <vector>
#include <string>
#include <unordered_map>

#include <glm/glm.hpp>

#include "../../../Matrix/Libs/matrix/Matrix.h"


namespace ezg
{
    class PatternMatchingGPU
    {
        cl::Context m_context;
        cl::CommandQueue m_commandQueue;

        cl::Program m_program;
        size_t m_maxWorkGroupSize = 0;

        std::string m_kernelSourceName = "kernel.cl";


    public:

        PatternMatchingGPU(const cl::Device &device);


    public:

        std::unordered_map< size_t, std::vector< size_t > >
        match(const std::string &string
              , const std::vector< std::string > &patterns);

    private:

        matrix::Matrix< std::vector< std::string > >
        buildPatternsTable(const std::vector< std::string > &patterns);

        matrix::Matrix< glm::vec4 > buildSignatureTable(const matrix::Matrix< std::vector< std::string > > &patTable);

    };

}//namespace ezg