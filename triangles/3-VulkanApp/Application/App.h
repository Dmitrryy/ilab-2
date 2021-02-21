/*************************************************************************
 *
 * App.h
 *
 * Created by dmitry
 * 22.02.2021
 *
 ***/

#pragma once

#include <vector>

#include "Driver.h"


namespace ezg
{

    class AppLVL3
    {

        struct Entity
        {
            std::vector< vks::Vertex > m_vertices;

            glm::vec3 m_color         = { 0.f, 256.f, 0.f };
            glm::vec3 m_fixedPoint;
            glm::vec3 m_dirRotation;
            float     m_speedRotation = 0;
            float     m_liveTimeSec   = 0;

        };//struct Entity

    private:

        std::vector< Entity > m_entities;

        float m_time = 0;

    public:

        void addTriangle(glm::vec3 pos_a,
                         glm::vec3 pos_b,
                         glm::vec3 pos_c,
                         glm::vec3 fixed_point,
                         glm::vec3 direction_rotation,
                         float rotation_speed,
                         float live_time_sec);

        void run();

    private:

        void update_entity_(float time);
        void checkEvents_();
    };

}//namespace ezg