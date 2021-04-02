/****************************************************************************************
 *
 *   App.h
 *
 *   Created by dmitry
 *   22.02.2021
 *
 ***/

#pragma once

#include <vector>

#include <OtherLibs/timer.h>
#include "../../LAL/include/LAL.h"
#include <Engine/Engine.hpp>

#include <tinyxml2.h>



///======================================================================================

///======================================================================================
///======================================================================================

namespace ezg
{

    class AppLVL4
    {

        Engine            m_driver;
        GLFWwindow*       m_pWindow = nullptr;

        std::vector< Engine::Mesh* > m_entities;
        la::Rectangle3    m_box = { { -10, -10, -10 }, { 10, 10, 10 } };

        CameraView        m_cameraView;
        float             m_speed = 100.f;

        ezg::Timer        m_time;

    public:

        AppLVL4           (const AppLVL4&) = delete; //not supported
        AppLVL4& operator=(const AppLVL4&) = delete; //not supported
        AppLVL4           (AppLVL4&&)      = delete; //not supported
        AppLVL4& operator=(AppLVL4&&)      = delete; //not supported

        //default destructor
        AppLVL4()
            : m_driver     ("Triangles-4")
            , m_cameraView ({ 2.f, 2.f, 2.f }, { -2.f, -2.f, -2.f})
        {}

        ~AppLVL4() {
            glfwDestroyWindow(m_pWindow);
        }


    public:

        tinyxml2::XMLError loadSceneFromXML(const std::string& fileName);

        /// Launch the app!
        /// A window opens with a visualization of the triangles
        /// added by the addTriangle calls. control returns after the window closes
        /// or after an emergency shutdown.
        /// \note inside the loops, the exit from which is performed by closing the window
        /// \return void
        void run();

    private:

        void update_entities_(float time);
        void update_camera_(float time);

        void clear_();


        void init_();


        static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
        static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
    };

}//namespace ezg