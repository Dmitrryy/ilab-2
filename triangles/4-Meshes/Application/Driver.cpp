/*************************************************************************
 *
 * Driver.cpp
 *
 * Created by dmitry
 * 22.02.2021
 *
 ***/

#include "Driver.h"
#include "PipelineBuilder.hpp"


#include <cassert>
#include <cstring>
#include <chrono>


#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"


const std::string vert_shader_fname = "resource/shaders/vert.spv";
const std::string frag_shader_fname = "resource/shaders/frag.spv";

namespace ezg
{

	void VulkanDriver::Init(GLFWwindow* window)
	{
		try
		{
            m_pWindow = window;

			m_core.Init(m_pWindow);

			VmaAllocatorCreateInfo allocatorInfo = {};
	        allocatorInfo.physicalDevice = m_core.getPhysDevice();
	        allocatorInfo.device = m_core.getDevice();
	        allocatorInfo.instance = m_core.getInstance();
	        vmaCreateAllocator(&allocatorInfo, &m_allocator);

	        m_deletionQueue.push_function([&]() {
		        vmaDestroyAllocator(m_allocator);
		    });

			vkGetDeviceQueue(m_core.getDevice(), m_core.getQueueFamily(), 0, &m_queue);

			createSwapChain_();

			createRenderPass_();

			createCommandBuffer_();
			createDepthResources_();
			createFramebuffer_();
/*			createVertexBuffer_();
            createIndexBuffer_();*/

            initDescriptors();

			createPipeline_();
/*
			recordCommandBuffers_();*/

			createSyncObjects_();


			//TODO

			Mesh triMesh{};
            //make the array 3 vertices long
            triMesh._vertices.resize(3);

            //vertex positions
            triMesh._vertices[0].pos = { 1.f,1.f, 0.0f };
            triMesh._vertices[1].pos = { -1.f,1.f, 0.0f };
            triMesh._vertices[2].pos = { 0.f,-1.f, 0.0f };

            //vertex colors, all green
            triMesh._vertices[0].color = { 0.f,1.f, 0.0f }; //pure green
            triMesh._vertices[1].color = { 0.f,1.f, 0.0f }; //pure green
            triMesh._vertices[2].color = { 0.f,1.f, 0.0f }; //pure green
            //we dont care about the vertex normals

            //load the monkey
            Mesh monkeyMesh{};
            if(!monkeyMesh.load_from_obj("resource/meshes/monkey_smooth.obj")){
                throw std::runtime_error("abort");
            }

            upload_mesh(triMesh);
            upload_mesh(monkeyMesh);

            m_meshes["monkey"] = monkeyMesh;
            m_meshes["triangle"] = triMesh;

            //TODO

            	RenderObject monkey;
	monkey.mesh = get_mesh("monkey");
	monkey.material = get_material(PipelineType::DEFAULT_MESH);
	monkey.transformMatrix = glm::mat4{ 1.0f };

	m_renderables.push_back(monkey);

	for (int x = -20; x <= 20; x++) {
		for (int y = -20; y <= 20; y++) {

			RenderObject tri;
			tri.mesh = get_mesh("triangle");
			tri.material = get_material(PipelineType::DEFAULT_MESH);
			glm::mat4 translation = glm::translate(glm::mat4{ 1.0 }, glm::vec3(x, 0, y));
			glm::mat4 scale = glm::scale(glm::mat4{ 1.0 }, glm::vec3(0.2, 0.2, 0.2));
			tri.transformMatrix = translation * scale;

			m_renderables.push_back(tri);
		}
	}
        }
		catch (std::exception& exc_)
		{
			std::cerr << "Fatal error in VulkanDriver::Init():\n";
			std::cerr << "What(): " << exc_.what() << std::endl;
			std::cerr << "Called VulkanDriver::cleanup()" << std::endl;
			cleanup();
		}
	}


	RenderMaterial* VulkanDriver::get_material(PipelineType type)
{
	//search for the object, and return nullpointer if not found
	auto it = m_renderMaterials.find(type);
	if (it == m_renderMaterials.end()) {
		return nullptr;
	}
	else {
		return &(*it).second;
	}
}


Mesh* VulkanDriver::get_mesh(const std::string& name)
{
	auto it = m_meshes.find(name);
	if (it == m_meshes.end()) {
		return nullptr;
	}
	else {
		return &(*it).second;
	}
}


