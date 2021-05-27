/*************************************************************************************************
 *
 *   Types.hpp
 *
 *   Created by dmitry
 *   28.04.2021
 *
 ***/

#pragma once


#include <vk_mem_alloc.h>


namespace ezg::engine
{

    class Engine;


    struct PushConstants
    {
        glm::mat4 viewProjMatrix = glm::mat4(1.f);
        glm::vec3 cameraPosition = {};
    };


    struct GPULightInfo {
        glm::vec3 position = {};
        glm::vec3 color = { 1.f, 1.f, 1.f };
        float farPlane = 0;
        float epsilon = 0.05;
        float shadowOpacity = 0.5;
    };


    struct GPUSceneInfo {
        float lightAmbient = 0;
        size_t numLights = 0;
    };


    struct Vertex
    {
        glm::vec3 pos;
        glm::vec3 color = {1.f, 1.f, 1.f};
        glm::vec3 normal;


        static VkVertexInputBindingDescription getBindingDescription()
        {
            VkVertexInputBindingDescription bindingDescription = {};
            bindingDescription.binding = 0;
            bindingDescription.stride = sizeof(Vertex);
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

            return bindingDescription;
        }

        static std::array< VkVertexInputAttributeDescription, 3 > getAttributeDescription()
        {
            std::array< VkVertexInputAttributeDescription, 3 > attributeDescriptions{};

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
    };//class Vertex



    struct frameBufferData
    {
        VkImageView imageView;
        VkImageView depthImageView;
        VkFramebuffer frameBuffer;
        VkExtent2D extent;
    };


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


    struct RenderMaterial
    {
        VkPipeline pipeline;
        VkPipelineLayout pipelineLayout;

        enum class Type
        {
            DEFAULT
            , DEFAULT_DEPTH_TEST_ONLY
            , REFLECTION
        };

        bool equal(const RenderMaterial& that) const noexcept { return pipeline == that.pipeline && pipelineLayout == that.pipelineLayout; }
    };

}//namespace ezg