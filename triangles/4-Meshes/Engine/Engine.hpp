/*************************************************************************
 *
 * Driver.h
 *
 * Created by dmitry
 * 22.02.2021
 *
 ***/


//
/// Engine
///======================================================================================
/// A humble engine based on the Vulkan API.
///
/// In this version, the following features:
/// - Drawing a grid of triangles of different sizes.
/// - Maximum objects == 10000 (configurable by default in the header of the current
///   file).
/// - Ability to load new objects after engine initialization.
/// - Rendering only those objects that are passed to the render function.
/// - All objects must be inherited from the Mesh class. In this class, you can override
///   methods that return the model matrix and color.
///======================================================================================
///======================================================================================
//


#pragma once


#include "vkCore.h"
#include "CameraView.h"
#include "DeleteonQueue.hpp"

#include <vk_mem_alloc.h>


#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include <fstream>
#include <functional>
#include <deque>
#include <array>
#include <algorithm>

#include <OtherLibs/timer.h>


const size_t ezg_default_max_objects = 10000;


namespace ezg
{

	struct Vertex 
	{
		glm::vec3 pos;
		glm::vec3 color = { 1.f, 1.f, 1.f };
		glm::vec3 normal;


		static VkVertexInputBindingDescription getBindingDescription()
		{
			VkVertexInputBindingDescription bindingDescription = {};
			bindingDescription.binding = 0;
			bindingDescription.stride = sizeof(Vertex);
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			return bindingDescription;
		}

		static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescription()
		{
			std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

			attributeDescriptions[0].binding = 0;
			attributeDescriptions[0].location = 0;
			attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[0].offset = offsetof(Vertex, pos);

			attributeDescriptions[1].binding = 0;
			attributeDescriptions[1].location = 1;
			attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[1].offset = offsetof(Vertex, color);

			attributeDescriptions[2].binding = 0;
			attributeDescriptions[2].location = 2;
			attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[2].offset = offsetof(Vertex, normal);

			return attributeDescriptions;
		}
	};


	class Engine
	{

	    // this structure is used to compactly store data
	    // defining the same buffer (from Vulkan and vma)
        struct AllocatedBuffer
        {
            VkBuffer _buffer;
            VmaAllocation _allocation;
        };

        // this structure is used to compactly store data
        // defining the same image (from Vulkan and vma)
        struct AllocatedImage
        {
            VkImage _image;
            VmaAllocation _allocation;
        };


    public:

	    /// class Mesh
	    /// class that defines the required fields and methods required for rendering
        class Mesh {
            AllocatedBuffer m_vertexBuffer = {};
            bool            m_isUploaded   = false;

            // so that the engine can access private fields
            // (these fields are not needed by an external user)
            friend Engine;
        public:

            std::vector< Vertex > vertices;

        public:

            virtual ~Mesh() = default;

        public:

            virtual glm::mat4 getModelMatrix() const { return glm::mat4{1.f}; }
            virtual glm::vec3 getColor()       const { return glm::vec3(1.f); }

            bool load_from_obj(const std::string& filename);
        };//class Mesh


	private:

        struct FrameData
        {
            VkSemaphore presentSemaphore = nullptr;
            VkSemaphore renderSemaphore  = nullptr;
            VkFence     renderFence      = nullptr;

            VkCommandPool   commandPool       = nullptr;
            VkCommandBuffer mainCommandBuffer = nullptr;


            AllocatedBuffer cameraBuffer = {};
            VkDescriptorSet globalDescriptor = nullptr;

            AllocatedBuffer objectBuffer = {};
            VkDescriptorSet objectDescriptor = nullptr;
        };//struct FrameData


        struct RenderMaterial
        {
            VkPipeline pipeline;
            VkPipelineLayout pipelineLayout;
        };


        struct GPUCameraData {
            glm::mat4 view;
            glm::mat4 proj;
        };

        struct GPUObjectData {
            alignas(16) glm::mat4 model;
            alignas(16) glm::vec3 color;
        };

	private:

        struct RenderObject
        {
            Mesh *mesh = nullptr;
        };

	private:

		const Window&                  m_rWindow;
		Core                           m_core;

        VmaAllocator                   m_allocator = nullptr;

        std::vector< FrameData >       m_frames;

        std::vector< VkImage >         m_images;
        std::vector< VkImageView >     m_views;
        std::vector< VkFramebuffer >   m_frameBuffers;

        VkSwapchainKHR                 m_swapChainKHR   = nullptr;
		VkQueue                        m_queue          = nullptr;

		VkRenderPass                   m_renderPass     = nullptr;

        RenderMaterial                 m_renderMaterial = {};

        VkImageView                    m_depthImageView = nullptr;
        AllocatedImage                 m_depthImage     = {};


		VkDescriptorPool               m_descriptorPool = nullptr;
        VkDescriptorSetLayout          m_globalSetLayout = nullptr;
        //VkDescriptorSetLayout          m_objectSetLayout = nullptr; //todo

        Timer m_time;

		size_t                         m_currentFrame      = 0;
		size_t                         m_maxFramesInFlight = 2;

		// TODO increase if necessary
		size_t                         m_numObjects        = ezg_default_max_objects;

        ezg::CameraView                m_cameraView;


        DeletionQueue_t                m_deletionQueue;

    public:

	    Engine           (const Engine&) = delete; //not supported
	    Engine& operator=(const Engine&) = delete; //not supported
	    Engine           (Engine&&)      = delete; //not supported
	    Engine& operator=(Engine&&)      = delete; //not supported


		Engine(const Window& window);


		~Engine();


	public:

		void upload_mesh(Mesh& mesh);
		void unload_mesh(Mesh& mesh);

		void render_meshes(const std::vector< Mesh* >& objects);


		void cleanup();


		void setCameraView(const ezg::CameraView& camera) { m_cameraView = camera; }


	private:

        void init_(GLFWwindow* window);

	    FrameData& getCurFrame() { return m_frames[m_currentFrame % m_maxFramesInFlight]; }

        AllocatedBuffer create_buffer_(size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage);

        VkShaderModule createShaderModule_(const std::string &source_);

		void createSwapChain_();

		void initDescriptors();

		void updateUniformBuffer_(uint32_t currentImage_, const std::vector< Mesh* >& meshes);

		void createCommandBuffer_();

		void createFramebuffer_();

		void createDepthResources_();

		void createRenderPass_();

		void createPipeline_();

		void createSyncObjects_();


		bool hasStencilComponent(VkFormat format) {
			return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D16_UNORM_S8_UINT;
		}
	};

}//namespace vks