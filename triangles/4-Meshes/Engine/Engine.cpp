/*************************************************************************
 *
 * Driver.cpp
 *
 * Created by dmitry
 * 22.02.2021
 *
 ***/


//
/// Engine
///======================================================================================
///
///======================================================================================
///======================================================================================
//

#include "Engine.hpp"
#include "PipelineBuilder.hpp"


#include <cassert>
#include <cstring>
#include <chrono>
#include <sstream>


#define VMA_IMPLEMENTATION

#include "vk_mem_alloc.h"


constexpr char vert_shader_fname[] = "resource/shaders/vert.spv";
constexpr char frag_shader_fname[] = "resource/shaders/frag.spv";

namespace ezg
{


    Engine::Engine(const Window& window)
            : m_rWindow(window), m_core(window)
    {
        VmaAllocatorCreateInfo allocatorInfo = {};
        allocatorInfo.physicalDevice = m_core.getPhysDevice();
        allocatorInfo.device = m_core.getDevice();
        allocatorInfo.instance = m_core.getInstance();
        vmaCreateAllocator(&allocatorInfo, &m_allocator);

        // all added functions are executed in the destructor of the delete queue.
        // So when throwing an exception in the constructor, all created elements
        // will be deleted eventually.
        m_deletionQueue.push([allocator = m_allocator]()
                             {
                                 vmaDestroyAllocator(allocator);
                             });

        vkGetDeviceQueue(m_core.getDevice(), m_core.getQueueFamily(), 0, &m_queue);

        createSwapChain_();

        createRenderPass_();

        createCommandBuffer_();
        createDepthResources_();

        createFramebuffer_();

        initDescriptors();

        createPipeline_();

        createSyncObjects_();


        //TODO
        prepareCubeFrameBuffers_t();
        createReflectionDescriptors_t();
        createReflectionPipeLine_t();
    }


    Engine::~Engine()
    {
        cleanup();
    }


    void Engine::createSwapChain_()
    {
        const VkSurfaceCapabilitiesKHR& SurfaceCaps = m_core.getSurfaceCaps();

        uint32_t numImages = SurfaceCaps.minImageCount + 1;
        assert(numImages <= SurfaceCaps.maxImageCount);

        VkSwapchainCreateInfoKHR swapChainCreateInfo = {};
        swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapChainCreateInfo.surface = m_core.getSurface();
        swapChainCreateInfo.minImageCount = numImages;
        swapChainCreateInfo.imageFormat = m_core.getSurfaceFormat().format;
        swapChainCreateInfo.imageColorSpace = m_core.getSurfaceFormat().colorSpace;
        swapChainCreateInfo.imageExtent = SurfaceCaps.currentExtent;
        swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        swapChainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        swapChainCreateInfo.imageArrayLayers = 1;
        swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapChainCreateInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
        swapChainCreateInfo.clipped = VK_TRUE;
        swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

        if (vkCreateSwapchainKHR(m_core.getDevice(), &swapChainCreateInfo, nullptr, &m_swapChainKHR) != VK_SUCCESS) {
            throw std::runtime_error(("failed to create swap chain!"));
        }
        m_deletionQueue.push([dev = m_core.getDevice(), sc = m_swapChainKHR]()
                             {
                                 vkDestroySwapchainKHR(dev, sc, nullptr);
                             });


        uint32_t numSwapChainImages = 0;
        if (VK_SUCCESS != vkGetSwapchainImagesKHR(m_core.getDevice(), m_swapChainKHR, &numSwapChainImages, nullptr)) {
            throw std::runtime_error(("failed to get swap chain ImageKHR!"));
        }

        m_images.resize(numSwapChainImages);
        m_frames.resize(numSwapChainImages);
        m_views.resize(numSwapChainImages);

        if (VK_SUCCESS !=
            vkGetSwapchainImagesKHR(m_core.getDevice(), m_swapChainKHR, &numSwapChainImages, m_images.data())) {
            throw std::runtime_error(("failed to get swap chain ImageKHR!"));
        }
    }


    void Engine::cleanup()
    {
        if (m_core.getDevice() != nullptr)
            vkDeviceWaitIdle(m_core.getDevice());

        m_deletionQueue.flush();
    }


    void Engine::createCommandBuffer_()
    {
        VkCommandPoolCreateInfo cmdPoolCreateInfo = {};
        cmdPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        cmdPoolCreateInfo.queueFamilyIndex = m_core.getQueueFamily();
        cmdPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

        auto&& device = m_core.getDevice();

        for (auto&& frame : m_frames) {
            if (vkCreateCommandPool(device, &cmdPoolCreateInfo, nullptr, &(frame.commandPool)) != VK_SUCCESS) {
                throw std::runtime_error(("failed to create graphics command pool!"));
            }

            VkCommandBufferAllocateInfo allocInfo = {};
            allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            allocInfo.commandPool = frame.commandPool;
            allocInfo.commandBufferCount = 1;
            allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

            if (vkAllocateCommandBuffers(device, &allocInfo, &frame.mainCommandBuffer) != VK_SUCCESS) {
                throw std::runtime_error(("failed to allocate command buffers!"));
            }

            m_deletionQueue.push([device, cmp = frame.commandPool]()
                                 {
                                     vkDestroyCommandPool(device, cmp, nullptr);
                                 });
        }

    }


