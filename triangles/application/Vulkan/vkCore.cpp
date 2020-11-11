//
// Created by dmitry on 10.11.2020.
//

#include "vkCore.h"

#include <cassert>
#include <cstring>

#define PRINTF_DEVICE_INFO

namespace vks {

    bool Core::Init(WindowControl* pWindowControl)
    {
        std::vector<VkExtensionProperties> ExtProps = VulkanEnumExtProps();

        createInstance_();

        m_surface = pWindowControl->createSurface(m_inst);
        assert(m_surface);

        m_physDevices = VulkanGetPhysicalDevices(m_inst, m_surface);
        selectPhysicalDevice_();
        createLogicalDevice();

        return true;
    }

    void Core::createInstance_()
    {
        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = m_appName.c_str();
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo instInfo = {};
        instInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instInfo.pApplicationInfo = &appInfo;

        auto extensions = getRequiredExtensions();
        instInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        instInfo.ppEnabledExtensionNames = extensions.data();

#ifdef ENABLE_DEBUG_LAYERS

        if (!checkValidationLayerSupport()) {
            throw std::runtime_error("validation layers requested, but not available!");
        }

        instInfo.enabledLayerCount = static_cast<uint32_t>(m_validationLayers.size());
        instInfo.ppEnabledLayerNames = m_validationLayers.data();

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
        debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debugCreateInfo.pfnUserCallback = debugCallback;

        instInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;

#endif

        if (vkCreateInstance(&instInfo, nullptr, &m_inst) != VK_SUCCESS) {
            throw std::runtime_error("failed to create instance!");
        }
    }


