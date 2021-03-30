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
#include "Entity.hpp"


///======================================================================================
/// This file defines a class that performs a level-3 triangle task:
/// visualizes rotating triangles in three-dimensional space and highlights intersecting
/// triangles.
///======================================================================================
///======================================================================================

namespace ezg
{

    class AppLVL3
    {

        VulkanDriver m_driver;
        GLFWwindow*       m_pWindow = nullptr;

        //TODO go to the pointer on Entity
        std::vector< Triangle > m_entities;

        CameraView m_cameraView;
        float      m_speed = 10.f;

        ezg::Timer     m_time;

    public:

        AppLVL3           (const AppLVL3&) = delete; //not supported
        AppLVL3& operator=(const AppLVL3&) = delete; //not supported
        AppLVL3           (AppLVL3&&)      = delete; //not supported
        AppLVL3& operator=(AppLVL3&&)      = delete; //not supported

        //default destructor
        AppLVL3()
            : m_driver     ("Triangles-3")
            , m_cameraView ({ 2.f, 2.f, 2.f }, { -2.f, -2.f, -2.f})
        {}

        ~AppLVL3() {
            glfwDestroyWindow(m_pWindow);
        }


    public:

        /// Adds a triangle
        /// \param pos_a    - position of the vertex 'A' relative to the coordinate center.
        /// \param pos_b    - position of the vertex 'B' relative to the coordinate center.
        /// \param pos_c    - position of the vertex 'C' relative to the coordinate center.
        /// \param position - offset of the shape center relative to the coordinate center.
        /// \param direction_rotation - angular velocity vector.
        /// \param rotation_speed     - angular velocity (does not have to be the same as the
        ///                             modulus of the angular velocity vector).
        /// \param live_time_sec      - the time that the triangle will make the movement.
        /// \return void
        void addTriangle(glm::vec3 pos_a,
                         glm::vec3 pos_b,
                         glm::vec3 pos_c,
                         glm::vec3 position,
                         glm::vec3 direction_rotation,
                         float rotation_speed,
                         float live_time_sec);

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

        void load_entity_data_in_driver_();


        void init_();


        static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
        static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
    };

}//namespace ezg