#pragma once

#include "VulkanDriver.h"

namespace vks
{
	class Entity
	{
		//?public:
		glm::vec3 m_pos   = {};
		glm::vec3 m_color = {0.f, 0.5f, 0.f};
		glm::vec3 m_normal;

		//todo optional
		glm::vec3 m_movDirection = {1.f, 0.f, 0.f};
		float     m_movSpeed     = 0.f;
		//

		///////////////////transformation///////////////////
		glm::vec3 m_scale         = { 1.f, 1.f, 1.f };
		//
		glm::vec3 m_rotationAxis  = { 0.f, 0.f, 1.f };
		float     m_rotationSpeed = 0.f;
		float     m_curAngle      = 0.f;
		////////////////////////////////////////////////////

		std::vector<Vertex> m_vertices;

		std::vector<Vertex> m_verticesWithTransform;
		bool m_VWT_isActual = false;

	public:

		void updata(float time)
		{
			m_VWT_isActual = false;

			m_curAngle += m_rotationSpeed * time;
			m_pos += m_movDirection * m_movSpeed;
		}

		glm::mat4 getTranslationMatrix() const { return glm::translate(glm::mat4(1.f), m_pos); }
		glm::mat4 getScaleMatrix()       const { return glm::scale(glm::mat4(1.f), m_scale); }
		glm::mat4 getRotationMatrix()    const { return glm::rotate(glm::mat4(1.f), m_curAngle, m_rotationAxis); }
	};


	class App
	{
		VulkanDriver m_vulkanDriver;

		std::vector< Entity > m_entities;

		float m_time = 0.f;
	};

}//namespace vks