    bool Core::checkValidationLayerSupport() const
    {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for (const char* layerName : m_validationLayers) {
            bool layerFound = false;

            for (const auto& layerProperties : availableLayers) {
                if (strcmp(layerName, layerProperties.layerName) == 0) {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound) {
                return false;
            }
        }

        return true;
    }

    void Core::selectPhysicalDevice_()
    {
        for (size_t i = 0, mi = m_physDevices.m_devices.size(); i < mi; i++)
        {
            for (size_t k = 0, km = m_physDevices.m_qFamilyProps[i].size(); k < km; k++)
            {
                VkQueueFamilyProperties& QFamilyProp = m_physDevices.m_qFamilyProps[i][k];

                VkQueueFlags flags = QFamilyProp.queueFlags;
#ifdef PRINTF_DEVICE_INFO
                printf("    GFX %s, Compute %s, Transfer %s, Sparse binding %s\n",
                       (flags & VK_QUEUE_GRAPHICS_BIT) ? "Yes" : "No",
                       (flags & VK_QUEUE_COMPUTE_BIT) ? "Yes" : "No",
                       (flags & VK_QUEUE_TRANSFER_BIT) ? "Yes" : "No",
                       (flags & VK_QUEUE_SPARSE_BINDING_BIT) ? "Yes" : "No");
#endif

                if (flags & VK_QUEUE_GRAPHICS_BIT) {
                    if (!m_physDevices.m_qSupportPresent[i][k]) {
#ifdef PRINTF_DEVICE_INFO
                        printf("Present is not supported\n");
#endif
                        continue;
                    }

                    m_DeviceIndex = i;
                    m_QueueFamily = k;
#ifdef PRINTF_DEVICE_INFO
                    printf("Using GFX device %d and queue family %d\n", m_DeviceIndex, m_QueueFamily);
#endif
                    break;
                }

            }
        }

        if (m_DeviceIndex < 0) {
            std::cerr << "No GFX device found!\n";
            assert(0);
        }
    }


    void Core::createLogicalDevice()
    {
        float qPriorities = 1.f;
        VkDeviceQueueCreateInfo qInfo = {};
        qInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        qInfo.queueFamilyIndex = m_QueueFamily;
        qInfo.queueCount = 1;
        qInfo.pQueuePriorities = &qPriorities;

        VkDeviceCreateInfo devInfo = {};
        devInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        devInfo.enabledExtensionCount = m_deviceExtensions.size();
        devInfo.ppEnabledExtensionNames = m_deviceExtensions.data();
        devInfo.queueCreateInfoCount = 1;
        devInfo.pQueueCreateInfos = &qInfo;

#ifdef ENABLE_DEBUG_LAYERS
        devInfo.enabledLayerCount = static_cast<uint32_t>(m_validationLayers.size());
        devInfo.ppEnabledLayerNames = m_validationLayers.data();
#endif
    }


    ////////////////////////////////////////////////////////////////////



    std::vector<VkExtensionProperties> VulkanEnumExtProps()
    {
        uint32_t numExt = 0;
        if (vkEnumerateInstanceExtensionProperties(NULL, &numExt, NULL) != VK_SUCCESS){
            assert(0);
        }

        std::vector<VkExtensionProperties> result(numExt);

        if (vkEnumerateInstanceExtensionProperties(NULL, &numExt, &result[0]) != VK_SUCCESS) {
            assert(0);
        }

#ifdef PRINTF_DEVICE_INFO
        printf("Found %d extensions\n", numExt);
        for (uint i = 0 ; i < numExt ; i++) {
            printf("Instance extension %d - %s\n", i, result[i].extensionName);
        }
#endif

        return result;
    }


    VulkanPhysicalDevices VulkanGetPhysicalDevices(
            const VkInstance &inst_,
            const VkSurfaceKHR &surface_)
    {
        VulkanPhysicalDevices result;

        uint32_t numDevices = 0;

        if (vkEnumeratePhysicalDevices(inst_, &numDevices, nullptr) != VK_SUCCESS){
            assert(0);
        }
#ifdef PRINTF_DEVICE_INFO
        printf("Num physical devices %d\n", numDevices);
#endif

        result.resize(numDevices);

        if (vkEnumeratePhysicalDevices(inst_, &numDevices, result.m_devices.data()) != VK_SUCCESS){
            assert(0);
        }

        for (size_t i = 0; i < numDevices; i++)
        {
            const VkPhysicalDevice &curDevice = result.m_devices[i];

            vkGetPhysicalDeviceProperties(curDevice, &result.m_devProps[i]);

            uint32_t numQFamily = 0;
            vkGetPhysicalDeviceQueueFamilyProperties(curDevice, &numQFamily, nullptr);
            result.m_qFamilyProps[i].resize(numQFamily);
            result.m_qSupportPresent[i].resize(numQFamily);

            vkGetPhysicalDeviceQueueFamilyProperties(curDevice, &numQFamily, result.m_qFamilyProps[i].data());

            for (size_t q = 0; q < numQFamily; q++)
            {
                if (vkGetPhysicalDeviceSurfaceSupportKHR(curDevice, q, surface_, &(result.m_qSupportPresent[i][q])) != VK_SUCCESS)
                {
                    assert(0);
                }
            }

            uint32_t numFormats = 0;
            vkGetPhysicalDeviceSurfaceFormatsKHR(curDevice, surface_, &numFormats, nullptr);
            assert(numFormats > 0);

            result.m_surfaceFormats[i].resize(numFormats);

            if (vkGetPhysicalDeviceSurfaceFormatsKHR(curDevice, surface_, &numFormats, result.m_surfaceFormats[i].data()) != VK_SUCCESS) {
                assert(0);
            }

            if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(curDevice, surface_, &(result.m_surfaceCaps[i])) != VK_SUCCESS) {
                assert(0);
            }
#ifdef PRINTF_DEVICE_INFO
            printf("Device name: %s\n", result.m_devProps[i].deviceName);
            uint32_t apiVer = result.m_devProps[i].apiVersion;
            printf("    API version: %d.%d.%d\n", VK_VERSION_MAJOR(apiVer),
                   VK_VERSION_MINOR(apiVer),
                   VK_VERSION_PATCH(apiVer));

            printf("    Num of family queues: %d\n", numQFamily);

            for (uint j = 0 ; j < numFormats ; j++) {
                const VkSurfaceFormatKHR& SurfaceFormat = result.m_surfaceFormats[i][j];
                printf("    Format %d color space %d\n", SurfaceFormat.format , SurfaceFormat.colorSpace);
            }
#endif
        }

        return result;
    }


    std::vector<const char*> getRequiredExtensions() {
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

        if (enableValidationLayers) {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        return extensions;
    }




#ifdef ENABLE_DEBUG_LAYERS

    /*static*/ VKAPI_ATTR VkBool32 VKAPI_CALL Core::debugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
            void* pUserData)
    {
        std::cerr << "validation layer [";

        if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) {
            std::cerr << "Verbose";
        }
        else if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
            std::cerr << "Info";
        }
        else if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
            std::cerr << "Warning";
        }
        else if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
            std::cerr << "Error";
        }
        std::cerr << "]: " << pCallbackData->pMessage << std::endl;

        return VK_FALSE;
    }

#endif //ENABLE_DEBUG_LAYERS

}//namespace vks