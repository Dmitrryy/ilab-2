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
#define CL_HPP_TARGET_OPENCL_VERSION 210
#include <CL/cl2.hpp>

#include <vector>
#include <string>
#include <unordered_map>


#include <matrix/Matrix.h>


namespace ezg
{
    class PatternMatchingGPU
    {
        //denotes not a symbol
        //for example, there is no pattern with such coordinates in the signature table
        static cl_float nons;

        cl::Context m_context;
        cl::CommandQueue m_commandQueue;

        cl::Program m_program;
        size_t m_maxWorkGroupSize = 0;

        //TODO settings
        std::string m_kernelSourceName = "kernel.cl";

        double m_lastTime = 0;
        size_t m_maxFramesInFly = 4;


    public:

        /// constructor.
        /// The constructor fully prepares the library for sorting
        /// (compiling kernels, creating a command queue, and so on).
        /// \param device - the device on which the sorting will be performed.
        PatternMatchingGPU(const cl::Device &device);

    public:

        /// starts searching for patterns in a string.
        /// \param string  - the string to search in.
        /// \param patterns - what to look for
        /// \return a hash table, where the key is the pattern number,
        /// the value is a vector of positions where the corresponding pattern was found.
        std::unordered_map< size_t, std::vector< size_t > >
        match(const std::string &string
              , const std::vector< std::string > &patterns);


        ///
        /// \return
        double getLastTime() const { return m_lastTime; }

    private:
        //what scary types ...
        using PatternTable = matrix::Matrix< std::vector< std::pair< size_t, std::string > > >;

        /// sorts patterns by prefix (first two letters)
        /// \param patterns - sortable patterns
        /// \return matrix, where the patterns with the prefix "s1s2" lie under
        /// the coordinates ('s1', 's2').
        /// \note To save the pattern number in the general array, the lines
        /// are wrapped in a pair containing the corresponding pattern number
        PatternTable
        buildPatternsTable(const std::vector< std::string > &patterns);


        /// builds a signature table that can be loaded into the GPU
        /// \param patTable - result of buildPatternsTable
        /// \param step     - depth ... at stage == 4, patterns with index 4
        /// will be taken from the table (the table contains vectors of patterns)
        /// \return pixel matrix (signature table)
        matrix::Matrix< cl_float4 > buildSignatureTable(const PatternTable &patTable
                                                    , size_t step);

        /// the filtering algorithm does not work for patterns less than 6 characters in size.
        /// This function checks all small patterns on the CPU, leaving the big ones for the GPU.
        /// \param str - the string to search in.
        /// \param patterns - what to look for
        /// \return part of the overall result
        std::unordered_map< size_t, std::vector< size_t > >
        checkSmallPatterns(const std::string& str, const std::vector< std::string >& patterns);

        /// used to check for a possible match
        /// \param str     - the string to search in.
        /// \param pos     - the position at which a possible match was recorded
        /// \param pattern - what to look for
        /// \return is there a match or not
        bool checkMatch(const std::string& str, size_t pos, const std::string& pattern);


        size_t getMaxDepthPatternTable(const PatternTable& table) const;
    };


}//namespace ezg