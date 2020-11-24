#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


namespace vks
{

	class CameraView
	{
		glm::vec3 m_position;
		glm::vec3 m_direction;

		glm::vec3 m_topDirection = glm::vec3(0.f, 0.f, 1.f);
		float m_viewingAngle = 45.f;
		float m_aspect = 0.f;
		float m_near = 0.1f;
		float m_far = 1000.f;

	public:

		CameraView(const glm::vec3& pos, const glm::vec3& direct)
			: m_position(pos)
			, m_direction(glm::normalize(direct))
		{		}

		/////////////////////////get////////////////////////////
		glm::vec3 getPosition    () const { return m_position; }
		glm::vec3 getDirection   () const { return m_direction; }
		glm::vec3 getTopDirection() const { return m_topDirection; }
		float getViewingAngle    () const { return m_viewingAngle; }
		float getAspect          () const { return m_aspect; }
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
		////////////////////////////////////////////////////////

		void turnInHorizontalPlane(float angle)
		{
			m_direction = glm::rotate(glm::mat4(1.f), angle, m_topDirection) * glm::vec4(m_direction, 0.f);
		}
		void turnInVerticalPlane(float angle)
		{
			auto axis = glm::cross(m_direction, m_topDirection);
			m_direction = glm::rotate(glm::mat4(1.f), angle, axis) * glm::vec4(m_direction, 0.f);
		}

		void moveAlongDirection(float distance)
		{
			m_position += m_direction * distance;
		}
		void movePerpendicularDirection(float distance_horizontal)
		{
			auto axis = glm::cross(m_direction, m_topDirection);
			m_position += axis * distance_horizontal;
		}
		void moveVertical(float distance)
		{
			m_position += m_topDirection * distance;
		}
	};

}//namespace vks