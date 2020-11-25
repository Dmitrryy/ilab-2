////
//// Created by dmitry on 10.11.2020.
////
//#pragma once
//
//#define GLFW_INCLUDE_VULKAN
//#include <GLFW/glfw3.h>
//
//#include <iostream>
//
//
//namespace vks {
//
//    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
//    {
//        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
//            glfwSetWindowShouldClose(window, GL_TRUE);
//    }
//
//    class WindowControl
//    {
//        GLFWwindow* m_window = nullptr;
//
//
//        //int m_height = 0;
//        //int m_width  = 0;
//
//    public:
//
//        WindowControl() {};
//        virtual ~WindowControl() = default;
//
//        bool shouldClose() const { return glfwWindowShouldClose(m_window); }
//
//        bool resized() {
//            bool result = m_framebufferResized;
//            m_framebufferResized = false;
//            return result;
//        }
//
//        int getWidth() {
//            int res = 0;
//            glfwGetWindowSize(m_window, &res, nullptr);
//            return res;
//        }
//        int getHeight() {
//            int res = 0;
//            glfwGetWindowSize(m_window, nullptr, &res);
//            return res;
//        }
//
//        void getCursorPos(double* x, double* y) { glfwGetCursorPos(m_window, x, y); }
//        void setCursorPos(double x, double y) { glfwSetCursorPos(m_window, x, y); }
//        int  getKey(int key) { return glfwGetKey(m_window, key); }
//
//        void setInputMode(int mode, int value) { glfwSetInputMode(m_window, mode, value); }
//        void setWindowUserPointer(void* point) { glfwSetWindowUserPointer(m_window, point); }
//        void setFramebufferSizeCallback(GLFWframebuffersizefun fun) { glfwSetFramebufferSizeCallback(m_window, fun); }
//        void setKeyCallback(GLFWkeyfun fun) { glfwSetKeyCallback(m_window, fun); }
//
//    public:
//
//        bool Init(uint32_t width_, uint32_t height_)
//        {
//            glfwInit();
//
//            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
//
//            m_window = glfwCreateWindow(width_, height_, "Vulkan", nullptr, nullptr);
//            //glfwSetWindowUserPointer(m_window, this);
//            //glfwSetFramebufferSizeCallback(m_window, framebufferResizeCallback);
//            setInputMode (GLFW_STICKY_KEYS, 1);
//
//            //glfwSetKeyCallback(m_window, keyCallback);
//
//            return true;
//        }
//
//        VkSurfaceKHR createSurface(VkInstance& inst_)
//        {
//            VkSurfaceKHR surface = {};
//
//            if (glfwCreateWindowSurface(inst_, m_window, nullptr, &surface) != VK_SUCCESS) {
//                throw std::runtime_error("failed to create window surface!");
//            }
//
//            return surface;
//        }
//    };
//
//
///*    class XCBControl : public WindowControl
//    {
//        xcb_connection_t* m_pXCBConn;
//        xcb_screen_t*     m_pXCBScreen;
//        xcb_window_t      m_xcbWindow;
//
//    public:
//
//        XCBControl();
//        virtual ~XCBControl();
//
//        bool Init(uint32_t width_, uint32_t height_) override;
//        VkSurfaceKHR createSurface(VkInstance& inst_) override;
//    };*/
//
//}//namespace vks