  /*  void VulkanDriver::addObject(const ObjectInfo& info)
    {
	    GPUObjectData cur {};
	    cur.model = info.model_matrix;
	    cur.color = info.color;
        const size_t id = m_vertices.size();

        if (m_vertexBufferOffsets.empty()) {
            m_vertexBufferOffsets.push_back(0);
        }
        else {
            m_vertexBufferOffsets.push_back(m_vertexBufferOffsets.at(id - 1) + m_vertices.at(id - 1).size());
        }

	    m_modelData.push_back(cur);
	    m_vertices.push_back(info.vertices);
	    m_worldCoords.resize(m_worldCoords.size() + info.vertices.size());

	    //todo
	    m_numAllVertices += info.vertices.size();

	    //TODO
	    m_indices.push_back({0, 1, 2});
	    m_numAllIndices += 3;
	    if (m_indexBufferOffsets.empty()) {
	        m_indexBufferOffsets.push_back(0);
	    }
	    else {
            m_indexBufferOffsets.push_back(m_indexBufferOffsets.at(id - 1) + 3);//TODO 3
	    }
    }*/
  /*  void VulkanDriver::setObjectInfo(size_t objectID, const ObjectInfo& info)
    {
        auto& md = m_modelData.at(objectID);

        md.model = info.model_matrix;
        md.color = info.color;
    }


    std::vector< glm::vec3 > VulkanDriver::getWorldCoords(size_t objectID)
    {
	    //todo objects of different sizes
	    const size_t num_vertices = 3;
	    std::vector< glm::vec3 > res(num_vertices);

	    for(size_t i = 0; i < num_vertices; i++) {
	        res.at(i) = m_worldCoords[num_vertices * objectID + i];
	    }

	    return res;
    }*/



    void VulkanDriver::createSwapChain_()
	{
		const VkSurfaceCapabilitiesKHR& SurfaceCaps = m_core.getSurfaceCaps();
		assert(SurfaceCaps.currentExtent.width != -1);
		
		uint32_t numImages = SurfaceCaps.minImageCount + 1;
		assert(numImages <= SurfaceCaps.maxImageCount);

		VkSwapchainCreateInfoKHR swapChainCreateInfo = {};
		swapChainCreateInfo.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapChainCreateInfo.surface          = m_core.getSurface();
		swapChainCreateInfo.minImageCount    = numImages;
		swapChainCreateInfo.imageFormat      = m_core.getSurfaceFormat().format;
		swapChainCreateInfo.imageColorSpace  = m_core.getSurfaceFormat().colorSpace;
		swapChainCreateInfo.imageExtent      = SurfaceCaps.currentExtent;
		swapChainCreateInfo.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		swapChainCreateInfo.preTransform     = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
		swapChainCreateInfo.imageArrayLayers = 1;
		swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		swapChainCreateInfo.presentMode      = VK_PRESENT_MODE_FIFO_KHR;
		swapChainCreateInfo.clipped          = VK_TRUE;
		swapChainCreateInfo.compositeAlpha   = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

		if (vkCreateSwapchainKHR(m_core.getDevice(), &swapChainCreateInfo, nullptr, &m_swapChainKHR) != VK_SUCCESS) {
			throw std::runtime_error(DEBUG_MSG("failed to create swap chain!"));
		}
		m_deletionQueue.push_function([=]() {
		    vkDestroySwapchainKHR(m_core.getDevice(), m_swapChainKHR, nullptr);
		});


		uint32_t numSwapChainImages = 0;
		if (VK_SUCCESS != vkGetSwapchainImagesKHR(m_core.getDevice(), m_swapChainKHR, &numSwapChainImages, nullptr)) {
            throw std::runtime_error(DEBUG_MSG("failed to get swap chain ImageKHR!"));
        }

		m_images.resize(numSwapChainImages);
		m_frames.resize(numSwapChainImages);
		m_views.resize(numSwapChainImages);

        if (VK_SUCCESS != vkGetSwapchainImagesKHR(m_core.getDevice(), m_swapChainKHR, &numSwapChainImages, m_images.data())) {
            throw std::runtime_error(DEBUG_MSG("failed to get swap chain ImageKHR!"));
        }
	}


