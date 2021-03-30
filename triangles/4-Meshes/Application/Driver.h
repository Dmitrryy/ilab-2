/*************************************************************************
 *
 * Driver.h
 *
 * Created by dmitry
 * 22.02.2021
 *
 ***/
#pragma once

#include "vkCore.h"
#include "Helpa/helpa.h"
#include "CameraView.h"

#include <vk_mem_alloc.h>

#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <fstream>
#include <functional>
#include <deque>
#include <array>



#define EZG_ENGINE_DEFAULT_MAX_OBJECTS_NUM 10000
#define EZG_ENGINE_DEFAULT_MAX_VERTICES_IN_OBJECT 10000



namespace ezg
{

	struct Vertex 
	{
		glm::vec3 pos;
		glm::vec3 color = { 255.f, 0.f, 0.f };
		glm::vec3 normal;

        //alignas(8) uint64_t entityId;


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


    struct AllocatedBuffer
    {
        VkBuffer _buffer;
        VmaAllocation _allocation;
    };

    struct AllocatedImage
    {
        VkImage _image;
        VmaAllocation _allocation;
    };


    struct DeletionQueue
    {
        std::deque< std::function< void() > > deletors;

        void push_function(std::function< void() > &&function)
        {
            deletors.push_front(function);
        }

        void flush()
        {
            // reverse iterate the deletion queue to execute all the functions
            for (auto&& f : deletors) {
                f(); //call functors
            }

            deletors.clear();
        }
    };


    struct FrameData
    {
        VkSemaphore presentSemaphore = nullptr;
        VkSemaphore renderSemaphore  = nullptr;
        VkFence     renderFence      = nullptr;

        DeletionQueue   frameDeletionQueue;

        VkCommandPool   commandPool       = nullptr;
        VkCommandBuffer mainCommandBuffer = nullptr;

        VkFramebuffer   frameBuffer = nullptr;

        AllocatedBuffer cameraBuffer = {};
        VkDescriptorSet globalDescriptor = nullptr;

        AllocatedBuffer objectBuffer = {};
        VkDescriptorSet objectDescriptor = nullptr;

        AllocatedBuffer storageBufferWorldCoords = {};
    };


	struct ObjectInfo
    {
	    std::vector< Vertex > vertices;
	    glm::mat4 model_matrix;
	    glm::vec3 color;
    };


	struct GPUCameraData {
		glm::mat4 view;
		glm::mat4 proj;
	};

	struct GPUObjectData {
        alignas(16) glm::mat4 model;
        alignas(16) glm::vec3 color;
	};

	struct OutputVertShader {
        alignas(16) glm::vec3 world_coord;
	};


	enum class PipelineType
    {
	    DEFAULT_MESH
	    //todo
    };

	struct RenderMaterial
    {
        VkPipeline pipeline;
        VkPipelineLayout pipelineLayout;
    };


    struct Mesh {
        std::vector<Vertex> _vertices;

        AllocatedBuffer _vertexBuffer;

        bool load_from_obj(const char* filename);
    };


	class VulkanDriver
	{
		std::string                    m_appName;
		GLFWwindow*                    m_pWindow = nullptr;
		bool                           m_framebufferResized = false;

		Core                           m_core;

        DeletionQueue                  m_deletionQueue;

        VmaAllocator                   m_allocator;

        std::vector< FrameData >       m_frames;

        std::vector< VkImage >         m_images;
        std::vector< VkImageView >     m_views;

        VkSwapchainKHR                 m_swapChainKHR   = nullptr;
		VkQueue                        m_queue          = nullptr;

		VkRenderPass                   m_renderPass     = nullptr;

        std::unordered_map< PipelineType, RenderMaterial > m_renderMaterials;
        std::unordered_map< std::string, Mesh >            m_meshes;



        VkImageView                    m_depthImageView;
        AllocatedImage                 m_depthImage;


		VkDescriptorPool               m_descriptorPool = nullptr;
        VkDescriptorSetLayout          m_globalSetLayout = nullptr;
        //VkDescriptorSetLayout          m_objectSetLayout = nullptr; //todo



		size_t                         m_currentFrame      = 0;
		size_t                         m_maxFramesInFlight = 2;
		size_t                         m_curNumFrameInFlight = 0;

		size_t                         m_numObjects          = EZG_ENGINE_DEFAULT_MAX_OBJECTS_NUM;
		size_t                         m_numVerticesInObject = EZG_ENGINE_DEFAULT_MAX_VERTICES_IN_OBJECT;


		std::vector< GPUObjectData >    m_modelData;

        ezg::CameraView     m_cameraView;

        std::vector< std::vector< uint32_t > > m_indices;
        size_t                                 m_numAllIndices = 0;
        std::vector< size_t >                  m_indexBufferOffsets;
        AllocatedBuffer                        m_indexBuffer;

        //todo: different buffer for each object
		std::vector< std::vector < Vertex > >  m_vertices;
        size_t                                 m_numAllVertices = 0;
        std::vector< size_t >                  m_vertexBufferOffsets;
        AllocatedBuffer                        m_vertexBuffer;

        //todo: access to the vertices of a specific element by offset in a shared array
        std::vector< glm::vec3 >               m_worldCoords;

	public:

	    VulkanDriver           (const VulkanDriver&) = delete; //not supported
	    VulkanDriver& operator=(const VulkanDriver&) = delete; //not supported
	    VulkanDriver           (VulkanDriver&&)      = delete; //not supported
	    VulkanDriver& operator=(VulkanDriver&&)      = delete; //not supported


		VulkanDriver(std::string appName_)
			: m_core(appName_)
			, m_appName(std::move(appName_))
			, m_cameraView({ 2.f, 2.f, 2.f }, { -2.f, -2.f, -2.f})
		{ }


		~VulkanDriver()
		{
			cleanup();
		}


		void detectFrameBufferResized() { m_framebufferResized = true; }


		void Init(GLFWwindow* window);


		void addObject(const ObjectInfo& info);
		void setObjectInfo(size_t objectID, const ObjectInfo& info);

		void updateWorldCoordsData(uint32_t currentImage_);
		std::vector< glm::vec3 > getWorldCoords(size_t objectID);


		void render();


		void cleanup();


		void setCameraView(const ezg::CameraView& camera) { m_cameraView = camera; }


	private:

        AllocatedBuffer create_buffer_(size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage);

        VkShaderModule createShaderModule_(const std::vector< char >& source_);

		void createSwapChain_();

		void initDescriptors();


		void createVertexBuffer_();
		void createIndexBuffer_();

		void updateUniformBuffer_(uint32_t currentImage_);

		void createCommandBuffer_();
		void recordCommandBuffers_();

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