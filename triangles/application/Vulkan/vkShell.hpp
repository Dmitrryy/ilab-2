#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vulkan/vulkan.h>


namespace vks
{

	class Sight_t
	{
	public:

		glm::vec3 m_position;
		glm::vec3 m_direction;
		
		glm::vec3 m_topDirection;
		float m_fovy;
		float m_aspect;
		float m_near;
		float m_far;
	};

	struct InstanceCreateInfo
	{

	};

	class Vulkan_t
	{
		VkInstance m_instance;

		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
		VkDevice device;
	};

	class Device_t
	{

	};

}//namespace vks