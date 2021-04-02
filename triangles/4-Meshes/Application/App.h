/****************************************************************************************
 *
 *   App.h
 *
 *   Created by dmitry
 *   22.02.2021
 *
 ***/


//
// App
///======================================================================================
/// The application renders various triangular meshes, which can also be moved.
///
/// The Vulkan API is used for rendering.
/// Unlike the previous version (level 3), the written engine allows you to add new
/// objects after initialization. And it allows you to draw not all objects that have
/// been loaded, but only those that have been passed to the drawing function.
/// See engine implementation files for details
///======================================================================================
///======================================================================================
//


#pragma once

#include <vector>

#include <OtherLibs/timer.h>
#include "../../LAL/include/LAL.h"
#include <Engine/Engine.hpp>

#include <tinyxml2.h>


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

            for(auto&& e : m_entities) {
                delete e;
            }
        }


    public:

        /// loads scene information from a file.
        /// e.g. information about camera position & speed & direction, bounding box and objects
        /// \param fileName - file with data of scene
        /// \return result code(enumeration member). If successful, tinyxml2::XML_SUCCESS is returned
        tinyxml2::XMLError loadSceneFromXML(const std::string& fileName);

        /// Launch the app!
        /// A window opens with a visualization of the triangle meshes.
        /// Control returns after the window closes or after an emergency shutdown.
        /// \note inside the loops, the exit from which is performed by closing the window
        /// \return void
        void run();

    private:

        void update_entities_(float time);
        void update_camera_(float time);

        void init_();

        static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
        static void framebufferResizeCallback(GLFWwindow* window, int width, int height); //TODO now you cant resize window
    };

}//namespace ezg