/*************************************************************************************************
 *
 *   Mirror.cpp
 *
 *   Created by dmitry
 *   25.04.2021
 *
 ***/


#include "triangles/MyEngine/Engine/Engine.hpp"


namespace ezg::engine
{

    void Mirror::draw(VkCommandBuffer cmdBuff
                              , const CameraView& camera
                              , RenderMaterial& last) const
    {
        if (!isUploaded()) {
            std::cerr << "object with id " << m_curInstanceId << " isn't uploaded!" << std::endl;
            return;
        }

        const glm::vec3 mirrorCameraLength = camera.getPosition() - getPosition();
        const glm::vec3 pushConst1 = camera.getPosition()
                                  + mirrorCameraLength * ((m_distanceToEye / glm::length(mirrorCameraLength)) - 1.f);

        //only bind the pipeline if it doesnt match with the already bound one
        if (!m_renderMaterial.equal(last)) {

            vkCmdBindPipeline(cmdBuff, VK_PIPELINE_BIND_POINT_GRAPHICS, m_renderMaterial.pipeline);
            last = m_renderMaterial;
        }

        vkCmdPushConstants(cmdBuff
                           , m_renderMaterial.pipelineLayout
                           , VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT
                           , offsetof(PushConstants, cameraPosition), sizeof(PushConstants::cameraPosition)
                           , &pushConst1);

        vkCmdBindDescriptorSets(cmdBuff
                                , VK_PIPELINE_BIND_POINT_GRAPHICS
                                , m_renderMaterial.pipelineLayout, 2, 1
                                , &m_descriptorSet, 0, nullptr);

        VkDeviceSize offset = 0;
        vkCmdBindVertexBuffers(cmdBuff, 0, 1, &m_vertexBuffer._buffer, &offset);

        vkCmdDraw(cmdBuff, vertices.size(), 1, 0, m_curInstanceId);
    }


    void Mirror::updateEnvironmentMap(VkCommandBuffer cmdBuff
                                              , const std::vector< Renderable* >& objects)
    {
        std::array< VkClearValue, 2 > clearValues{};
        clearValues[0].color = {{0.2f, 0.2f, 0.2f, 0.01f}};
        clearValues[1].depthStencil = {1.0f, 0};

        VkRenderPassBeginInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = m_environmentRenderPass;
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent.height = m_extent.height;
        renderPassInfo.renderArea.extent.width = m_extent.width;
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        VkViewport viewport = {};
        viewport.x = 0.f;
        viewport.y = 0.f;
        viewport.width = m_extent.width;
        viewport.height = m_extent.height;
        viewport.minDepth = 0.f;
        viewport.maxDepth = 1.f;

        VkRect2D scissor = {};
        scissor.offset = {0, 0};
        scissor.extent = m_extent;

        vkCmdSetViewport(cmdBuff, 0, 1, &viewport);
        vkCmdSetScissor(cmdBuff, 0, 1, &scissor);

        RenderMaterial lastMaterial = {};

        CameraView cameraView = {};
        cameraView.setViewingAngle(static_cast<float>(M_PI / 2.f));
        cameraView.setMaxZcomponentDirection(1.f);
        cameraView.m_near = 0.1f;
        cameraView.m_far = 1000.f;
        cameraView.setPosition(getPosition());

        glm::mat4 projMatrix = cameraView.getProjectionMatrix();
        projMatrix[1][1] *= -1; //todo

        for (size_t k = 0, mk = m_cubeFrameBuffers.size(); k < mk; ++k) {

            renderPassInfo.framebuffer = m_cubeFrameBuffers[k].frameBuffer;
            vkCmdBeginRenderPass(cmdBuff, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);


            switch (k) {
                case 0: // POSITIVE_X
                    cameraView.setDirection(glm::vec3(1.f, 0.f, 0.f));
                    cameraView.setTopDirection(glm::vec3(0.f, -1.f, 0.f));
                    break;
                case 1:    // NEGATIVE_X
                    cameraView.setDirection(glm::vec3(-1.f, 0.f, 0.f));
                    cameraView.setTopDirection(glm::vec3(0.f, -1.f, 0.f));
                    break;
                case 2:    // POSITIVE_Y
                    cameraView.setDirection(glm::vec3(0.f, 1.f, 0.f));
                    cameraView.setTopDirection(glm::vec3(0.f, 0.f, 1.f));
                    break;
                case 3:    // NEGATIVE_Y
                    cameraView.setDirection(glm::vec3(0.f, -1.f, 0.f));
                    cameraView.setTopDirection(glm::vec3(0.f, 0.f, -1.f));
                    break;
                case 4:    // POSITIVE_Z
                    cameraView.setDirection(glm::vec3(0.f, 0.f, 1.f));
                    cameraView.setTopDirection(glm::vec3(0.f, -1.f, 0.f));
                    break;
                case 5:    // NEGATIVE_Z
                    cameraView.setDirection(glm::vec3(0.f, 0.f, -1.f));
                    cameraView.setTopDirection(glm::vec3(0.f, -1.f, 0.f));
                    break;
            }

            const PushConstants pushConst = {
                    .viewProjMatrix = projMatrix * cameraView.getViewMatrix(),
                    .cameraPosition = getPosition()
            };
            vkCmdPushConstants(cmdBuff
                               , m_renderMaterial.pipelineLayout
                               , VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT
                               , 0, sizeof(PushConstants)
                               , &pushConst);

            std::for_each(objects.begin(), objects.end(), [&](auto* obj) {
                if(obj != nullptr && obj != this) {
                    obj->draw(cmdBuff, cameraView, lastMaterial);
                }
            });


            vkCmdEndRenderPass(cmdBuff);
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
        imageMemoryBarrier.image = m_environmentCubeMap._image;
        imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imageMemoryBarrier.subresourceRange = cubeFaceSubresourceRange;

        vkCmdPipelineBarrier(
                cmdBuff
                , VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
                , VK_PIPELINE_STAGE_VERTEX_SHADER_BIT
                , 0
                , 0, nullptr
                , 0, nullptr
                , 1, &imageMemoryBarrier
        );
    }

}//namespace ezg