    void Engine::createDepthResources_()
    {
        VkFormat depthFormat = m_core.findSupportedFormat(
                {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT}
                , VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
        );
        int wHeight = m_rWindow.getHeight(), wWidth = m_rWindow.getWidth();

        m_depthImage = create_image_(VK_IMAGE_TYPE_2D
                                     , depthFormat
                                     , 0, VK_SAMPLE_COUNT_1_BIT
                                     , VK_IMAGE_TILING_OPTIMAL
                                     , VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT
                                     , VMA_MEMORY_USAGE_GPU_ONLY
                                     , VkExtent3D{static_cast<uint32_t>(wWidth), static_cast<uint32_t>(wHeight), 1}
                                     , 1, 1);


        m_depthImageView = m_core.createImageView(m_depthImage._image
                                                  , depthFormat
                                                  , VK_IMAGE_VIEW_TYPE_2D
                                                  , VK_IMAGE_ASPECT_DEPTH_BIT);

        m_deletionQueue.push(
                [dev = m_core.getDevice(), dIm = m_depthImage, dImV = m_depthImageView, alloc = m_allocator]()
                {
                    vkDestroyImageView(dev, dImV, nullptr);
                    vmaDestroyImage(alloc, dIm._image, dIm._allocation);
                });
    }


    void Engine::initDescriptors()
    {
        std::vector< VkDescriptorPoolSize > sizes =
                {
                        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,         10},
                        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 10},
                        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,         10},
                        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 10},
                        {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 10}
                };

        VkDescriptorPoolCreateInfo pool_info = {};
        pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.flags = 0;
        pool_info.maxSets = 10;
        pool_info.poolSizeCount = (uint32_t) sizes.size();
        pool_info.pPoolSizes = sizes.data();

        auto&& device = m_core.getDevice();
        vkCreateDescriptorPool(device, &pool_info, nullptr, &m_descriptorPool);
        m_deletionQueue.push([device, dp = m_descriptorPool]()
                             {
                                 vkDestroyDescriptorPool(device, dp, nullptr);
                             });

        VkDescriptorSetLayoutBinding uboLayoutBinding = {};
        uboLayoutBinding.binding = 0;
        uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
        uboLayoutBinding.descriptorCount = 1;
        uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        uboLayoutBinding.pImmutableSamplers = nullptr;

        VkDescriptorSetLayoutBinding modelLayoutBinding = {};
        modelLayoutBinding.binding = 1;
        modelLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        modelLayoutBinding.descriptorCount = 1;
        modelLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        modelLayoutBinding.pImmutableSamplers = nullptr;

        VkDescriptorSetLayoutBinding bindings[]{
                uboLayoutBinding,
                modelLayoutBinding
                //coordsLayoutBinding
        };

        VkDescriptorSetLayoutCreateInfo layoutInfo = {};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = 2;
        layoutInfo.pBindings = bindings;

        if (vkCreateDescriptorSetLayout(m_core.getDevice(), &layoutInfo, nullptr, &m_globalSetLayout) != VK_SUCCESS) {
            throw std::runtime_error(("failed to create descriptor set layput!"));
        }
        m_deletionQueue.push([device, gsl = m_globalSetLayout]()
                             {
                                 vkDestroyDescriptorSetLayout(device, gsl, nullptr);
                             });


        for (auto&& frame : m_frames) {
            frame.cameraBuffer = create_buffer_(sizeof(GPUCameraData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT
                                                , VMA_MEMORY_USAGE_CPU_TO_GPU);
            m_deletionQueue.push([alloc = m_allocator, cb = frame.cameraBuffer]()
                                 {
                                     vmaDestroyBuffer(alloc, cb._buffer, cb._allocation);
                                 });

            frame.objectBuffer = create_buffer_(sizeof(GPUObjectData) * m_numObjects, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
                                                , VMA_MEMORY_USAGE_CPU_TO_GPU);
            m_deletionQueue.push([alloc = m_allocator, ob = frame.objectBuffer]()
                                 {
                                     vmaDestroyBuffer(alloc, ob._buffer, ob._allocation);
                                 });

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
            descriptorWriteUbo.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
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
            descriptorWriteModel1.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            descriptorWriteModel1.descriptorCount = 1;
            descriptorWriteModel1.pBufferInfo = &objectBufferInfo;
            descriptorWriteModel1.pImageInfo = nullptr;
            descriptorWriteModel1.pTexelBufferView = nullptr;


            VkWriteDescriptorSet descriptorWrites[] = {
                    descriptorWriteUbo,
                    descriptorWriteModel1
                    //descriptorWriteColors
            };

            vkUpdateDescriptorSets(device, 2, descriptorWrites, 0, nullptr);
        }

    }


    void Engine::upload_mesh(Mesh& mesh)
    {
        if (mesh.vertices.empty()) {
            throw std::runtime_error("cant create a mesh with empty vertex data!");
        }
        if (mesh.m_isUploaded) {
            std::cerr << "mesh already uploaded" << std::endl;
            return;
        }

        mesh.m_vertexBuffer = create_buffer_(mesh.vertices.size() * sizeof(Vertex)
                                             , VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
                                             , VMA_MEMORY_USAGE_CPU_TO_GPU);

        //copy vertex data
        void* data;
        vmaMapMemory(m_allocator, mesh.m_vertexBuffer._allocation, &data);
        memcpy(data, mesh.vertices.data(), mesh.vertices.size() * sizeof(Vertex));
        vmaUnmapMemory(m_allocator, mesh.m_vertexBuffer._allocation);

        mesh.m_isUploaded = true;
    }


    void Engine::unload_mesh(Mesh& mesh)
    {
        if (!mesh.m_isUploaded) {
            std::cerr << "I can not upload data that has not been downloaded" << std::endl;
        } else {
            vkQueueWaitIdle(m_queue);
            vmaDestroyBuffer(m_allocator, mesh.m_vertexBuffer._buffer, mesh.m_vertexBuffer._allocation);

            mesh.m_isUploaded = false;
            mesh.m_vertexBuffer = {};
        }
    }


    void Engine::updateUniformBuffer_(uint32_t currentImage_, const std::vector< Mesh* >& meshes)
    {
        GPUCameraData ubo = {};
        ubo.view = m_cameraView.getViewMatrix();
        ubo.proj = m_cameraView.getProjectionMatrix();

        GPUCameraData uboEmt = {
                .view = glm::mat4(1.f),
                .proj = glm::mat4(1.f)
        };

        std::vector< GPUCameraData > ubos = {
                ubo, uboEmt
        };

        auto&& curFrame = m_frames[currentImage_];

        //todo push constant
/*        vmaMapMemory(m_allocator, curFrame.cameraBuffer._allocation, &data);
        memcpy(data, ubos.data(), sizeof(ubo) * ubos.size());
        vmaUnmapMemory(m_allocator, curFrame.cameraBuffer._allocation);
        data = nullptr;*/

        void* data = nullptr;
        vmaMapMemory(m_allocator, curFrame.objectBuffer._allocation, &data);

        auto* pModelData = static_cast< GPUObjectData* >(data);
        for (size_t i = 0, mi = meshes.size(); i < mi; i++) {
            pModelData[i].model = meshes[i]->getModelMatrix();
            pModelData[i].color = meshes[i]->getColor();
        }

        vmaUnmapMemory(m_allocator, curFrame.objectBuffer._allocation);
    }


    void Engine::render_meshes(const std::vector< Mesh* >& objects)
    {

        auto&& curFrame = getCurFrame();
        int wHeight = m_rWindow.getHeight(), wWidth = m_rWindow.getWidth();


        vkWaitForFences(m_core.getDevice(), 1, &curFrame.renderFence, VK_TRUE, UINT64_MAX);
        vkResetFences(m_core.getDevice(), 1, &curFrame.renderFence);

        VkCommandBuffer cmd = curFrame.mainCommandBuffer;
        vkResetCommandBuffer(cmd, 0);

        uint32_t imageIndex = 0;
        VkResult result = vkAcquireNextImageKHR(m_core.getDevice(), m_swapChainKHR, UINT64_MAX, curFrame.renderSemaphore
                                                , VK_NULL_HANDLE, &imageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            assert(0); //TODO resize window
            return;
        } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error(("failed to acquire swap chain image!"));
        }


        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0;
        beginInfo.pInheritanceInfo = nullptr;

        if (vkBeginCommandBuffer(cmd, &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error(("failed to begin recording command buffer!"));
        }

        updateUniformBuffer_(m_currentFrame % m_maxFramesInFlight, objects);

        RenderMaterial* lastMaterial = nullptr;
#if 1
        ///TODO===================================================
        const glm::vec3 mirrorPosition = objects[0]->getPosition();
        for (size_t k = 0; k < m_cubeFrameBuffers.size(); ++k) {
            std::array< VkClearValue, 2 > clearValues{};
            clearValues[0].color = {0.0f, 0.0f, 0.5f, 1.0f};
            clearValues[1].depthStencil = {1.0f, 0};

            VkRenderPassBeginInfo renderPassInfo = {};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = m_renderPass;
            renderPassInfo.framebuffer = m_cubeFrameBuffers[k].frameBuffer;
            renderPassInfo.renderArea.offset = {0, 0};
            renderPassInfo.renderArea.extent.height = m_cubeExtent.height;
            renderPassInfo.renderArea.extent.width = m_cubeExtent.width;
            renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
            renderPassInfo.pClearValues = clearValues.data();


            vkCmdBeginRenderPass(cmd, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

            const glm::mat4 projMatrix = glm::perspective(static_cast<float>(M_PI / 2.f)
                                                          , 1.f, 0.1f, 1000.f);
            glm::mat4 viewMatrix = glm::mat4(1.0f);
            switch (k) {
                case 0: // POSITIVE_X
                    viewMatrix = glm::rotate(viewMatrix, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                    viewMatrix = glm::rotate(viewMatrix, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
                    break;
                case 1:    // NEGATIVE_X
                    viewMatrix = glm::rotate(viewMatrix, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                    viewMatrix = glm::rotate(viewMatrix, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
                    break;
                case 2:    // POSITIVE_Y
                    viewMatrix = glm::rotate(viewMatrix, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
                    break;
                case 3:    // NEGATIVE_Y
                    viewMatrix = glm::rotate(viewMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
                    break;
                case 4:    // POSITIVE_Z
                    viewMatrix = glm::rotate(viewMatrix, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
                    break;
                case 5:    // NEGATIVE_Z
                    viewMatrix = glm::rotate(viewMatrix, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
                    break;
            }

            lastMaterial = nullptr;
            const glm::vec3 cameraPos = m_cameraView.m_position;
            const glm::vec3 mirrorCameraLength = cameraPos - mirrorPosition;
            const PushConstants pushConst = {
                    .viewProjMatrix = projMatrix * viewMatrix,
                    .cameraPosition = cameraPos
                            /*.cameraPosition = cameraPos
                            + mirrorCameraLength * ((100.f / glm::length(mirrorCameraLength)) - 1.f)*/
            };
            std::cout << pushConst.cameraPosition.x << ' ' << pushConst.cameraPosition.y << ' ' << pushConst.cameraPosition.z << std::endl;
            std::cout << glm::length(pushConst.cameraPosition - mirrorPosition) << std::endl;
            for (size_t i = 1; i < objects.size(); ++i) {
                auto&& object = *objects.at(i);
                if (!object.m_isUploaded) {
                    std::cerr << "object with id " << i << " isn't uploaded!" << std::endl;
                    continue;
                }


                auto&& material = &m_renderMaterials[RenderMaterial::Type::DEFAULT];
                //only bind the pipeline if it doesnt match with the already bound one
                if (material != lastMaterial) {

                    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, material->pipeline);

                    VkViewport viewport = {};
                    viewport.x = 0.f;
                    viewport.y = 0.f;
                    viewport.width = m_cubeExtent.width;
                    viewport.height = m_cubeExtent.height;
                    viewport.minDepth = 0.f;
                    viewport.maxDepth = 1.f;

                    VkRect2D scissor = {};
                    scissor.offset = {0, 0};
                    scissor.extent.width = m_cubeExtent.width;
                    scissor.extent.height = m_cubeExtent.height;

                    vkCmdSetViewport(cmd, 0, 1, &viewport);
                    vkCmdSetScissor(cmd, 0, 1, &scissor);

                    vkCmdPushConstants(cmd, material->pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0
                                       , sizeof(PushConstants), &pushConst);

                    lastMaterial = material;

                    uint32_t dynamicOffsets[] = {
                            0
                    };
                    vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, material->pipelineLayout, 0, 1
                                            , &getCurFrame().globalDescriptor, 1, dynamicOffsets);
                }

                VkDeviceSize offset = 0;
                vkCmdBindVertexBuffers(cmd, 0, 1, &object.m_vertexBuffer._buffer, &offset);

                vkCmdDraw(cmd, object.vertices.size(), 1, 0, i);

            }


            vkCmdEndRenderPass(cmd);
        }

        VkImageSubresourceRange cubeFaceSubresourceRange = {};
        cubeFaceSubresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        cubeFaceSubresourceRange.baseMipLevel = 0;
        cubeFaceSubresourceRange.levelCount = 1;
        cubeFaceSubresourceRange.baseArrayLayer = 0;
        cubeFaceSubresourceRange.layerCount = 6;

        VkImageMemoryBarrier imageMemoryBarrier = {};
        imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageMemoryBarrier.image = m_cubeMap._image;
        imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imageMemoryBarrier.subresourceRange = cubeFaceSubresourceRange;

        vkCmdPipelineBarrier(
                cmd
                , VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
                , VK_PIPELINE_STAGE_VERTEX_SHADER_BIT
                , 0
                , 0, nullptr
                , 0, nullptr
                , 1, &imageMemoryBarrier
        );
        ///TODO===================================================
#endif


        std::array< VkClearValue, 2 > clearValues{};
        clearValues[0].color = {0.0f, 0.0f, 0.1f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0};

        VkRenderPassBeginInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = m_renderPass;
        renderPassInfo.framebuffer = m_frameBuffers[imageIndex];
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent.height = wHeight;
        renderPassInfo.renderArea.extent.width = wWidth;
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();


        vkCmdBeginRenderPass(cmd, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);


        const PushConstants pushConst = {
            .viewProjMatrix = m_cameraView.getProjectionMatrix() * m_cameraView.getViewMatrix(),
            .cameraPosition = m_cameraView.m_position
        };
        vkCmdPushConstants(cmd, lastMaterial->pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PushConstants)
                           , &pushConst);
        //RenderMaterial* lastMaterial = nullptr;
        for (size_t i = 0, mi = objects.size(); i < mi; ++i) {
            auto&& object = *objects.at(i);
            if (!object.m_isUploaded) {
                std::cerr << "object with id " << i << " isn't uploaded!" << std::endl;
                continue;
            }

            if(i == 0)
            {
                auto&& material = &m_renderMaterials[RenderMaterial::Type::REFLECTION];
                const glm::vec3 cameraPos = m_cameraView.m_position;
                const glm::vec3 mirrorCameraLength = cameraPos - mirrorPosition;
                const PushConstants pushConst1 = {
                        .viewProjMatrix = m_cameraView.getProjectionMatrix() * m_cameraView.getViewMatrix(),
                        //.cameraPosition = cameraPos
                        .cameraPosition = cameraPos
                        + mirrorCameraLength * ((10.f / glm::length(mirrorCameraLength)) - 1.f)
                };
                //only bind the pipeline if it doesnt match with the already bound one
                if (material != lastMaterial) {

                    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, material->pipeline);

                    VkViewport viewport = {};
                    viewport.x = 0.f;
                    viewport.y = 0.f;
                    viewport.width = static_cast<float>(wWidth);
                    viewport.height = static_cast<float>(wHeight);
                    viewport.minDepth = 0.f;
                    viewport.maxDepth = 1.f;

                    VkRect2D scissor = {};
                    scissor.offset = {0, 0};
                    scissor.extent.width = wWidth;
                    scissor.extent.height = wHeight;

                    vkCmdSetViewport(cmd, 0, 1, &viewport);
                    vkCmdSetScissor(cmd, 0, 1, &scissor);

                    vkCmdPushConstants(cmd, material->pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PushConstants)
                                       , &pushConst1);

                    lastMaterial = material;

                    vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, material->pipelineLayout, 0, 1
                                            , &m_reflectionDescriptorSet, 0, nullptr);
                }

            }
            else {
                auto&& material = &m_renderMaterials[RenderMaterial::Type::DEFAULT];
                //only bind the pipeline if it doesnt match with the already bound one
                if (material != lastMaterial) {

                    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, material->pipeline);

                    VkViewport viewport = {};
                    viewport.x = 0.f;
                    viewport.y = 0.f;
                    viewport.width = static_cast<float>(wWidth);
                    viewport.height = static_cast<float>(wHeight);
                    viewport.minDepth = 0.f;
                    viewport.maxDepth = 1.f;

                    VkRect2D scissor = {};
                    scissor.offset = {0, 0};
                    scissor.extent.width = wWidth;
                    scissor.extent.height = wHeight;

                    vkCmdSetViewport(cmd, 0, 1, &viewport);
                    vkCmdSetScissor(cmd, 0, 1, &scissor);

                    vkCmdPushConstants(cmd, material->pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0
                                       , sizeof(PushConstants)
                                       , &pushConst);

                    lastMaterial = material;

                    uint32_t dynamicOffsets[] = {
                            0
                    };
                    vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, material->pipelineLayout, 0, 1
                                            , &getCurFrame().globalDescriptor, 1, dynamicOffsets);
                }
            }

            VkDeviceSize offset = 0;
            vkCmdBindVertexBuffers(cmd, 0, 1, &object.m_vertexBuffer._buffer, &offset);

            vkCmdDraw(cmd, object.vertices.size(), 1, 0, i);
        }

        vkCmdEndRenderPass(cmd);

        vkEndCommandBuffer(cmd);


        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = {curFrame.renderSemaphore};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &cmd;

        VkSemaphore signalSemaphores[] = {curFrame.presentSemaphore};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;


        if (VK_SUCCESS != vkQueueSubmit(m_queue, 1, &submitInfo, curFrame.renderFence)) {
            throw std::runtime_error(("failed to submit draw command buffer!"));
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
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
            assert(0); //TODO resize window
        } else if (result != VK_SUCCESS) {
            throw std::runtime_error(("failed to present swap chain image!"));
        }

        m_currentFrame++;
    }


    void Engine::createRenderPass_()
    {
        //TODO 622 page in example
        VkAttachmentDescription colorAttachment = {};
        colorAttachment.format = m_core.getSurfaceFormat().format;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference attachRef = {};
        attachRef.attachment = 0;
        attachRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentDescription depthAttachment = {};
        depthAttachment.format = m_core.findSupportedFormat(
                {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT}
                , VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
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
        dependency.srcStageMask =
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask =
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        std::array< VkAttachmentDescription, 2 > attachments = {colorAttachment, depthAttachment};

        VkRenderPassCreateInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        renderPassInfo.pAttachments = attachments.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpassDesc;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        if (vkCreateRenderPass(m_core.getDevice(), &renderPassInfo, nullptr, &m_renderPass) != VK_SUCCESS) {
            throw std::runtime_error(("failed to create render pass!"));
        }
        m_deletionQueue.push([dev = m_core.getDevice(), rp = m_renderPass]()
                             {
                                 vkDestroyRenderPass(dev, rp, nullptr);
                             });
    }


    void Engine::createFramebuffer_()
    {
        int wHeight = m_rWindow.getHeight(), wWidth = m_rWindow.getWidth();

        m_frameBuffers.resize(m_images.size());

        for (uint32_t i = 0, mi = m_images.size(); i < mi; i++) {
            m_views[i] = m_core.createImageView(m_images[i], m_core.getSurfaceFormat().format
                                                , VK_IMAGE_VIEW_TYPE_2D
                                                , VK_IMAGE_ASPECT_COLOR_BIT);
            m_deletionQueue.push([dev = m_core.getDevice(), view = m_views[i]]()
                                 {
                                     vkDestroyImageView(dev, view, nullptr);
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
            fbInfo.layers = 1; //TODO to 6 from cube map

            if (VK_SUCCESS != vkCreateFramebuffer(m_core.getDevice(), &fbInfo, NULL, &m_frameBuffers[i])) {
                throw std::runtime_error(("failed to create frame buffer"));
            }
            m_deletionQueue.push([dev = m_core.getDevice(), fb = m_frameBuffers[i]]()
                                 {
                                     vkDestroyFramebuffer(dev, fb, nullptr);
                                 });
        }
    }


    VkShaderModule Engine::createShaderModule_(const std::string& source_)
    {
        VkShaderModuleCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = source_.size();
        createInfo.pCode = reinterpret_cast< const uint32_t* >(source_.data());

        VkShaderModule sModule = nullptr;
        if (vkCreateShaderModule(m_core.getDevice(), &createInfo, nullptr, &sModule) != VK_SUCCESS) {
            throw std::runtime_error(("failed to create shader module!"));
        }

        return sModule;
    }


    void Engine::createPipeline_()
    {
        std::ostringstream forShader;

        std::ifstream iFile(vert_shader_fname);
        forShader << iFile.rdbuf();
        VkShaderModule vertShader = createShaderModule_(forShader.str());
        forShader.flush();

        std::ostringstream forFrag;
        std::ifstream fragFile(frag_shader_fname);
        forFrag << fragFile.rdbuf();
        VkShaderModule fragShader = createShaderModule_(forFrag.str());


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
                //, m_objectSetLayout
        };


        VkPushConstantRange pushConstantInfo = {};
        pushConstantInfo.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantInfo.offset = 0;
        pushConstantInfo.size = sizeof(PushConstants);


        VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = setLayouts.size();
        pipelineLayoutInfo.pSetLayouts = setLayouts.data();
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantInfo;

        VkPipelineLayout pipelineLayout = {};
        if (vkCreatePipelineLayout(m_core.getDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error(("failed to create pipeline layout!"));
        }
        pipelineBuildInfo.pipelineLayout = pipelineLayout;
        m_deletionQueue.push([dev = m_core.getDevice(), pll = pipelineLayout]()
                             {
                                 vkDestroyPipelineLayout(dev, pll, nullptr);
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


        pipelineBuildInfo.rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        pipelineBuildInfo.rasterizer.depthClampEnable = VK_FALSE;
        pipelineBuildInfo.rasterizer.rasterizerDiscardEnable = VK_FALSE;
        pipelineBuildInfo.rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        pipelineBuildInfo.rasterizer.lineWidth = 1.f;
        //todo here you can change cull mode
        pipelineBuildInfo.rasterizer.cullMode = VK_CULL_MODE_NONE;
        pipelineBuildInfo.rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
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
                                                                | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
                                                                VK_COLOR_COMPONENT_A_BIT;
        pipelineBuildInfo.colorBlendAttachment.blendEnable = VK_FALSE;
        pipelineBuildInfo.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
        pipelineBuildInfo.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
        pipelineBuildInfo.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        pipelineBuildInfo.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        pipelineBuildInfo.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        pipelineBuildInfo.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;


        std::vector< VkDynamicState > dynamicStates = {
                VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR
        };
        pipelineBuildInfo.dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        pipelineBuildInfo.dynamicState.dynamicStateCount = dynamicStates.size();
        pipelineBuildInfo.dynamicState.pDynamicStates = dynamicStates.data();


        VkPipeline meshPipeline = pipelineBuildInfo.build_pipeline(m_core.getDevice(), m_renderPass);


        m_renderMaterials[RenderMaterial::Type::DEFAULT] = {meshPipeline, pipelineLayout};
//TODO
        m_deletionQueue.push([dev = m_core.getDevice(), mpl = meshPipeline]()
                             {
                                 vkDestroyPipeline(dev, mpl, nullptr);
                             });

        vkDestroyShaderModule(m_core.getDevice(), fragShader, nullptr);
        vkDestroyShaderModule(m_core.getDevice(), vertShader, nullptr);
    }


    void Engine::createSyncObjects_()
    {
        VkFenceCreateInfo fenceInfo = {};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        VkSemaphoreCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        auto&& device = m_core.getDevice();

        for (auto&& frame : m_frames) {
            vkCreateFence(device, &fenceInfo, nullptr, &frame.renderFence);

            //enqueue the destruction of the fence
            m_deletionQueue.push([device, frf = frame.renderFence]()
                                 {
                                     vkDestroyFence(device, frf, nullptr);
                                 });


            vkCreateSemaphore(device, &createInfo, nullptr, &frame.presentSemaphore);
            vkCreateSemaphore(device, &createInfo, nullptr, &frame.renderSemaphore);

            m_deletionQueue.push([device, fps = frame.presentSemaphore, frs = frame.renderSemaphore]()
                                 {
                                     vkDestroySemaphore(device, fps, nullptr);
                                     vkDestroySemaphore(device, frs, nullptr);
                                 });
        }

    }


    Engine::AllocatedBuffer
    Engine::create_buffer_(size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage)
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
        vmaCreateBuffer(m_allocator, &bufferInfo, &vmaallocInfo, &newBuffer._buffer, &newBuffer._allocation, nullptr);

        return newBuffer;
    }


    Engine::AllocatedImage
    Engine::create_image_(VkImageType type
                          , VkFormat format
                          , VkImageCreateFlags flags
                          , VkSampleCountFlagBits samples
                          , VkImageTiling tiling
                          , VkImageUsageFlags usage
                          , VmaMemoryUsage memoryUsage
                          , VkExtent3D extent
                          , uint32_t layers
                          , uint32_t mipLevels)
    {
        VkImageCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        info.pNext = nullptr;
        info.imageType = type;
        info.format = format;
        info.extent = extent;
        info.mipLevels = mipLevels;
        info.arrayLayers = layers;
        info.samples = samples;
        info.tiling = tiling;
        info.usage = usage;
        info.flags = flags;


        VmaAllocationCreateInfo allocInfo = {};
        allocInfo.usage = memoryUsage;


        AllocatedImage new_image = {};
        vmaCreateImage(m_allocator, &info, &allocInfo, &new_image._image, &new_image._allocation
                       , nullptr);

        return new_image;
    }


    void Engine::prepareCubeFrameBuffers_t()
    {
        m_cubeMap = create_image_(VK_IMAGE_TYPE_2D
                                  , m_cubeMapFormat
                                  , VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT
                                  , VK_SAMPLE_COUNT_1_BIT
                                  , VK_IMAGE_TILING_OPTIMAL
                                  , VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
                                  , VMA_MEMORY_USAGE_GPU_ONLY
                                  , m_cubeExtent
                                  , 6, 1);

        m_deletionQueue.push([dev = m_core.getDevice(), dIm = m_cubeMap, alloc = m_allocator]()
                             {
                                 vmaDestroyImage(alloc, dIm._image, dIm._allocation);
                             });


        m_cubeImageView = m_core.createImageView(m_cubeMap._image
                                                 , m_cubeMapFormat
                                                 , VK_IMAGE_VIEW_TYPE_CUBE
                                                 , VK_IMAGE_ASPECT_COLOR_BIT
                                                 , { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A }
                                                 , 0, 6);


        VkSamplerCreateInfo samplerCreateInfo {};
        samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerCreateInfo.maxAnisotropy = 1.0f;
        samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
        samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
        samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        samplerCreateInfo.addressModeV = samplerCreateInfo.addressModeU;
        samplerCreateInfo.addressModeW = samplerCreateInfo.addressModeU;
        samplerCreateInfo.mipLodBias = 0.0f;
        samplerCreateInfo.compareOp = VK_COMPARE_OP_NEVER;
        samplerCreateInfo.minLod = 0.0f;
        samplerCreateInfo.maxLod = 0.f;
        samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
/*        if (vulkanDevice->features.samplerAnisotropy)
        {
            samplerCreateInfo.maxAnisotropy = vulkanDevice->properties.limits.maxSamplerAnisotropy;
            samplerCreateInfo.anisotropyEnable = VK_TRUE;
        }*/
        vkCreateSampler(m_core.getDevice(), &samplerCreateInfo, nullptr, &m_cubeSampler);



        VkFormat depthFormat = m_core.findSupportedFormat(
                {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT}
                , VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
        );

        m_cubeDepthImage = create_image_(VK_IMAGE_TYPE_2D
                                         , depthFormat
                                         , VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT
                                         , VK_SAMPLE_COUNT_1_BIT
                                         , VK_IMAGE_TILING_OPTIMAL
                                         , VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT
                                         , VMA_MEMORY_USAGE_GPU_ONLY
                                         , m_cubeExtent
                                         , 6, 1);

        m_deletionQueue.push([dev = m_core.getDevice(), dIm = m_cubeDepthImage, alloc = m_allocator]()
                             {
                                 vmaDestroyImage(alloc, dIm._image, dIm._allocation);
                             });


        for (size_t i = 0, mi = m_cubeFrameBuffers.size(); i < mi; ++i) {
            auto&& curFrameBuff = m_cubeFrameBuffers.at(i);

            curFrameBuff.imageView = m_core.createImageView(m_cubeMap._image
                                                            , m_cubeMapFormat
                                                            , VK_IMAGE_VIEW_TYPE_2D
                                                            , VK_IMAGE_ASPECT_COLOR_BIT
                                                            , { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A }
                                                            , i, 1);
            m_deletionQueue.push([dev = m_core.getDevice(), dImV = curFrameBuff.imageView]()
                                 {
                                     vkDestroyImageView(dev, dImV, nullptr);
                                 });

            curFrameBuff.depthImageView = m_core.createImageView(m_cubeDepthImage._image
                                                                 , depthFormat
                                                                 , VK_IMAGE_VIEW_TYPE_2D
                                                                 , VK_IMAGE_ASPECT_DEPTH_BIT
                                                                 , {VK_COMPONENT_SWIZZLE_R}
                                                                 , i, 1);
            m_deletionQueue.push([dev = m_core.getDevice(), dImV = curFrameBuff.depthImageView]()
                                 {
                                     vkDestroyImageView(dev, dImV, nullptr);
                                 });


            std::array< VkImageView, 2 > attachments = {
                    curFrameBuff.imageView, curFrameBuff.depthImageView
            };

            VkFramebufferCreateInfo fbInfo = {};
            fbInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            fbInfo.renderPass = m_renderPass;
            fbInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
            fbInfo.pAttachments = attachments.data();
            fbInfo.width = m_cubeExtent.width;
            fbInfo.height = m_cubeExtent.height;
            fbInfo.layers = 1;

            if (VK_SUCCESS != vkCreateFramebuffer(m_core.getDevice(), &fbInfo, NULL, &curFrameBuff.frameBuffer)) {
                throw std::runtime_error(("failed to create frame buffer"));
            }
            m_deletionQueue.push([dev = m_core.getDevice(), fb = curFrameBuff.frameBuffer]()
                                 {
                                     vkDestroyFramebuffer(dev, fb, nullptr);
                                 });
        }

    }

    void Engine::createReflectionDescriptors_t()
    {
        //TODO
        VkDescriptorSetLayoutBinding modelLayoutBinding = {};
        modelLayoutBinding.binding = 0;
        modelLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        modelLayoutBinding.descriptorCount = 1;
        modelLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        modelLayoutBinding.pImmutableSamplers = nullptr;

        VkDescriptorSetLayoutBinding cubeLayoutBinding = {};
        cubeLayoutBinding.binding = 1;
        cubeLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        cubeLayoutBinding.descriptorCount = 1;
        cubeLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        cubeLayoutBinding.pImmutableSamplers = nullptr;

        VkDescriptorSetLayoutBinding bindings[]{
                modelLayoutBinding,
                cubeLayoutBinding
        };

        VkDescriptorSetLayoutCreateInfo layoutInfo = {};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = 2;
        layoutInfo.pBindings = bindings;

        if (vkCreateDescriptorSetLayout(m_core.getDevice(), &layoutInfo, nullptr, &m_reflectionDescriptorSetLayout) !=
            VK_SUCCESS) {
            throw std::runtime_error(("failed to create descriptor set layput!"));
        }
        auto&& device = m_core.getDevice();
        m_deletionQueue.push([device, gsl = m_reflectionDescriptorSetLayout]()
                             {
                                 vkDestroyDescriptorSetLayout(device, gsl, nullptr);
                             });

        ///todo

        VkDescriptorSetAllocateInfo allocInfo = {};
        allocInfo.pNext = nullptr;
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = m_descriptorPool;
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &m_reflectionDescriptorSetLayout;

        vkAllocateDescriptorSets(device, &allocInfo, &m_reflectionDescriptorSet);


        VkDescriptorBufferInfo objectBufferInfo;
        objectBufferInfo.buffer = m_frames[0].objectBuffer._buffer;
        objectBufferInfo.offset = 0;
        objectBufferInfo.range = sizeof(GPUObjectData) * m_numObjects;


        VkDescriptorImageInfo cubeBufferInfo = {};
        cubeBufferInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        cubeBufferInfo.imageView = m_cubeImageView;
        cubeBufferInfo.sampler = m_cubeSampler;

        ////

        VkWriteDescriptorSet descriptorWriteModel1 = {};
        descriptorWriteModel1.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWriteModel1.dstSet = m_reflectionDescriptorSet;
        descriptorWriteModel1.dstBinding = 0;
        descriptorWriteModel1.dstArrayElement = 0;
        descriptorWriteModel1.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        descriptorWriteModel1.descriptorCount = 1;
        descriptorWriteModel1.pBufferInfo = &objectBufferInfo;
        descriptorWriteModel1.pImageInfo = nullptr;
        descriptorWriteModel1.pTexelBufferView = nullptr;

        ////

        VkWriteDescriptorSet descriptorWriteCubeMap = {};
        descriptorWriteCubeMap.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWriteCubeMap.dstSet = m_reflectionDescriptorSet;
        descriptorWriteCubeMap.dstBinding = 1;
        descriptorWriteCubeMap.dstArrayElement = 0;
        descriptorWriteCubeMap.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWriteCubeMap.descriptorCount = 1;
        descriptorWriteCubeMap.pBufferInfo = nullptr;
        descriptorWriteCubeMap.pImageInfo = &cubeBufferInfo;
        descriptorWriteCubeMap.pTexelBufferView = nullptr;


        VkWriteDescriptorSet descriptorWrites[] = {
                descriptorWriteModel1
                , descriptorWriteCubeMap
                //descriptorWriteColors
        };

        vkUpdateDescriptorSets(device, 2, descriptorWrites, 0, nullptr);
    }

    void Engine::createReflectionPipeLine_t()
    {
        std::ostringstream forShader;

        std::ifstream iFile(m_reflectionVertShaderPathName);
        forShader << iFile.rdbuf();
        VkShaderModule vertShader = createShaderModule_(forShader.str());
        forShader.flush();

        std::ostringstream forFrag;
        std::ifstream fragFile(m_reflectionFragShaderPathName);
        forFrag << fragFile.rdbuf();
        VkShaderModule fragShader = createShaderModule_(forFrag.str());


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
                m_reflectionDescriptorSetLayout //TODO!!!
                //, m_objectSetLayout
        };


        VkPushConstantRange pushConstantInfo = {};
        pushConstantInfo.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantInfo.offset = 0;
        pushConstantInfo.size = sizeof(PushConstants);


        VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = setLayouts.size();
        pipelineLayoutInfo.pSetLayouts = setLayouts.data();
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantInfo;

        VkPipelineLayout pipelineLayout = {};
        if (vkCreatePipelineLayout(m_core.getDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error(("failed to create pipeline layout!"));
        }
        pipelineBuildInfo.pipelineLayout = pipelineLayout;
        m_deletionQueue.push([dev = m_core.getDevice(), pll = pipelineLayout]()
                             {
                                 vkDestroyPipelineLayout(dev, pll, nullptr);
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


        pipelineBuildInfo.rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        pipelineBuildInfo.rasterizer.depthClampEnable = VK_FALSE;
        pipelineBuildInfo.rasterizer.rasterizerDiscardEnable = VK_FALSE;
        pipelineBuildInfo.rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        pipelineBuildInfo.rasterizer.lineWidth = 1.f;
        //todo here you can change cull mode
        pipelineBuildInfo.rasterizer.cullMode = VK_CULL_MODE_NONE;
        pipelineBuildInfo.rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
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
                                                                | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
                                                                VK_COLOR_COMPONENT_A_BIT;
        pipelineBuildInfo.colorBlendAttachment.blendEnable = VK_FALSE;
        pipelineBuildInfo.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
        pipelineBuildInfo.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
        pipelineBuildInfo.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        pipelineBuildInfo.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        pipelineBuildInfo.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        pipelineBuildInfo.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;


        std::vector< VkDynamicState > dynamicStates = {
                VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR
        };
        pipelineBuildInfo.dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        pipelineBuildInfo.dynamicState.dynamicStateCount = dynamicStates.size();
        pipelineBuildInfo.dynamicState.pDynamicStates = dynamicStates.data();


        VkPipeline reflectionPipeline = pipelineBuildInfo.build_pipeline(m_core.getDevice(), m_renderPass);


        m_renderMaterials[RenderMaterial::Type::REFLECTION] = {reflectionPipeline, pipelineLayout};

        m_deletionQueue.push([dev = m_core.getDevice(), rpl = reflectionPipeline]()
                             {
                                 vkDestroyPipeline(dev, rpl, nullptr);
                             });

        vkDestroyShaderModule(m_core.getDevice(), fragShader, nullptr);
        vkDestroyShaderModule(m_core.getDevice(), vertShader, nullptr);
    }

}//namespace vks
