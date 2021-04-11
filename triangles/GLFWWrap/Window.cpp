/*************************************************************************************************
 *
 *   Window.cpp
 *
 *   Created by dmitry
 *   11.04.2021
 *
 ***/

#include "Window.hpp"

namespace ezg
{

    Window::Window(int width, int height
                   , const std::string& title
                   , GLFWmonitor* monitor
                   , GLFWwindow* share
                   , const std::vector< std::pair< int, int > >& hints)
    {
        glfwDefaultWindowHints();
        for (auto&& h : hints) {
            glfwWindowHint(h.first, h.second);
        }

        m_pWindow = glfwCreateWindow(width, height
                                     , title.c_str()
                                     , monitor, share);
        if (m_pWindow == nullptr) {
            throw std::runtime_error("failed create window");
        }
    }

    Window::~Window()
    {
        glfwDestroyWindow(m_pWindow);
    }


    void Window::setUserPointer(void* pointer) const
    {
        glfwSetWindowUserPointer(m_pWindow, pointer);
    }

    void Window::setKeyCallback(GLFWkeyfun func) const
    {
        glfwSetKeyCallback(m_pWindow, func);
    }

    void Window::setInputMode(int mode, int value) const
    {
        glfwSetInputMode(m_pWindow, mode, value);
    }


    int Window::getHeight() const
    {
        int height = 0, width = 0;
        glfwGetWindowSize(m_pWindow, &width, &height);
        return height;
    }

    int Window::getWidth() const
    {
        int height = 0, width = 0;
        glfwGetWindowSize(m_pWindow, &width, &height);
        return width;
    }

    bool Window::shouldClose() const
    {
        return glfwWindowShouldClose(m_pWindow);
    }


    void Window::setCursorPosition(double xPos, double yPos) const
    {
        glfwSetCursorPos(m_pWindow, xPos, yPos);
    }


    double Window::getCursorXPos() const
    {
        double x = 0, y = 0;
        glfwGetCursorPos(m_pWindow, &x, &y);
        return x;
    }

    double Window::getCursorYPos() const
    {
        double x = 0, y = 0;
        glfwGetCursorPos(m_pWindow, &x, &y);
        return y;
    }

    int Window::getKey(int key) const
    {
        return glfwGetKey(m_pWindow, key);
    }


}//namespace ezg