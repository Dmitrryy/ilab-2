/*************************************************************************************************
 *
 *   PipelineBuilder.hpp
 *
 *   Created by dmitry
 *   30.03.2021
 *
 ***/

#pragma once

#include <vector>
#include <vulkan/vulkan.h>


namespace ezg
{

    struct PipelineBuildInfo
    {
        std::vector< VkPipelineShaderStageCreateInfo > shaderStages;
        VkPipelineVertexInputStateCreateInfo           vertexInputInfo      = {};
        VkPipelineInputAssemblyStateCreateInfo         inputAssembly        = {};
        VkViewport                                     viewport             = {};
        VkRect2D                                       scissor              = {};
        VkPipelineRasterizationStateCreateInfo         rasterizer           = {};
        VkPipelineColorBlendAttachmentState            colorBlendAttachment = {};
        VkPipelineMultisampleStateCreateInfo           multisampling        = {};
        VkPipelineLayout                               pipelineLayout       = {};
        VkPipelineDepthStencilStateCreateInfo          depthStencil         = {};

        VkPipeline build_pipeline(VkDevice device, VkRenderPass pass);
    };


}//namespace ezg