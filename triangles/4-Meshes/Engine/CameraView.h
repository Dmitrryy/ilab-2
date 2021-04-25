/*************************************************************************
 *
 * CameraView.h
 *
 * Created by dmitry
 * 22.02.2021
 *
 ***/

#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


namespace ezg
{

	struct CameraView
	{
		glm::vec3 m_position   = { 2.f, 2.f, 2.f };
		glm::vec3 m_direction  = { -1.f, -1.f, -1.f };

		glm::vec3 m_topDirection = glm::vec3(0.f, 0.f, 1.f);
		float m_viewingAngle = glm::radians(70.f);
		float m_aspect = 1.f;
		float m_near = 0.1f;
		float m_far = 1000.f;

		float m_max_z_direction_component = 0.998f;

	public:

		CameraView(const glm::vec3& pos = {}, const glm::vec3& direct = {})
			: m_position(pos)
			, m_direction(glm::normalize(direct))
		{		}

		/////////////////////////get////////////////////////////
		glm::vec3 getPosition    () const { return m_position; }
		glm::vec3 getDirection   () const { return m_direction; }
		glm::vec3 getTopDirection() const { return m_topDirection; }
		float getViewingAngle    () const { return m_viewingAngle; }
		float getAspect          () const { return m_aspect; }
		float getMaxZComponentDirection() const { return m_max_z_direction_component; }
		//
		glm::mat4 getViewMatrix() const { return glm::lookAt(m_position, m_position + m_direction, m_topDirection); }
		glm::mat4 getProjectionMatrix() const 
		{ 
			auto res = glm::perspective(m_viewingAngle, m_aspect, m_near, m_far);
			res[1][1] *= -1;
			return res;
		}
		////////////////////////////////////////////////////////


		/////////////////////////set////////////////////////////
		void setPosition     (const glm::vec3& pos) { m_position = pos; }
		void setDirection    (const glm::vec3& dir) { m_direction = glm::normalize(dir); }
		void setTopDirection (const glm::vec3& topDir) { m_topDirection = glm::normalize(topDir); }
		void setViewingAngle (float angle) { m_viewingAngle = angle; }
		void setAspect       (float aspect) { m_aspect = aspect; }
		void setMaxZcomponentDirection(float max_z) { if (std::abs(max_z) <= 1.f) { m_max_z_direction_component = max_z; } }
		////////////////////////////////////////////////////////

		void turnInHorizontalPlane(float angle)
		{
			m_direction = glm::rotate(glm::mat4(1.f), angle, m_topDirection) * glm::vec4(m_direction, 0.f);
		}
		void turnInVerticalPlane(float angle)
		{
		    m_direction = glm::normalize(m_direction);
			if ((angle > 0 && m_direction.z >= m_max_z_direction_component) ||
                    (angle < 0 && m_direction.z <= -m_max_z_direction_component))
			{
				return;
			}
			auto axis = glm::cross(m_direction, m_topDirection);
			m_direction = glm::rotate(glm::mat4(1.f), angle, axis) * glm::vec4(m_direction, 0.f);
		}

		void moveAlongDirection(float distance)
		{
            m_direction = glm::normalize(m_direction);
            m_position += m_direction * distance;
		}
		void movePerpendicularDirection(float distance_horizontal)
		{
            m_direction = glm::normalize(m_direction);
            auto axis = glm::cross(m_direction, m_topDirection);
			m_position += axis * distance_horizontal;
		}
		void moveVertical(float distance)
		{
			m_position += m_topDirection * distance;
		}
	};

}//namespace vks