	void VulkanDriver::cleanup()
	{
	    vkDeviceWaitIdle(m_core.getDevice());

	    m_deletionQueue.flush();

	}

	
	void VulkanDriver::createCommandBuffer_()
	{
		VkCommandPoolCreateInfo cmdPoolCreateInfo = {};
		cmdPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		cmdPoolCreateInfo.queueFamilyIndex = m_core.getQueueFamily();
		cmdPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

		auto&& device = m_core.getDevice();

		for(auto&& frame : m_frames)
		{
		    if (vkCreateCommandPool(device, &cmdPoolCreateInfo, nullptr, &(frame.commandPool)) != VK_SUCCESS) {
			    throw std::runtime_error(DEBUG_MSG("failed to create graphics command pool!"));
		    }

		    VkCommandBufferAllocateInfo allocInfo = {};
		    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		    allocInfo.commandPool = frame.commandPool;
		    allocInfo.commandBufferCount = 1;
		    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

		    if (vkAllocateCommandBuffers(device, &allocInfo, &frame.mainCommandBuffer) != VK_SUCCESS) {
			    throw std::runtime_error(DEBUG_MSG("failed to allocate command buffers!"));
		    }

		    m_deletionQueue.push_function([=]() {
		        vkDestroyCommandPool(device, frame.commandPool, nullptr);
		    });
		}

	}

#include "../../../OtherLibs/timer.h"
/*

	void VulkanDriver::recordCommandBuffers_()
	{
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0;
		beginInfo.pInheritanceInfo = nullptr;

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };

		VkImageSubresourceRange imageRange = {};
		imageRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageRange.levelCount = 1;
		imageRange.layerCount = 1;

		int wHeight = 0, wWidth = 0;
		glfwGetWindowSize(m_pWindow, &wWidth, &wHeight);

		for (auto&& frame : m_frames)
		{
		    auto&& curBuff = frame.mainCommandBuffer;
		    ezg::Timer timer;
			if (vkBeginCommandBuffer(curBuff, &beginInfo) != VK_SUCCESS) {
				throw std::runtime_error(DEBUG_MSG("failed to begin recording command buffer!"));
			}

			VkRenderPassBeginInfo renderPassInfo = {}
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = m_renderPass;
			renderPassInfo.framebuffer = frame.frameBuffer;
			renderPassInfo.renderArea.offset = { 0, 0 };
			renderPassInfo.renderArea.extent.height = wHeight;
			renderPassInfo.renderArea.extent.width = wWidth;
			renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
			renderPassInfo.pClearValues = clearValues.data();

			vkCmdBeginRenderPass(curBuff, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

			//TODO
			auto&& material = m_renderMaterials[PipelineType::DEFAULT_MESH];
			vkCmdBindPipeline(curBuff, VK_PIPELINE_BIND_POINT_GRAPHICS, material.pipeline);

*/
/*			VkBuffer vertexBuffers[] = {
				m_vertexBuffer
			};
			VkDeviceSize offsets[] = {
				0,
			};*//*

            //std::vector< VkDeviceSize > offsets(m_vertexBuffer.size(), 0);
			//vkCmdBindVertexBuffers(m_cmdBufs[k], 0, m_vertexBuffer.size(), m_vertexBuffer.data(), offsets.data());
            //todo
            uint32_t dynamicOffsets[] = {
                    0,
                    0
            };
			vkCmdBindDescriptorSets(curBuff, VK_PIPELINE_BIND_POINT_GRAPHICS, material.pipelineLayout, 0, 1, &frame.globalDescriptor, 2, dynamicOffsets);


            VkDeviceSize offset = 0;
            vkCmdBindVertexBuffers(curBuff, 0, 1, &m_vertexBuffer._buffer, &offset);


            vkCmdBindIndexBuffer(curBuff, m_indexBuffer._buffer, 0, VK_INDEX_TYPE_UINT32);


			size_t numInstance = m_vertices.size();
            for (size_t cur = 0; cur < numInstance; cur++) {
                // curT - 1 == gl_BaseInstance in vert shader
                //vkCmdDraw(m_cmdBufs[k], m_vertices.at(cur).size(), 1, m_vertexBufferOffsets.at(cur), cur);
                vkCmdDrawIndexed(curBuff, m_indices.at(cur).size(), 1
                                 , m_indexBufferOffsets.at(cur), m_vertexBufferOffsets.at(cur), cur);
            }

			vkCmdEndRenderPass(curBuff);

			if (vkEndCommandBuffer(curBuff) != VK_SUCCESS) {
				throw std::runtime_error(DEBUG_MSG("failed to record command buffer!"));
			}
			std::cout << timer.elapsed() << std::endl;
		}
	}
*/


	void VulkanDriver::createDepthResources_()
	{
		VkFormat depthFormat = m_core.findSupportedFormat(
			{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
			VK_IMAGE_TILING_OPTIMAL,
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
		);
		int wHeight = 0, wWidth = 0;
		glfwGetWindowSize(m_pWindow, &wWidth, &wHeight);

		VkImageCreateInfo dimg_imfo = {};
        dimg_imfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        dimg_imfo.pNext = nullptr;
        dimg_imfo.imageType = VK_IMAGE_TYPE_2D;
        dimg_imfo.format = depthFormat;
        dimg_imfo.extent = VkExtent3D{ static_cast<uint32_t>(wWidth), static_cast<uint32_t>(wHeight), 1 };
        dimg_imfo.mipLevels = 1;
        dimg_imfo.arrayLayers = 1;
        dimg_imfo.samples = VK_SAMPLE_COUNT_1_BIT;
        dimg_imfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        dimg_imfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;


        VmaAllocationCreateInfo dimg_allocinfo = {};
        dimg_allocinfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
        dimg_allocinfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);


        vmaCreateImage(m_allocator, &dimg_imfo, &dimg_allocinfo, &m_depthImage._image, &m_depthImage._allocation, nullptr);

		m_depthImageView = m_core.createImageView(m_depthImage._image, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);

		m_deletionQueue.push_function([=](){
            vkDestroyImageView(m_core.getDevice(), m_depthImageView, nullptr);
            vmaDestroyImage(m_allocator, m_depthImage._image, m_depthImage._allocation);
		});
	}


/*	void VulkanDriver::createVertexBuffer_()
	{
	    const size_t num_buffers = m_vertices.size();

        auto&& device = m_core.getDevice();

        VkBufferCreateInfo  bufferInfo = {};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.pNext = nullptr;
        bufferInfo.size = m_numAllVertices * sizeof(Vertex);
        bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

        VmaAllocationCreateInfo vmaallocInfo = {};
        vmaallocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

        vmaCreateBuffer(m_allocator, &bufferInfo, &vmaallocInfo,
                &m_vertexBuffer._buffer,
                &m_vertexBuffer._allocation,
                nullptr);


        void *data = nullptr;
        vmaMapMemory(m_allocator, m_vertexBuffer._allocation, &data);

	    for (size_t i = 0; i < num_buffers; ++i)
	    {
            auto&& cur_vertices = m_vertices.at(i);
	        const auto curOffset = sizeof(Vertex) * m_vertexBufferOffsets.at(i);
	        const auto curSize = cur_vertices.size() * sizeof(Vertex);

            memcpy(static_cast<char*>(data) + curOffset, cur_vertices.data(), curSize);
        }
        vmaUnmapMemory(m_allocator, m_vertexBuffer._allocation);

	    m_deletionQueue.push_function([=](){
	        vmaDestroyBuffer(m_allocator, m_vertexBuffer._buffer, m_vertexBuffer._allocation);
	    });

    }*/

/*
    void VulkanDriver::createIndexBuffer_()
    {
        auto&& device = m_core.getDevice();
        const size_t num_buffers = m_indices.size();

        VkBufferCreateInfo  bufferInfo = {};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.pNext = nullptr;
        bufferInfo.size = m_numAllIndices * sizeof(m_indices.at(0).at(0));
        bufferInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;

        VmaAllocationCreateInfo vmaallocInfo = {};
        vmaallocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

        vmaCreateBuffer(m_allocator, &bufferInfo, &vmaallocInfo,
                &m_indexBuffer._buffer,
                &m_indexBuffer._allocation,
                nullptr);


        void *data = nullptr;
        vmaMapMemory(m_allocator, m_indexBuffer._allocation, &data);

        for(size_t i = 0; i < num_buffers; ++i)
        {
            auto&& cur_buff = m_indices.at(i);
            auto curOffset = sizeof(cur_buff.at(0)) * m_indexBufferOffsets.at(i);
            auto curSize = sizeof(cur_buff.at(0)) * cur_buff.size();

            memcpy(static_cast<char*>(data) + curOffset, cur_buff.data(), curSize);
        }

        vmaUnmapMemory(m_allocator, m_indexBuffer._allocation);


        m_deletionQueue.push_function([=](){
            vmaDestroyBuffer(m_allocator, m_indexBuffer._buffer, m_indexBuffer._allocation);
        });
    }
*/



