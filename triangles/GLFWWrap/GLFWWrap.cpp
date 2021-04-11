/*************************************************************************************************
 *
 *   GLFWWrap.cpp
 *
 *   Created by dmitry
 *   11.04.2021
 *
 ***/

#include "GLFWWrap.hpp"


namespace ezg
{

    size_t GLFWWrap::g_instanceCounter = 0;


    GLFWWrap::GLFWWrap()
    {
        // according to the documentation, a repeated call
        // of the function immediately returns success.
        int res = glfwInit();
        if (res != GLFW_TRUE) {
            throw std::runtime_error("failed glfwInit!");
        }
        g_instanceCounter++;
    }


    GLFWWrap::GLFWWrap(const GLFWWrap &) noexcept
    {
        g_instanceCounter++;
    }

    GLFWWrap::GLFWWrap(GLFWWrap &&) noexcept
    {
        g_instanceCounter++;
    }



    GLFWWrap::~GLFWWrap()
    {
        g_instanceCounter--;
        if (g_instanceCounter == 0) {
            glfwTerminate();
        }
    }

}//namespace ezg