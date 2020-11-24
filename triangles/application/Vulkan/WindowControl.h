//
// Created by dmitry on 10.11.2020.
//
#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>


namespace vks {

    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
    {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GL_TRUE);
    }

    class WindowControl
    {
        GLFWwindow* m_window = nullptr;
        bool m_framebufferResized = false;

        int m_height = 0;
        int m_width  = 0;

    public:

        WindowControl() {};
        virtual ~WindowControl() = default;

        bool shouldClose() const { return glfwWindowShouldClose(m_window); }

        bool resized() {
            bool result = m_framebufferResized;
            m_framebufferResized = false;
            return result;
        }

        int getWidth() {
            return m_width;
        }
        int getHeight() {
            return m_height;
        }

        void getCursorPos(double* x, double* y) { glfwGetCursorPos(m_window, x, y); }
        void setCursorPos(double x, double y) { glfwSetCursorPos(m_window, x, y); }
        int getKey(int key) { return glfwGetKey(m_window, key); }

        void setInputMode(int mode, int value) { glfwSetInputMode(m_window, mode, value); }

    public:

        bool Init(uint32_t width_, uint32_t height_)
        {
            m_height = height_;
            m_width = width_;

            glfwInit();

            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

            m_window = glfwCreateWindow(m_width, m_height, "Vulkan", nullptr, nullptr);
            glfwSetWindowUserPointer(m_window, this);
            glfwSetFramebufferSizeCallback(m_window, framebufferResizeCallback);
            glfwSetInputMode (m_window, GLFW_STICKY_KEYS, 1);

            glfwSetKeyCallback(m_window, keyCallback);

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
            app->m_framebufferResized = true;
            app->m_height = height;
            app->m_width = width;
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