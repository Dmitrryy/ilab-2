/*************************************************************************************************
 *
 *   Entity.cpp
 *
 *   Created by dmitry
 *   24.02.2021
 *
 ***/

#include "algorithm"

#include "Entity.hpp"


namespace ezg
{

    void Entity::update(float time)
    {
        m_liveTimeSec -= time;

        if (m_liveTimeSec > 0.f) {
            m_angle += m_speedRotation * std::min(time, m_liveTimeSec);
        }
    }

    glm::mat4 Entity::getModelMatrix() const
    {
        //todo scale
        glm::mat4 translationMatrix = glm::translate(glm::mat4(1.f), m_position);
        glm::mat4 res = glm::rotate(translationMatrix, glm::radians(m_angle), m_dirRotation);
        return res;
    }

}// namespace ezg