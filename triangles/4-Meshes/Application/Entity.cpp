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

    glm::mat4 TriangleMesh::getModelMatrix() const noexcept /*override*/
    {
        glm::mat4 res = glm::translate(glm::mat4(1.f), m_position);
        res = glm::rotate(res, glm::radians(m_angle), m_dirRotation);
        res = glm::scale(res, m_scale);
        return res;
    }

    void TriangleMesh::update(float dt) noexcept
    {
        m_angle += m_speedRotation * dt;

        m_position += m_dirTravel * dt;
    }


    void TriangleMesh::grow(size_t triangleId, float distance)
    {
        const size_t id0 = triangleId * 3;
        const size_t id1 = triangleId * 3 + 1;
        const size_t id2 = triangleId * 3 + 2;
        Vertex v0 = vertices.at(id0);
        Vertex v1 = vertices.at(id1);
        Vertex v2 = vertices.at(id2);

        Vertex v3 = v0;
        v3.pos = v0.pos + ((v1.pos - v0.pos) + (v2.pos - v0.pos)) / 5.f + v0.normal * distance;

        {
            v0.normal = v1.normal = v3.normal = glm::normalize(glm::cross(v3.pos - v0.pos, v3.pos - v1.pos));
            vertices.push_back(v0);
            vertices.push_back(v1);
            vertices.push_back(v3);
        }
        {
            v1.normal = v2.normal = v3.normal = glm::normalize(glm::cross(v3.pos - v1.pos, v3.pos - v2.pos));
            vertices.push_back(v1);
            vertices.push_back(v2);
            vertices.push_back(v3);
        }
        {
            v2.normal = v0.normal = v3.normal = glm::normalize(glm::cross(v3.pos - v2.pos, v3.pos - v0.pos));
            vertices.at(id0) = v2;
            vertices.at(id1) = v0;
            vertices.at(id2) = v3;
        }

    }

}// namespace ezg