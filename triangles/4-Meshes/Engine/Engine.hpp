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
#include "CameraView.h"

#include <vk_mem_alloc.h>

#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include <fstream>
#include <functional>
#include <deque>
#include <array>

#include <OtherLibs/timer.h>


#define EZG_ENGINE_DEFAULT_MAX_OBJECTS_NUM 10000


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


/*    struct DeletionQueue
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
    };*/


    struct FrameData
    {
        VkSemaphore presentSemaphore = nullptr;
        VkSemaphore renderSemaphore  = nullptr;
        VkFence     renderFence      = nullptr;

        std::deque< std::function< void() > >   frameDeletionQueue;

        VkCommandPool   commandPool       = nullptr;
        VkCommandBuffer mainCommandBuffer = nullptr;


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



	class Engine
	{
	public:

        class Mesh {
            AllocatedBuffer m_vertexBuffer = {};
            bool            m_isUploaded   = false;

            PipelineType    m_pipelineType = PipelineType::DEFAULT_MESH;

            friend Engine;
        public:

            std::vector<Vertex> vertices;

        public:

            ~Mesh() = default;

        public:

            virtual glm::mat4 getModelMatrix() const noexcept { return glm::mat4{1.f}; }
            virtual glm::vec3 getColor()       const noexcept { return glm::vec3(1.f); }

            bool load_from_obj(const char* filename);
        };

	private:

        struct RenderObject
        {
            Mesh *mesh = nullptr;

            RenderMaterial *material = nullptr;

            glm::mat4 transformMatrix = {};
        };

	private:

	    using FunctionQueue = std::deque< std::function< void() > >;

		std::string                    m_appName;
		GLFWwindow*                    m_pWindow = nullptr;
		bool                           m_framebufferResized = false;

		Core                           m_core;

        FunctionQueue                  m_deletionQueue;

        VmaAllocator                   m_allocator;

        std::vector< FrameData >       m_frames;

        std::vector< VkImage >         m_images;
        std::vector< VkImageView >     m_views;
        std::vector< VkFramebuffer >   m_frameBuffers;

        VkSwapchainKHR                 m_swapChainKHR   = nullptr;
		VkQueue                        m_queue          = nullptr;

		VkRenderPass                   m_renderPass     = nullptr;

        std::unordered_map< PipelineType, RenderMaterial > m_renderMaterials;

        VkImageView                    m_depthImageView;
        AllocatedImage                 m_depthImage;


		VkDescriptorPool               m_descriptorPool = nullptr;
        VkDescriptorSetLayout          m_globalSetLayout = nullptr;
        //VkDescriptorSetLayout          m_objectSetLayout = nullptr; //todo

        Timer m_time;

		size_t                         m_currentFrame      = 0;
		size_t                         m_maxFramesInFlight = 2;
		size_t                         m_curNumFrameInFlight = 0;

		size_t                         m_numObjects          = EZG_ENGINE_DEFAULT_MAX_OBJECTS_NUM;

        ezg::CameraView     m_cameraView;

        //todo: access to the vertices of a specific element by offset in a shared array
        std::vector< glm::vec3 >               m_worldCoords;

	public:

	    Engine           (const Engine&) = delete; //not supported
	    Engine& operator=(const Engine&) = delete; //not supported
	    Engine           (Engine&&)      = delete; //not supported
	    Engine& operator=(Engine&&)      = delete; //not supported


		Engine(std::string appName_)
			: m_core(appName_)
			, m_appName(std::move(appName_))
			, m_cameraView({ 2.f, 2.f, 2.f }, { -2.f, -2.f, -2.f})
		{ }


		~Engine()
		{
			cleanup();
		}


		void detectFrameBufferResized() { m_framebufferResized = true; }


		void Init(GLFWwindow* window);


		void upload_mesh(Mesh& mesh);
		void render_meshes(const std::vector< Mesh* >& objects);


		void cleanup();


		void setCameraView(const ezg::CameraView& camera) { m_cameraView = camera; }


	private:

        RenderMaterial *get_material(PipelineType type);

        Mesh *get_mesh(const std::string &name);

	    FrameData& getCurFrame() { return m_frames[m_currentFrame % m_maxFramesInFlight]; }

        AllocatedBuffer create_buffer_(size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage);

        VkShaderModule createShaderModule_(const std::string &source_);

		void createSwapChain_();

		void initDescriptors();


/*		void createVertexBuffer_();
		void createIndexBuffer_();*/

		void updateUniformBuffer_(uint32_t currentImage_, const std::vector< Mesh* >& meshes);

		void createCommandBuffer_();
/*		void recordCommandBuffers_();*/

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