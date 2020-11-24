#pragma once

#include "vkCore.h"
#include "../helpa.h"
#include "CameraView.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <fstream>
#include <array>


namespace vks
{

	struct Vertex {
		glm::vec3 pos;
		glm::vec3 color;


		static VkVertexInputBindingDescription getBindingDescription()
		{
			VkVertexInputBindingDescription bindingDescription = {};
			bindingDescription.binding = 0;
			bindingDescription.stride = sizeof(Vertex);
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			return bindingDescription;
		}

		static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescription()
		{
			std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

			attributeDescriptions[0].binding = 0;
			attributeDescriptions[0].location = 0;
			attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[0].offset = offsetof(Vertex, pos);

			attributeDescriptions[1].binding = 0;
			attributeDescriptions[1].location = 1;
			attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[1].offset = offsetof(Vertex, color);

			return attributeDescriptions;
		}
	};

	struct UniformBufferObject {
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 proj;
	};

	class VulkanApp
	{
		std::string                    m_appName;
		WindowControl*                 m_pWindowControl = nullptr;

		Core                           m_core;
		std::vector< VkImage >         m_images;
		VkSwapchainKHR                 m_swapChainKHR   = nullptr;
		VkQueue                        m_queue          = nullptr;
		std::vector< VkCommandBuffer > m_cmdBufs;
		VkCommandPool                  m_cmdBufPool     = nullptr;

		std::vector< VkImageView >     m_views;
		VkRenderPass                   m_renderPass     = nullptr;
		std::vector< VkFramebuffer >   m_fbs;
		VkPipeline                     m_pipeline       = nullptr;

		VkImage                        m_depthImage = nullptr;
		VkDeviceMemory                 m_depthImageMemory = nullptr;
		VkImageView                    m_depthImageView = nullptr;

		VkDescriptorSetLayout          m_descriptorSetLayout = nullptr;
		VkDescriptorPool               m_descriptorPool = nullptr;
		std::vector< VkDescriptorSet > m_descriptorSets;
		VkPipelineLayout               m_pipelineLayout = nullptr;

		std::vector< VkSemaphore >     m_imageAvailableSem;
		std::vector< VkSemaphore >     m_renderFinishedSem;
		std::vector< VkFence >         m_inFlightFences;
		std::vector< VkFence >         m_imagesInFlight;
		size_t                         m_currentFrame   = 0;

		size_t                         m_maxFramesInFlight = 2;

		VkBuffer                       m_vertexBuffer = {};
		VkDeviceMemory                 m_vertexBufferMemory = {};

		std::vector< VkBuffer >       m_uniformBuffers;
		std::vector< VkDeviceMemory > m_uniformBuffersMemory;

		CameraView                     m_cameraView;
		float                          m_speed = 1.f;

		std::vector<Vertex> vertices = {
			{{-0.5f, 0.f, -0.5f}, {1.0f, 0.0f, 0.0f}},
			{{-0.5f, 0.0f, 0.5f}, {0.0f, 1.0f, 0.0f}},
			{{0.5f, 0.f, 0.5f},  {0.0f, 0.0f, 1.0f}},
			{{-0.5f, 0.f, -0.5f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, 0.0f, -0.5f}, {0.0f, 1.0f, 0.0f}},
			{{0.5f, 0.f, 0.5f},  {0.0f, 0.0f, 1.0f}},

			{{-0.5f, 0.f, -0.5f}, {1.0f, 0.0f, 0.0f}},
			{{-0.5f, 0.0f, 0.5f}, {0.0f, 1.0f, 0.0f}},
			{{0.f, 1.f, 0.f}, {1.0f, 0.0f, 0.0f}},

			{{-0.5f, 0.0f, 0.5f}, {0.0f, 1.0f, 0.0f}},
			{{0.5f, 0.f, 0.5f},  {0.0f, 0.0f, 1.0f}},
			{{0.f, 1.f, 0.f}, {1.0f, 0.0f, 0.0f}},

			{{-0.5f, 0.f, -0.5f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, 0.0f, -0.5f}, {0.0f, 1.0f, 0.0f}},
			{{0.f, 1.f, 0.f}, {1.0f, 0.0f, 0.0f}},

			{{0.5f, 0.0f, -0.5f}, {0.0f, 1.0f, 0.0f}},
			{{0.5f, 0.f, 0.5f},  {0.0f, 0.0f, 1.0f}},
			{{0.f, 1.f, 0.f}, {1.0f, 0.0f, 0.0f}},
		};

	public:

		VulkanApp(std::string appName_)
			: m_core(appName_)
			, m_appName(std::move(appName_))
			, m_cameraView({ 2.f, 2.f, 2.f }, { -2.f, -2.f, -2.f})
		{ }

		~VulkanApp()
		{
			cleanup();
			delete m_pWindowControl;
		}

		void Init();

		void setVertexBuffer(const std::vector< Vertex >& new_data) { vertices = new_data;}
		void setCameraView(const CameraView& camera) { m_cameraView = camera; }
		void setCameraSpeed(float speed) { m_speed = speed; }

		void Run();

	private:

		void updateCamera(float time);


		void createSwapChain_();
		void recreateSwapChain_();

		void cleanupSwapChain();
		void cleanup();


		void createVertexBuffer_();

		void createUniformBuffers_();
		void updateUniformBuffer_(uint32_t currentImage_);

		void copyBuffer_(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
		VkCommandBuffer beginSingleTimeCommands();
		void endSingleTimeCommands(VkCommandBuffer commandBuffer);

		void createCommandBuffer_();
		void recordCommandBuffers_();

		void renderScene_();

		void createFramebuffer_();

		void createDepthResources_();

		void createRenderPass_();

		void createDescriptorSetLayput_();
		void createDescriptorPool_();
		void createDescriptorSets_();

		void createPipeline_();

		void createSyncObjects_();

		VkShaderModule createShaderModule_(const std::vector< char >& source_);

		bool hasStencilComponent(VkFormat format) {
			return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D32_SFLOAT_S8_UINT;
		}
	};

}//namespace vks