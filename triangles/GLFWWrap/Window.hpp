/*************************************************************************************************
 *
 *   Window.hpp
 *
 *   Created by dmitry
 *   11.04.2021
 *
 ***/

#pragma once


#include "GLFWWrap.hpp"

#include <vector>
#include <string>


namespace ezg
{

    class Window final
    {
        GLFWWrap    m_glfwInstance;
        GLFWwindow* m_pWindow = nullptr;

    public:

        Window(int width, int height
               , const std::string& title
               , GLFWmonitor *monitor
               , GLFWwindow *share
               , const std::vector< std::pair< int, int > >& hints);

        Window           (const Window&) = delete; //not supported
        Window& operator=(const Window&) = delete; //not supported

        Window           (Window&&)      = default;
        Window& operator=(Window&&)      = default;

        ~Window();

    public:

        GLFWwindow* get() const noexcept { return m_pWindow; }

        void setUserPointer(void* pointer) const;

        void setKeyCallback(GLFWkeyfun func) const;

        void setInputMode(int mode, int value) const;


        int getHeight() const;
        int getWidth() const;


        bool shouldClose() const;

        void setCursorPosition(double xPos, double yPos) const;
        double getCursorXPos() const;
        double getCursorYPos() const;

        int getKey(int key) const;

#if defined(GLFW_INCLUDE_VULKAN)
        VkSurfaceKHR createSurface(VkInstance instance) const
        {
            VkSurfaceKHR res = nullptr;

            if (glfwCreateWindowSurface(instance, m_pWindow, nullptr, &res) != VK_SUCCESS) {
                throw std::runtime_error(("failed to create window surface!"));
            }

            return res;
        }
#endif
    };


}//namespace ezg