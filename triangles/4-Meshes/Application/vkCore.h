/*************************************************************************
 *
 * vkCore.h
 *
 * Created by dmitry
 * 22.02.2021
 *
 ***/

#pragma once

#include <vulkan/vulkan.h>

#include <string>
#include <vector>
#include <iostream>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#ifndef NDEBUG

#define ENABLE_DEBUG_LAYERS
#define PRINTF_DEVICE_INFO

#endif

namespace vks
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
    };

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

        std::string m_appName;
        size_t      m_DeviceIndex = -1;
        size_t      m_QueueFamily = -1;

#ifdef ENABLE_DEBUG_LAYERS
        const bool enableValidationLayers = true;
#else
        const bool enableValidationLayers = false;
#endif

        const std::vector<const char*> m_deviceExtensions = {
                VK_KHR_SWAPCHAIN_EXTENSION_NAME,
                VK_KHR_SHADER_DRAW_PARAMETERS_EXTENSION_NAME//,
                //VK_EXT_SHADER_ATOMIC_FLOAT_EXTENSION_NAME
        };

#ifdef ENABLE_DEBUG_LAYERS

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

#endif //ENABLE_DEBUG_LAYERS

    public:

        Core(const std::string& appName_)
            : m_appName(appName_)
        {}
        ~Core()
        {
            cleanup();
        }

        bool Init(GLFWwindow* pWindowControl);

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

        void createBuffer(VkDeviceSize size,
            VkBufferUsageFlags usage,
            VkMemoryPropertyFlags properties,
            VkBuffer& buffer,
            VkDeviceMemory& bufferMemory);

        void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
        VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);

        VkFormat findSupportedFormat(const std::vector< VkFormat >& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

        void cleanup() 
        {
            if (m_inst != nullptr) 
            {
                vkDestroyDevice(m_device, nullptr);
                m_device = nullptr;

                vkDestroySurfaceKHR(m_inst, m_surface, nullptr);
                m_surface = nullptr;

                vkDestroyInstance(m_inst, nullptr);
                m_inst = nullptr;

                m_DeviceIndex = m_QueueFamily = -1;
                m_physDevices.resize(0);
            }
        }

    private:

        void createInstance_();
        void createSurface_();
        void selectPhysicalDevice_();
        void createLogicalDevice();


    };

}//namespace vks