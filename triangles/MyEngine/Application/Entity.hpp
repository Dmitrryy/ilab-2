/*************************************************************************************************
 *
 *   Entity.hpp
 *
 *   Created by dmitry
 *   24.02.2021
 *
 ***/

#pragma once

#include <Engine/Engine.hpp>

#include <LAL/include/LALmath.h>


#include <path/tinyxml2p.hpp>


namespace ezg
{

    struct Entity
    {
        glm::vec3 m_scale = {1.f, 1.f, 1.f};

        glm::vec3 m_position = {};
        glm::vec3 m_dirTravel = {1.f, 0.f, 0.f};

        glm::vec3 m_dirRotation = {0.f, 0.f, 1.f};
        float m_angle = 0;
        float m_speedRotation = 0;

    public:

        virtual void loadFromXML(tinyxml2::XMLElement* xmlElem) = 0;

        /// moves the object at a distance corresponding to a given time interval
        /// \param dt - the amount of time it takes to move the object
        void update(float dt) noexcept;
    };


    struct TriangleMesh : public Entity, public engine::Mesh
    {
        glm::vec3 m_color = {1.f, 1.f, 1.f};

    public:

        glm::mat4 getModelMatrix() const override;

        glm::vec3 getColor() const override { return m_color; }

        glm::vec3 getPosition() const override { return m_position; }

        /// Carries out the growth of the mesh according to the following algorithm:
        /// A point is selected in the triangle, then it is stretched along the normal
        /// to the triangle. The result is 3 triangles from the original one.
        /// \param triangleId - the number of the triangle in the grid that will be
        /// the base for the growth
        /// \param distance - the distance the new point will be stretched along the normal
        void grow(size_t triangleId, float distance);

        void loadFromXML(tinyxml2::XMLElement* xmlElem) override;
    };


    struct Mirror : public Entity, public engine::Mirror
    {
        glm::vec3 m_color = {0.1f, 0.1f, 0.1f};

    public:

        glm::mat4 getModelMatrix() const override;

        glm::vec3 getColor() const override { return m_color; }

        glm::vec3 getPosition() const override { return m_position; }

        void loadFromXML(tinyxml2::XMLElement* xmlElem) override;
    };


}// namespace ezg