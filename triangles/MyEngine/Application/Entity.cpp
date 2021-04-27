/*************************************************************************************************
 *
 *   Entity.cpp
 *
 *   Created by dmitry
 *   24.02.2021
 *
 ***/

#include <algorithm>
#include <OtherLibs/random.h>


#include "Entity.hpp"


namespace ezg
{

    void Entity::update(float dt) noexcept
    {
        m_angle += m_speedRotation * dt;

        m_position += m_dirTravel * dt;
    }





    glm::mat4 TriangleMesh::getModelMatrix() const /*override*/
    {
        glm::mat4 res = glm::translate(glm::mat4(1.f), m_position);
        res = glm::rotate(res, glm::radians(m_angle), m_dirRotation);
        res = glm::scale(res, m_scale);
        return res;
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


    void TriangleMesh::loadFromXML(tinyxml2::XMLElement* xmlElem)
    {
        auto* modelXML = xmlElem->FirstChildElement("model");
        if (modelXML == nullptr || !load_from_obj(modelXML->Attribute("file"))) {
            throw std::runtime_error("cant load model from file: " + std::string(modelXML->Attribute("file")));
        }

        using tinyxml2p::getAttribute;

        auto* prop = xmlElem->FirstChildElement("properties");
        if (prop != nullptr) {
            auto* posXML = prop->FirstChildElement("position");
            if (posXML != nullptr) {
                getAttribute(posXML, "x", m_position.x, "y", m_position.y, "z"
                             , m_position.z);
            }

            auto* colorXML = prop->FirstChildElement("color");
            if (colorXML != nullptr) {
                getAttribute(colorXML, "r", m_color.x, "g", m_color.y, "b", m_color.z);
            }

            auto* scaleXML = prop->FirstChildElement("scale");
            if (scaleXML != nullptr) {
                getAttribute(scaleXML, "x", m_scale.x, "y", m_scale.y, "z", m_scale.z);
            }

            auto* dirTravelXML = prop->FirstChildElement("dirTravel");
            if (dirTravelXML != nullptr) {
                getAttribute(dirTravelXML, "x", m_dirTravel.x, "y", m_dirTravel.y, "z"
                             , m_dirTravel.z);
            }

            auto* dirRotateXML = prop->FirstChildElement("dirRotate");
            if (dirRotateXML != nullptr) {
                getAttribute(dirRotateXML, "x", m_dirRotation.x, "y", m_dirRotation.y, "z"
                             , m_dirRotation.z);
            }

            auto* startAngleRotateXML = prop->FirstChildElement("startAngle");
            if (startAngleRotateXML != nullptr) {
                getAttribute(startAngleRotateXML, "val", m_angle);
            }

            auto* speedRotationXML = prop->FirstChildElement("speedRotation");
            if (speedRotationXML != nullptr) {
                m_speedRotation = speedRotationXML->FloatAttribute("val");
            }
        }

        auto* growXML = xmlElem->FirstChildElement("grow");
        if (growXML != nullptr) {
            size_t stages = growXML->Unsigned64Attribute("stages");
            float max_dist = growXML->FloatAttribute("distance");

            vertices.reserve(vertices.size() + stages * 2);

            // the random number generator can only generate integers.
            // To get a random non-integer number, we multiply the limits
            // by accuracy and, during the generation itself, divide by accuracy
            const size_t accuracy = 100000;
            const uint max = static_cast<uint>(max_dist * accuracy);
            ezg::Random randDist(max / 6, max);

            for (size_t i = 0; i < stages; ++i) {
                ezg::Random randTriangle(0, vertices.size() / 3 - 1);

                grow(randTriangle(), static_cast<float>(randDist()) / accuracy);
            }
        }
    }








    void Mirror::loadFromXML(tinyxml2::XMLElement* xmlElem)
    {
        auto* modelXML = xmlElem->FirstChildElement("model");
        if (modelXML == nullptr || !load_from_obj(modelXML->Attribute("file"))) {
            throw std::runtime_error("cant load model from file: " + std::string(modelXML->Attribute("file")));
        }

        using tinyxml2p::getAttribute;

        auto* prop = xmlElem->FirstChildElement("properties");
        if (prop != nullptr) {
            auto* posXML = prop->FirstChildElement("position");
            if (posXML != nullptr) {
                getAttribute(posXML, "x", m_position.x, "y", m_position.y, "z"
                             , m_position.z);
            }

            auto* colorXML = prop->FirstChildElement("color");
            if (colorXML != nullptr) {
                getAttribute(colorXML, "r", m_color.x, "g", m_color.y, "b", m_color.z);
            }

            auto* scaleXML = prop->FirstChildElement("scale");
            if (scaleXML != nullptr) {
                getAttribute(scaleXML, "x", m_scale.x, "y", m_scale.y, "z", m_scale.z);
            }

            auto* dirTravelXML = prop->FirstChildElement("dirTravel");
            if (dirTravelXML != nullptr) {
                getAttribute(dirTravelXML, "x", m_dirTravel.x, "y", m_dirTravel.y, "z"
                             , m_dirTravel.z);
            }

            auto* dirRotateXML = prop->FirstChildElement("dirRotate");
            if (dirRotateXML != nullptr) {
                getAttribute(dirRotateXML, "x", m_dirRotation.x, "y", m_dirRotation.y, "z"
                             , m_dirRotation.z);
            }

            auto* speedRotationXML = prop->FirstChildElement("speedRotation");
            if (speedRotationXML != nullptr) {
                m_speedRotation = speedRotationXML->FloatAttribute("val");
            }
        }

    }





    glm::mat4 Mirror::getModelMatrix() const /*override*/
    {
        glm::mat4 res = glm::translate(glm::mat4(1.f), m_position);
        res = glm::rotate(res, glm::radians(m_angle), m_dirRotation);
        res = glm::scale(res, m_scale);
        return res;
    }


}// namespace ezg