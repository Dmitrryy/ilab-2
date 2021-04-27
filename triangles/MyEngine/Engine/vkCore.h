/*************************************************************************
 *
 * vkCore.h
 *
 * Created by dmitry
 * 22.02.2021
 *
 ***/

#pragma once

#include "DeleteonQueue.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFWWrap/Window.hpp>

#include <string>
#include <vector>
#include <iostream>


#ifndef NDEBUG

#define ENABLE_DEBUG_LAYERS
#define PRINTF_DEVICE_INFO

#endif

namespace ezg
{

    struct VulkanPhysicalDevices
    {
        void resize(size_t size_)
        {
            m_devices.resize(size_);
            m_devProps.resize(size_);
            m_qFamilyProps.resize(size_);
            m_qSupportPresent.resize(size_);
            m_surfaceFormats.resize(size_);
            m_surfaceCaps.resize(size_);
        }

        void update(const VkSurfaceKHR& surface_);

        std::vector<VkPhysicalDevice>                         m_devices;
        std::vector<VkPhysicalDeviceProperties>               m_devProps;
        std::vector< std::vector< VkQueueFamilyProperties > > m_qFamilyProps;
        std::vector< std::vector< VkBool32 > >                m_qSupportPresent;
        std::vector< std::vector< VkSurfaceFormatKHR > >      m_surfaceFormats;
        std::vector< VkSurfaceCapabilitiesKHR >               m_surfaceCaps;
    };//struct VulkanPhysicalDevices


    VulkanPhysicalDevices VulkanGetPhysicalDevices(
            const VkInstance &inst_,
            const VkSurfaceKHR &surface_
            );


    std::vector<VkExtensionProperties> VulkanEnumExtProps();
    std::vector<const char*> getRequiredExtensions();


    class Core
    {
        VkInstance            m_inst    = nullptr;
        VkDevice              m_device  = nullptr;
        VkSurfaceKHR          m_surface = nullptr;
        VulkanPhysicalDevices m_physDevices;

        size_t      m_DeviceIndex = -1;
        size_t      m_QueueFamily = -1;

        DeletionQueue_t m_deletionQueue;

#ifdef ENABLE_DEBUG_LAYERS
        const bool enableValidationLayers = true;

        bool checkValidationLayerSupport() const;

        const std::vector<const char*> m_validationLayers = {
                "VK_LAYER_KHRONOS_validation"
        };

        static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
                VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
                VkDebugUtilsMessageTypeFlagsEXT             messageType,
                const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                void*                                       pUserData
        );
#else
        const bool enableValidationLayers = false;
#endif //ENABLE_DEBUG_LAYERS

        const std::vector<const char*> m_deviceExtensions = {
                VK_KHR_SWAPCHAIN_EXTENSION_NAME,
                VK_KHR_SHADER_DRAW_PARAMETERS_EXTENSION_NAME//,
                //VK_EXT_SHADER_ATOMIC_FLOAT_EXTENSION_NAME
        };

    public:

        Core(const Window& window);

    public:

        void updataPhysDivicesProp() { m_physDevices.update(m_surface); }
        size_t      getQueueFamily() const { return m_QueueFamily; }
        VkInstance& getInstance() { return m_inst; }
        VkDevice&   getDevice() { return m_device; }
        const VkPhysicalDevice& getPhysDevice() const {
            return m_physDevices.m_devices[m_DeviceIndex];
        }

        VkSurfaceKHR& getSurface() { return m_surface; }
        const VkSurfaceCapabilitiesKHR& getSurfaceCaps() const {
            return m_physDevices.m_surfaceCaps[m_DeviceIndex];
        }
        const VkSurfaceFormatKHR& getSurfaceFormat() const {
            return m_physDevices.m_surfaceFormats[m_DeviceIndex][0];
        }

        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

        VkImageView createImageView(VkImage image
                                    , VkFormat format
                                    , VkImageViewType type
                                    , VkImageAspectFlags aspectFlags
                                    , VkComponentMapping components = {}
                                    , uint32_t baseArrayLayer = 0
                                    , uint32_t layers = 1
                                    , uint32_t baseMipLevel = 0
                                    , uint32_t levels = 1);


        VkSampleCountFlagBits getMaxUsableSampleCount() {
            VkPhysicalDeviceProperties physicalDeviceProperties;
            vkGetPhysicalDeviceProperties(m_physDevices.m_devices.at(m_DeviceIndex), &physicalDeviceProperties);

            VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;
            if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
            if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
            if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
            if (counts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
            if (counts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
            if (counts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }

            return VK_SAMPLE_COUNT_1_BIT;
        }


        VkFormat findSupportedFormat(const std::vector< VkFormat >& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

    private:

        void createInstance_();
        void createSurface_();
        void selectPhysicalDevice_();
        void createLogicalDevice();


    };//class Core

}//namespace vks