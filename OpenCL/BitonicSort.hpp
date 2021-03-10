/*************************************************************************************************
 *
 *   BitonicSort.hpp
 *
 *   Created by dmitry
 *   10.03.2021
 *
 ***/

#pragma once


#define CL_HPP_ENABLE_EXCEPTIONS
#include <CL/cl2.hpp>

#include <vector>
#include <string>


namespace ezg
{

    /// driver class, for sorting an array using multi-processor devices
    class BitonicSort_t
    {
        cl::Context      m_context;
        cl::CommandQueue m_commandQueue;

        cl::Program      m_program;
        size_t           m_maxWorkGroupSize = 0;

        std::string      m_kernelSourceName = "kernel.cl";

    public:

        /// constructor.
        /// The constructor fully prepares the library for sorting
        /// (compiling kernels, creating a command queue, and so on).
        /// \param device - the device on which the sorting will be performed.
        explicit BitonicSort_t(const cl::Device& device);

    public:

        /// Function that starts sorting.
        /// On exit, a sorted array is written to source.
        /// \param source - the vector to be sorted.
        ///                 The result will be written to it
        void sort(std::vector< int >& source) const;
    };// class BitonicSort_t


}//namespace ezg