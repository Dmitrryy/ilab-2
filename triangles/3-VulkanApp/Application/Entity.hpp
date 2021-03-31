/*************************************************************************************************
 *
 *   Entity.hpp
 *
 *   Created by dmitry
 *   24.02.2021
 *
 ***/

#pragma once

#include "Engine.h"

#include "../../LAL/include/LALmath.h"

namespace ezg
{

    struct Entity
    {
        enum class Type{
            Triangle
        };


        virtual ~Entity() = default;


        std::vector< vks::Vertex > m_vertices;
        std::vector< glm::vec3 >   m_coordsInWorld;
        la::Rectangle3             m_area;

        glm::vec3 m_color         = { 0.f, 1.f, 0.f };

        glm::vec3 m_position;

        glm::vec3 m_dirRotation   = { 0.f, 0.f, 1.f };
        float     m_angle         = 0;
        float     m_speedRotation = 0;

        float     m_liveTimeSec   = 0;


        /// updates the position of the triangle.
        /// \param time - frame time
        void update(float time);

        glm::mat4 getModelMatrix() const;


        void updateArea()
        {
            if (m_coordsInWorld.empty()) { return ; }
            la::Vector3f a = { m_coordsInWorld[0].x, m_coordsInWorld[0].y, m_coordsInWorld[0].z };
            la::Vector3f b = a;

            for (const auto& vec : m_coordsInWorld) {
                a.x = std::min(static_cast< float >(a.x), vec.x);
                a.y = std::min(static_cast< float >(a.y), vec.y);
                a.z = std::min(static_cast< float >(a.z), vec.z);

                b.x = std::max(static_cast< float >(b.x), vec.x);
                b.y = std::max(static_cast< float >(b.y), vec.y);
                b.z = std::max(static_cast< float >(b.z), vec.z);
            }

            m_area = { a, b };
        }


        la::Rectangle3 getArea() const noexcept { return m_area; }


        virtual bool intersection(Entity& that) = 0;
        virtual Type type() const noexcept = 0;
    };//struct Entity


    struct Triangle : public Entity
    {
        la::Triangle m_laTriangle;

        Triangle( )
        {
            m_vertices.resize(3);
            m_coordsInWorld.resize(3);
        }

        bool intersection(Entity& that) override
        {
            bool res = false;
            if (la::intersec(m_area, that.m_area))
            {
                // TODO: in case added other Entity
                auto& TriangleThat = static_cast< Triangle& >(that);

                res = la::intersec(m_laTriangle, TriangleThat.m_laTriangle);
            }

            if (res) {
                m_color = that.m_color = { 1.f, 0.f, 0.f};
            }

            return res;
        }

        void updateLaTriangle() {
            m_laTriangle = {
                    {m_coordsInWorld[0].x, m_coordsInWorld[0].y, m_coordsInWorld[0].z},
                    {m_coordsInWorld[1].x, m_coordsInWorld[1].y, m_coordsInWorld[1].z},
                    {m_coordsInWorld[2].x, m_coordsInWorld[2].y, m_coordsInWorld[2].z},
            };
        }

        Type type() const noexcept override { return Type::Triangle; }
    };//struct Triangle

}// namespace ezg