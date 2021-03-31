/*************************************************************************************************
 *
 *   Entity.hpp
 *
 *   Created by dmitry
 *   24.02.2021
 *
 ***/

#pragma once

#include "triangles/4-Meshes/Engine/Engine.hpp"

#include "../../LAL/include/LALmath.h"

namespace ezg
{

    struct TriangleMesh : public Engine::Mesh
    {
        glm::vec3 m_color         = { 1.f, 1.f, 1.f };

        glm::vec3 m_scale         = { 1.f, 1.f, 1.f };

        glm::vec3 m_position      = {};
        glm::vec3 m_dirTravel     = { 1.f, 0.f, 0.f };

        glm::vec3 m_dirRotation   = { 0.f, 0.f, 1.f };
        float     m_angle         = 0;
        float     m_speedRotation = 0;

        glm::mat4 getModelMatrix() const noexcept override;
        glm::vec3 getColor() const noexcept override { return m_color; }

        void grow(size_t triangleId, float distance);

        void update(float dt) noexcept;
    };

}// namespace ezg