    void VulkanDriver::initDescriptors()
    {
        std::vector<VkDescriptorPoolSize> sizes =
                {
                        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 10 },
                        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 10 },
                        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 10 },
                        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 10}
                };

        VkDescriptorPoolCreateInfo pool_info = {};
        pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.flags = 0;
        pool_info.maxSets = 10;
        pool_info.poolSizeCount = (uint32_t)sizes.size();
        pool_info.pPoolSizes = sizes.data();

        auto&& device = m_core.getDevice();
        vkCreateDescriptorPool(device, &pool_info, nullptr, &m_descriptorPool);


        VkDescriptorSetLayoutBinding uboLayoutBinding = {};
        uboLayoutBinding.binding            = 0;
        uboLayoutBinding.descriptorType     = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboLayoutBinding.descriptorCount    = 1;
        uboLayoutBinding.stageFlags         = VK_SHADER_STAGE_VERTEX_BIT;
        uboLayoutBinding.pImmutableSamplers = nullptr;

        VkDescriptorSetLayoutBinding modelLayoutBinding = {};
        modelLayoutBinding.binding            = 1;
        modelLayoutBinding.descriptorType     = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
        modelLayoutBinding.descriptorCount    = 1;
        modelLayoutBinding.stageFlags         = VK_SHADER_STAGE_VERTEX_BIT;
        modelLayoutBinding.pImmutableSamplers = nullptr;

        VkDescriptorSetLayoutBinding coordsLayoutBinding = {};
        coordsLayoutBinding.binding            = 2;
        coordsLayoutBinding.descriptorType     = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
        coordsLayoutBinding.descriptorCount    = 1;
        coordsLayoutBinding.stageFlags         = VK_SHADER_STAGE_VERTEX_BIT;
        coordsLayoutBinding.pImmutableSamplers = nullptr;

        VkDescriptorSetLayoutBinding bindings[] {
                uboLayoutBinding,
                modelLayoutBinding,
                coordsLayoutBinding
        };

        VkDescriptorSetLayoutCreateInfo layoutInfo = {};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = 3;
        layoutInfo.pBindings = bindings;

        if (vkCreateDescriptorSetLayout(m_core.getDevice(), &layoutInfo, nullptr, &m_globalSetLayout) != VK_SUCCESS) {
            throw std::runtime_error(DEBUG_MSG("failed to create descriptor set layput!"));
        }


        for(auto&& frame : m_frames)
        {
            frame.cameraBuffer = create_buffer_(sizeof(GPUCameraData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);

            frame.objectBuffer = create_buffer_(sizeof(GPUObjectData) * m_numObjects, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);

            //TODO
            frame.storageBufferWorldCoords = create_buffer_(sizeof(Vertex) * m_numObjects, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);

            VkDescriptorSetAllocateInfo allocInfo = {};
            allocInfo.pNext = nullptr;
            allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            allocInfo.descriptorPool = m_descriptorPool;
            allocInfo.descriptorSetCount = 1;
            allocInfo.pSetLayouts = &m_globalSetLayout;

            vkAllocateDescriptorSets(device, &allocInfo, &frame.globalDescriptor);



            VkDescriptorBufferInfo cameraInfo;
            cameraInfo.buffer = frame.cameraBuffer._buffer;
            cameraInfo.offset = 0;
            cameraInfo.range = sizeof(GPUCameraData);


            VkDescriptorBufferInfo objectBufferInfo;
            objectBufferInfo.buffer = frame.objectBuffer._buffer;
            objectBufferInfo.offset = 0;
            objectBufferInfo.range = sizeof(GPUObjectData) * m_numObjects;

            ////


            VkWriteDescriptorSet descriptorWriteUbo = {};
            descriptorWriteUbo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWriteUbo.dstSet = frame.globalDescriptor;
            descriptorWriteUbo.dstBinding = 0;
            descriptorWriteUbo.dstArrayElement = 0;
            descriptorWriteUbo.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWriteUbo.descriptorCount = 1;
            descriptorWriteUbo.pBufferInfo = &cameraInfo;
            descriptorWriteUbo.pImageInfo = nullptr;
            descriptorWriteUbo.pTexelBufferView = nullptr;

            ///

            VkWriteDescriptorSet descriptorWriteModel1 = {};
            descriptorWriteModel1.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWriteModel1.dstSet = frame.globalDescriptor;
            descriptorWriteModel1.dstBinding = 1;
            descriptorWriteModel1.dstArrayElement = 0;
            descriptorWriteModel1.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
            descriptorWriteModel1.descriptorCount = 1;
            descriptorWriteModel1.pBufferInfo = &objectBufferInfo;
            descriptorWriteModel1.pImageInfo = nullptr;
            descriptorWriteModel1.pTexelBufferView = nullptr;

            ///

            //TODO
            VkDescriptorBufferInfo bufferInfoColors = {};
            bufferInfoColors.buffer = frame.storageBufferWorldCoords._buffer;
            bufferInfoColors.offset = 0;
            bufferInfoColors.range = sizeof(Vertex) * m_numObjects;

            VkWriteDescriptorSet descriptorWriteColors = {};
            descriptorWriteColors.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWriteColors.dstSet = frame.globalDescriptor;
            descriptorWriteColors.dstBinding = 2;
            descriptorWriteColors.dstArrayElement = 0;
            descriptorWriteColors.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
            descriptorWriteColors.descriptorCount = 1;
            descriptorWriteColors.pBufferInfo = &bufferInfoColors;
            descriptorWriteColors.pImageInfo = nullptr;
            descriptorWriteColors.pTexelBufferView = nullptr;


            VkWriteDescriptorSet descriptorWrites[] = {
                    descriptorWriteUbo,
                    descriptorWriteModel1,
                    descriptorWriteColors
            };

            vkUpdateDescriptorSets(device, 3, descriptorWrites, 0, nullptr);
        }

        m_deletionQueue.push_function([&]() {

            //vmaDestroyBuffer(m_allocator, _sceneParameterBuffer._buffer, _sceneParameterBuffer._allocation);
            //vkDestroyDescriptorSetLayout(device, m_objectSetLayout, nullptr);
            vkDestroyDescriptorSetLayout(device, m_globalSetLayout, nullptr);

            vkDestroyDescriptorPool(device, m_descriptorPool, nullptr);

            for (auto&& frame : m_frames)
            {
                vmaDestroyBuffer(m_allocator, frame.cameraBuffer._buffer, frame.cameraBuffer._allocation);

                vmaDestroyBuffer(m_allocator, frame.objectBuffer._buffer, frame.objectBuffer._allocation);

                vmaDestroyBuffer(m_allocator, frame.storageBufferWorldCoords._buffer, frame.storageBufferWorldCoords._allocation);
            }
        });
    }



	void VulkanDriver::updateUniformBuffer_(uint32_t currentImage_)
	{
		static auto startTime = std::chrono::high_resolution_clock::now();

		auto currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration< float, std::chrono::seconds::period >(currentTime - startTime).count();
	
		GPUCameraData ubo = {};
		ubo.view = m_cameraView.getViewMatrix();
		ubo.proj = m_cameraView.getProjectionMatrix();

		auto&& curFrame = m_frames[currentImage_];

		void* data = nullptr;
		auto device = m_core.getDevice();
		vmaMapMemory(m_allocator, curFrame.cameraBuffer._allocation, &data);
        memcpy(data, &ubo, sizeof(ubo));
        vmaUnmapMemory(m_allocator, curFrame.cameraBuffer._allocation);
        data = nullptr;



        vmaMapMemory(m_allocator, curFrame.objectBuffer._allocation, &data);

        auto* pModelData = static_cast< GPUObjectData* >(data);
        for (size_t i = 0, mi = m_renderables.size(); i < mi; i++) {
            pModelData[i] = { m_renderables.at(i).transformMatrix, {} };
        }

        vmaUnmapMemory(m_allocator, curFrame.objectBuffer._allocation);


        //todo
        //updateWorldCoordsData(currentImage_);
	}


