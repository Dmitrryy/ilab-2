//
// Created by dmitry on 10.11.2020.
//
#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>

namespace vks {

    class WindowControl
    {
        GLFWwindow* m_window;
        bool framebufferResized = false;

    public:

        WindowControl() {};
        virtual ~WindowControl() = default;

    public:

        bool Init(uint32_t width_, uint32_t height_)
        {
            glfwInit();

            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

            m_window = glfwCreateWindow(width_, height_, "Vulkan", nullptr, nullptr);
            glfwSetWindowUserPointer(m_window, this);
            glfwSetFramebufferSizeCallback(m_window, framebufferResizeCallback);
            glfwSetInputMode (m_window, GLFW_STICKY_KEYS, 1);

            return true;
        }

        VkSurfaceKHR createSurface(VkInstance& inst_)
        {
            VkSurfaceKHR surface = {};

            if (glfwCreateWindowSurface(inst_, m_window, nullptr, &surface) != VK_SUCCESS) {
                throw std::runtime_error("failed to create window surface!");
            }

            return surface;
        }

        static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
            auto app = reinterpret_cast<WindowControl*>(glfwGetWindowUserPointer(window));
            app->framebufferResized = true;
        }
    };


/*    class XCBControl : public WindowControl
    {
        xcb_connection_t* m_pXCBConn;
        xcb_screen_t*     m_pXCBScreen;
        xcb_window_t      m_xcbWindow;

    public:

        XCBControl();
        virtual ~XCBControl();

        bool Init(uint32_t width_, uint32_t height_) override;
        VkSurfaceKHR createSurface(VkInstance& inst_) override;
    };*/

}//namespace vks