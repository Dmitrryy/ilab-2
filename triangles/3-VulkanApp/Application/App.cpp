/****************************************************************************************
 *
 *   App.cpp
 *
 *   Created by dmitry
 *   22.02.2021
 *
 ***/

#include "App.h"
#include "../../Octree/Octree.h"


namespace ezg
{


    void AppLVL3::Entity::update(float time)
    {
        m_liveTimeSec -= time;

        if (m_liveTimeSec > 0.f) {
            m_angle += m_speedRotation * std::min(time, m_liveTimeSec);
        }
    }

    glm::mat4 AppLVL3::Entity::getModelMatrix() const
    {
        glm::mat4 rotateMatrix = glm::rotate(glm::mat4(1.f), glm::radians(m_angle), m_dirRotation);
        glm::mat4 res = glm::translate(rotateMatrix, m_position);
        return res;
    }


    void AppLVL3::addTriangle(glm::vec3 pos_a, glm::vec3 pos_b, glm::vec3 pos_c, glm::vec3 position
                              , glm::vec3 direction_rotation, float rotation_speed, float live_time_sec)
    {
        Entity entity;
        vks::Vertex vertex[3];

        glm::vec3 normal = glm::normalize(glm::cross(pos_b - pos_a, pos_c - pos_a));

        vertex[0].pos = pos_a;
        vertex[0].normal = normal;
        vertex[1].pos = pos_b;
        vertex[1].normal = normal;
        vertex[2].pos = pos_c;
        vertex[2].normal = normal;

        entity.m_vertices.push_back(vertex[0]);
        entity.m_vertices.push_back(vertex[1]);
        entity.m_vertices.push_back(vertex[2]);

        entity.m_position = position;
        entity.m_dirRotation = direction_rotation;
        entity.m_speedRotation = rotation_speed;
        entity.m_liveTimeSec = live_time_sec;

        m_entities.push_back(entity);
    }

    void AppLVL3::run()
    {

    }
}//namespace ezg