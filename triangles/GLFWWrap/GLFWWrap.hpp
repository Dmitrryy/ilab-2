
/*************************************************************************************************
 *
 *   GLFWWrap.hpp
 *
 *   Created by dmitry
 *   11.04.2021
 *
 ***/

#pragma once

#include <iostream>

#include <GLFW/glfw3.h>

namespace ezg
{

    class GLFWWrap final
    {
        static size_t g_instanceCounter;

    public:

        GLFWWrap();

        GLFWWrap (const GLFWWrap&) noexcept;
        GLFWWrap (GLFWWrap&&) noexcept;

        GLFWWrap& operator=(const GLFWWrap&) = default;
        GLFWWrap& operator=(GLFWWrap&&) = default;

        ~GLFWWrap();
    };



}//namespace ezg