/*    void VulkanDriver::updateWorldCoordsData(uint32_t currentImage_)
    {
        void* data = nullptr;
        auto device = m_core.getDevice();
        //m_worldCoords.resize(vertices.size());
        auto&& curFrame = m_frames[currentImage_];


        vmaMapMemory(m_allocator, curFrame.storageBufferWorldCoords._allocation, &data);

        //todo
        auto* pOutData = static_cast< OutputVertShader* >(data);
        for (size_t i = 0, mi = m_worldCoords.size(); i < mi; ++i)
        {
            m_worldCoords[i] = pOutData[i].world_coord;
        }
        vmaUnmapMemory(m_allocator, curFrame.storageBufferWorldCoords._allocation);


    }*/


    void VulkanDriver::upload_mesh(Mesh& mesh)
    {
        //allocate vertex buffer
        VkBufferCreateInfo bufferInfo = {};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.pNext = nullptr;
        //this is the total size, in bytes, of the buffer we are allocating
        bufferInfo.size = mesh._vertices.size() * sizeof(Vertex);
        //this buffer is going to be used as a Vertex Buffer
        bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;


        //let the VMA library know that this data should be writeable by CPU, but also readable by GPU
        VmaAllocationCreateInfo vmaallocInfo = {};
        vmaallocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

        //allocate the buffer
        vmaCreateBuffer(m_allocator, &bufferInfo, &vmaallocInfo,
                &mesh._vertexBuffer._buffer,
                &mesh._vertexBuffer._allocation,
                nullptr);

        //add the destruction of triangle mesh buffer to the deletion queue
        m_deletionQueue.push_function([=]() {

            vmaDestroyBuffer(m_allocator, mesh._vertexBuffer._buffer, mesh._vertexBuffer._allocation);
        });

        //copy vertex data
        void* data;
        vmaMapMemory(m_allocator, mesh._vertexBuffer._allocation, &data);

        memcpy(data, mesh._vertices.data(), mesh._vertices.size() * sizeof(Vertex));

        vmaUnmapMemory(m_allocator, mesh._vertexBuffer._allocation);
    }



	void VulkanDriver::render()
	{
        auto&& curFrame = getCurFrame();
        int wHeight = 0, wWidth = 0;
        glfwGetWindowSize(m_pWindow, &wWidth, &wHeight);

		vkWaitForFences(m_core.getDevice(), 1, &curFrame.renderFence, VK_TRUE, UINT64_MAX);
        vkResetFences(m_core.getDevice(), 1, &curFrame.renderFence);

        VkCommandBuffer cmd = curFrame.mainCommandBuffer;
        vkResetCommandBuffer(cmd, 0);

		uint32_t imageIndex = 0;
		VkResult result = vkAcquireNextImageKHR(m_core.getDevice(), m_swapChainKHR, UINT64_MAX, curFrame.renderSemaphore, VK_NULL_HANDLE, &imageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			m_framebufferResized = false;
			assert(0);
			//recreateSwapChain_();
			return;
		}
		else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error(DEBUG_MSG("failed to acquire swap chain image!"));
		}


        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0;
        beginInfo.pInheritanceInfo = nullptr;

        if (vkBeginCommandBuffer(cmd, &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error(DEBUG_MSG("failed to begin recording command buffer!"));
        }

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
        clearValues[1].depthStencil = { 1.0f, 0 };

        VkRenderPassBeginInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = m_renderPass;
        renderPassInfo.framebuffer = m_frameBuffers[imageIndex];
        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent.height = wHeight;
        renderPassInfo.renderArea.extent.width = wWidth;
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(cmd, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        //TODO draw objects

        vkCmdEndRenderPass(cmd);

        vkEndCommandBuffer(cmd);

/*		// Check if a previous frame is using this image (i.e. there is its fence to wait on)
		if (m_imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
			vkWaitForFences(m_core.getDevice(), 1, &m_imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
		}
		// Mark the image as now being in use by this frame
		m_imagesInFlight[imageIndex] = m_inFlightFences[m_currentFrame];*/

		//todo
		//updateUniformBuffer_(m_currentFrame);
		//todo

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = { curFrame.renderSemaphore };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &cmd;

		VkSemaphore signalSemaphores[] = { curFrame.presentSemaphore };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;


		if (VK_SUCCESS != vkQueueSubmit(m_queue, 1, &submitInfo, curFrame.renderFence)) {
			throw std::runtime_error(DEBUG_MSG("failed to submit draw command buffer!"));
		}

		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &m_swapChainKHR;
		presentInfo.pImageIndices = &imageIndex;

		presentInfo.pResults = nullptr;

		result = vkQueuePresentKHR(m_queue, &presentInfo); 
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_framebufferResized) {
			m_framebufferResized = false;

			assert(0);
			//recreateSwapChain_();
		} 
		else if (result != VK_SUCCESS) {
			throw std::runtime_error(DEBUG_MSG("failed to present swap chain image!"));
		}

		//vkQueueWaitIdle(m_queue);
		m_currentFrame = m_currentFrame + 1;
	}


	void VulkanDriver::createRenderPass_()
	{
		VkAttachmentDescription colorAttachment = {};
		colorAttachment.format         = m_core.getSurfaceFormat().format;
		colorAttachment.samples        = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
		//colorAttachment.initialLayout  = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		colorAttachment.finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference attachRef = {};
		attachRef.attachment = 0;
		attachRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentDescription depthAttachment = {};
		depthAttachment.format = m_core.findSupportedFormat(
			{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
			VK_IMAGE_TILING_OPTIMAL,
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
		);
		depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depthAttachmentRef = {};
		depthAttachmentRef.attachment = 1;
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;


		VkSubpassDescription subpassDesc = {};
		subpassDesc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpassDesc.colorAttachmentCount = 1;
		subpassDesc.pColorAttachments = &attachRef;
		subpassDesc.pDepthStencilAttachment = &depthAttachmentRef;

		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		std::array< VkAttachmentDescription, 2 > attachments = { colorAttachment, depthAttachment };

		VkRenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		renderPassInfo.pAttachments = attachments.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpassDesc;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		if (vkCreateRenderPass(m_core.getDevice(), &renderPassInfo, nullptr, &m_renderPass) != VK_SUCCESS) {
			throw std::runtime_error(DEBUG_MSG("failed to create render pass!"));
		}
		m_deletionQueue.push_function([=](){
		    vkDestroyRenderPass(m_core.getDevice(), m_renderPass, nullptr);
		});
	}


	void VulkanDriver::createFramebuffer_()
	{
		int wHeight = 0, wWidth = 0;
		glfwGetWindowSize(m_pWindow, &wWidth, &wHeight);

		m_frameBuffers.resize(m_maxFramesInFlight);

		for (uint32_t i = 0, mi = m_images.size(); i < mi; i++)
		{
			m_views[i] = m_core.createImageView(m_images[i], m_core.getSurfaceFormat().format, VK_IMAGE_ASPECT_COLOR_BIT);
            m_deletionQueue.push_function([=](){
                vkDestroyImageView(m_core.getDevice(), m_views[i], nullptr);
            });

			std::array< VkImageView, 2 > attachments = {
				m_views[i],
				m_depthImageView
			};

			VkFramebufferCreateInfo fbInfo = {};
			fbInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			fbInfo.renderPass = m_renderPass;
			fbInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
			fbInfo.pAttachments = attachments.data();
			fbInfo.width = wWidth;
			fbInfo.height = wHeight;
			fbInfo.layers = 1;

			if (VK_SUCCESS != vkCreateFramebuffer(m_core.getDevice(), &fbInfo, NULL, &m_frameBuffers[i])) {
				throw std::runtime_error(DEBUG_MSG("failed to create frame buffer"));
			}
			m_deletionQueue.push_function([=](){
			    vkDestroyFramebuffer(m_core.getDevice(), m_frameBuffers[i], nullptr);
			});
		}
	}


	VkShaderModule VulkanDriver::createShaderModule_(const std::vector< char >& source_)
	{
		VkShaderModuleCreateInfo createInfo = {}; 
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = source_.size();
		createInfo.pCode = reinterpret_cast< const uint32_t* >(source_.data());

		VkShaderModule sModule = nullptr;
		if (vkCreateShaderModule(m_core.getDevice(), &createInfo, nullptr, &sModule) != VK_SUCCESS) {
			throw std::runtime_error(DEBUG_MSG("failed to create shader module!"));
		}

		return sModule;
	}


	void VulkanDriver::createPipeline_()
	{
		auto vsCode = readFile(vert_shader_fname);
		auto fsCode = readFile(frag_shader_fname);

		VkShaderModule vertShader = createShaderModule_(vsCode);
		VkShaderModule fragShader = createShaderModule_(fsCode);


		VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageInfo.module = vertShader;
		vertShaderStageInfo.pName = "main";


		VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = fragShader;
		fragShaderStageInfo.pName = "main";


		PipelineBuildInfo pipelineBuildInfo;
		pipelineBuildInfo.shaderStages.push_back(vertShaderStageInfo);
		pipelineBuildInfo.shaderStages.push_back(fragShaderStageInfo);

        std::vector< VkDescriptorSetLayout > setLayouts = {
                m_globalSetLayout
                //, m_objectSetLayout TODO
        };

        VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = setLayouts.size();
        pipelineLayoutInfo.pSetLayouts = setLayouts.data();
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;

        VkPipelineLayout pipelineLayout = {};
        if (vkCreatePipelineLayout(m_core.getDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error(DEBUG_MSG("failed to create pipeline layout!"));
        }
        pipelineBuildInfo.pipelineLayout = pipelineLayout;
        m_deletionQueue.push_function([=](){
            vkDestroyPipelineLayout(m_core.getDevice(), pipelineLayout, nullptr);
        });



		auto bindingDescription = Vertex::getBindingDescription();
		auto attributeDescriptions = Vertex::getAttributeDescription();

		VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = 1;
		vertexInputInfo.pVertexBindingDescriptions = &bindingDescription; // Optional
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
		vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data(); // Optional

		pipelineBuildInfo.vertexInputInfo = vertexInputInfo;


		VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		pipelineBuildInfo.inputAssembly = inputAssembly;


		int wHeight = 0, wWidth = 0;
		glfwGetWindowSize(m_pWindow, &wWidth, &wHeight);

		pipelineBuildInfo.viewport.x = 0.f;
		pipelineBuildInfo.viewport.y = 0.f;
		pipelineBuildInfo.viewport.width =  static_cast<float>(wWidth);
		pipelineBuildInfo.viewport.height = static_cast<float>(wHeight);
		pipelineBuildInfo.viewport.minDepth = 0.f;
		pipelineBuildInfo.viewport.maxDepth = 1.f;

		pipelineBuildInfo.scissor.offset = { 0, 0 };
		pipelineBuildInfo.scissor.extent.width = wWidth;
		pipelineBuildInfo.scissor.extent.height = wHeight;



/*

		VkPipelineViewportStateCreateInfo viewportState = {};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.pViewports = &viewport;
		viewportState.scissorCount = 1;
		viewportState.pScissors = &scissor;*/
		
		pipelineBuildInfo.rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		pipelineBuildInfo.rasterizer.depthClampEnable = VK_FALSE;
		pipelineBuildInfo.rasterizer.rasterizerDiscardEnable = VK_FALSE;
		pipelineBuildInfo.rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		pipelineBuildInfo.rasterizer.lineWidth = 1.f;
		//todo
		pipelineBuildInfo.rasterizer.cullMode = VK_CULL_MODE_NONE;
		//rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		pipelineBuildInfo.rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		//end todo
		pipelineBuildInfo.rasterizer.depthClampEnable = VK_FALSE;
		pipelineBuildInfo.rasterizer.depthBiasConstantFactor = 0.f;
		pipelineBuildInfo.rasterizer.depthBiasClamp = 0.f;
		pipelineBuildInfo.rasterizer.depthBiasSlopeFactor = 0.f;


		pipelineBuildInfo.depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		pipelineBuildInfo.depthStencil.depthTestEnable = VK_TRUE;
		pipelineBuildInfo.depthStencil.depthWriteEnable = VK_TRUE;
		pipelineBuildInfo.depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
		pipelineBuildInfo.depthStencil.depthBoundsTestEnable = VK_FALSE;
		pipelineBuildInfo.depthStencil.minDepthBounds = 0.0f;
		pipelineBuildInfo.depthStencil.maxDepthBounds = 1.0f;
		pipelineBuildInfo.depthStencil.stencilTestEnable = VK_FALSE;
		pipelineBuildInfo.depthStencil.front = {};
		pipelineBuildInfo.depthStencil.back = {};


		pipelineBuildInfo.multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		pipelineBuildInfo.multisampling.sampleShadingEnable = VK_FALSE;
		pipelineBuildInfo.multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		pipelineBuildInfo.multisampling.minSampleShading = 1.f;
		pipelineBuildInfo.multisampling.pSampleMask = nullptr;
		pipelineBuildInfo.multisampling.alphaToCoverageEnable = VK_FALSE;
		pipelineBuildInfo.multisampling.alphaToOneEnable = VK_FALSE;


        pipelineBuildInfo.colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT
			| VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		pipelineBuildInfo.colorBlendAttachment.blendEnable = VK_FALSE;
		pipelineBuildInfo.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
		pipelineBuildInfo.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
		pipelineBuildInfo.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		pipelineBuildInfo.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		pipelineBuildInfo.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		pipelineBuildInfo.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

		VkPipeline meshPipeline = pipelineBuildInfo.build_pipeline(m_core.getDevice(), m_renderPass);

		//todo
		m_renderMaterials[PipelineType::DEFAULT_MESH] = {meshPipeline, pipelineLayout };

		m_deletionQueue.push_function([=](){
		    vkDestroyPipeline(m_core.getDevice(), meshPipeline, nullptr);
		});

		vkDestroyShaderModule(m_core.getDevice(), fragShader, nullptr);
		vkDestroyShaderModule(m_core.getDevice(), vertShader, nullptr);
	}


	void VulkanDriver::createSyncObjects_()
	{
        VkFenceCreateInfo fenceInfo = {};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        VkSemaphoreCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        auto&& device = m_core.getDevice();

	    for(auto&& frame : m_frames)
        {
            vkCreateFence(device, &fenceInfo, nullptr, &frame.renderFence);

            //enqueue the destruction of the fence
            m_deletionQueue.push_function([=]() {
                vkDestroyFence(device, frame.renderFence, nullptr);
            });


            vkCreateSemaphore(device, &createInfo, nullptr, &frame.presentSemaphore);
            vkCreateSemaphore(device, &createInfo, nullptr, &frame.renderSemaphore);

            m_deletionQueue.push_function([=]() {
                vkDestroySemaphore(device, frame.presentSemaphore, nullptr);
                vkDestroySemaphore(device, frame.renderSemaphore, nullptr);
            });
        }

	}


    AllocatedBuffer VulkanDriver::create_buffer_(size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage)
    {
        //allocate vertex buffer
        VkBufferCreateInfo bufferInfo = {};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.pNext = nullptr;
        bufferInfo.size = allocSize;

        bufferInfo.usage = usage;


        //let the VMA library know that this data should be writeable by CPU, but also readable by GPU
        VmaAllocationCreateInfo vmaallocInfo = {};
        vmaallocInfo.usage = memoryUsage;

        AllocatedBuffer newBuffer = {};

        //allocate the buffer
        vmaCreateBuffer(m_allocator, &bufferInfo, &vmaallocInfo,
                &newBuffer._buffer,
                &newBuffer._allocation,
                nullptr);

        return newBuffer;
    }

}//namespace vks
