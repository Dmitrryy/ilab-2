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

            glm::vec3 m_color         = { 0.f, 256.f, 0.f };

            glm::vec3 m_position;

            glm::vec3 m_dirRotation;
            float     m_angle         = 0;
            float     m_speedRotation = 0;

            float     m_liveTimeSec   = 0;


            /// updates the position of the triangle.
            /// \param time - frame time
            void update(float time);

            glm::mat4 getModelMatrix() const;
        };//struct Entity

    private:

        std::vector< Entity > m_entities;

        float m_time = 0;

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

        void update_entity_(float time);
        void checkEvents_();
    };

}//namespace ezg