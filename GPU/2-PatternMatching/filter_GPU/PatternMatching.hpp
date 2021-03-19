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



#include "../../../Matrix/Libs/matrix/Matrix.h"


namespace ezg
{
    class PatternMatchingGPU
    {
        struct vec4
        {
            cl_float x = 0, y = 0, z = 0, w = 0;

            vec4() = default;
            vec4(cl_float e)
                : x(e)
                , y(e)
                , z(e)
                , w(e)
            {}
            vec4(cl_float nx, cl_float ny, cl_float nz, cl_float nw)
                : x(nx)
                , y(ny)
                , z(nz)
                , w(nw)
            {}

            template< class U >
            friend std::basic_ostream< U >& operator<<(std::basic_ostream< U >& stream, const vec4& vec)
            {
                return stream << vec.x << ' ' << vec.y << ' ' << vec.z << ' ' << vec.w;
            }
        };

        static cl_float nons;

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

        matrix::Matrix< std::vector< std::pair< size_t, std::string > > >
        buildPatternsTable(const std::vector< std::string > &patterns);

        matrix::Matrix< vec4 > buildSignatureTable(const matrix::Matrix< std::vector< std::pair< size_t, std::string > > > &patTable
                                                    , size_t step);

        bool checkMatch(const std::string& str, size_t pos, const std::string& pattern);
    };


}//namespace ezg