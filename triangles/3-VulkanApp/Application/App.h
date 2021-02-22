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

#include "Driver.h"
#include <OtherLibs/timer.h>


///======================================================================================
/// This file defines a class that performs a level-3 triangle task:
/// visualizes rotating triangles in three-dimensional space and highlights intersecting
/// triangles.
///======================================================================================
///======================================================================================

namespace ezg
{

    ///
    ///
    class AppLVL3
    {

        struct Entity
        {
            std::vector< vks::Vertex > m_vertices;

            glm::vec3 m_color         = { 0.f, 240.f, 0.f };

            glm::vec3 m_position;

            glm::vec3 m_dirRotation   = { 0.f, 0.f, 1.f };
            float     m_angle         = 0;
            float     m_speedRotation = 0;

            float     m_liveTimeSec   = 0;


            /// updates the position of the triangle.
            /// \param time - frame time
            void update(float time);

            glm::mat4 getModelMatrix() const;
        };//struct Entity

    private:

        vks::VulkanDriver m_driver;
        GLFWwindow*       m_pWindow = nullptr;


        std::vector< Entity > m_entities;

        CameraView m_cameraView;
        float      m_speed = 10.f;

        ezg::Timer     m_time;

    public:

        AppLVL3()
            : m_driver     ("Triangles-3")
            , m_cameraView ({ 2.f, 2.f, 2.f }, { -2.f, -2.f, -2.f})
        {}

        ~AppLVL3() {
            glfwDestroyWindow(m_pWindow);
        }

    public:

        /// Adds a triangle with the following parameters:
        /// \param pos_a    - position of the vertex 'A' relative to the coordinate center.
        /// \param pos_b    - position of the vertex 'B' relative to the coordinate center.
        /// \param pos_c    - position of the vertex 'C' relative to the coordinate center.
        /// \param position - offset of the shape center relative to the coordinate center.
        /// \param direction_rotation - angular velocity vector.
        /// \param rotation_speed     - angular velocity (does not have to be the same as the
        ///                             modulus of the angular velocity vector).
        /// \param live_time_sec      - the time that the triangle will make the movement.
        void addTriangle(glm::vec3 pos_a,
                         glm::vec3 pos_b,
                         glm::vec3 pos_c,
                         glm::vec3 position,
                         glm::vec3 direction_rotation,
                         float rotation_speed,
                         float live_time_sec);

        void run();

    private:

        void update_entities_(float time);
        void update_camera_(float time);


        void load_entity_data_in_driver_();


        void init_();
        void init_camera_();


        static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
        static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
    };

}//namespace ezg