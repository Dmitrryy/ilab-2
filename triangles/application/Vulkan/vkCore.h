//
// Created by dmitry on 10.11.2020.
//

#pragma once

#include <vulkan/vulkan.h>

#include <string>
#include <vector>
#include <iostream>

#include <WindowControl.h>

#define ENABLE_DEBUG_LAYERS

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

#ifdef ENABLE_DEBUG_LAYERS
    constexpr bool enableValidationLayers = true;
#else
    constexpr bool enableValidationLayers = false;
#endif

    std::vector<VkExtensionProperties> VulkanEnumExtProps();
    std::vector<const char*> getRequiredExtensions();


    class VulkanWindowControl;

    class Core
    {
        VkInstance            m_inst = {};
        VkDevice              m_device = {};
        VkSurfaceKHR          m_surface = {};
        VulkanPhysicalDevices m_physDevices = {};

        std::string m_appName = {};
        size_t      m_DeviceIndex = -1;
        size_t      m_QueueFamily = -1;

/*        std::vector< const char* > m_extensions = {
                //VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
                VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
                VK_KHR_SURFACE_EXTENSION_NAME
        };*/

        const std::vector<const char*> m_deviceExtensions = {
                VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };

#ifdef ENABLE_DEBUG_LAYERS

        const std::vector<const char*> m_validationLayers = {
                "VK_LAYER_KHRONOS_validation"
        };


        static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
                VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                VkDebugUtilsMessageTypeFlagsEXT messageType,
                const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                void* pUserData);

#endif //ENABLE_DEBUG_LAYERS

    public:

        Core(const std::string& appName_)
            : m_appName(appName_)
        {}
        ~Core() = default;

        bool Init(WindowControl* pWindowControl);

        size_t getQueueFamily() const { return m_QueueFamily; }
        VkInstance& getInstance() { return m_inst; }
        VkDevice& getDevice() { return m_device; }


    private:

        void createInstance_();
        void createSurface_();
        void selectPhysicalDevice_();
        void createLogicalDevice();

        bool checkValidationLayerSupport() const;
    };

}//namespace vks