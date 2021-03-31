/*************************************************************************
 *
 * vkCore.cpp
 *
 * Created by dmitry
 * 22.02.2021
 *
 ***/

#include "vkCore.h"

#include <cassert>
#include <cstring>


namespace ezg {

    bool Core::Init(GLFWwindow* pWindow)
    {
        std::vector<VkExtensionProperties> ExtProps = VulkanEnumExtProps();

        createInstance_();

        if (glfwCreateWindowSurface(m_inst, pWindow, nullptr, &m_surface) != VK_SUCCESS) {
            throw std::runtime_error(("failed to create window surface!"));
        }

        m_physDevices = VulkanGetPhysicalDevices(m_inst, m_surface);
        selectPhysicalDevice_();
        createLogicalDevice();

        return true;
    }


    VkImageView Core::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags) {
        VkImageViewCreateInfo viewInfo = {};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = image;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = format;
        viewInfo.subresourceRange.aspectMask = aspectFlags;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        VkImageView imageView;
        if (vkCreateImageView(getDevice(), &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
            throw std::runtime_error(("failed to create texture image view!"));
        }

        return imageView;
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
            throw std::runtime_error(("validation layers requested, but not available!"));
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
            throw std::runtime_error(("failed to create instance!"));
        }
    }

#ifdef ENABLE_DEBUG_LAYERS
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
#endif

    void Core::selectPhysicalDevice_()
    {
        for (size_t i = 0, mi = m_physDevices.m_devices.size(); i < mi; i++)
        {
            const VkSurfaceCapabilitiesKHR& SurfaceCaps = m_physDevices.m_surfaceCaps[i];
            if (SurfaceCaps.minImageCount > SurfaceCaps.maxImageCount)
            {
                printf("Chawo?!? (SurfaceCaps.minImageCount > SurfaceCaps.maxImageCount)\n");
                continue;
            }


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
                    printf("Using GFX device %lu and queue family %lu\n", m_DeviceIndex, m_QueueFamily);
#endif
                    
                    return;
                }

            }
        }

        if (m_DeviceIndex < 0) {
            throw std::runtime_error(("fatal error in Core::selectPhysicalDevice(): No GFX device found!"));
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

        if (VK_SUCCESS != vkCreateDevice(getPhysDevice(), &devInfo, NULL, &m_device)) {
            throw std::runtime_error(("failed to create logical device!"));
        }
    }


    uint32_t Core::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
    {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(m_physDevices.m_devices[m_DeviceIndex], &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
        {
            if (typeFilter & (1 << i) &&
                (memProperties.memoryTypes[i].propertyFlags & properties) == properties) 
            {
                return i;
            }
        }

        throw std::runtime_error(("failed to find suitable memory type!"));
    }


    VkFormat Core::findSupportedFormat(const std::vector< VkFormat >& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
    {
        for (VkFormat format : candidates)
        {
            VkFormatProperties props = {};
            vkGetPhysicalDeviceFormatProperties(m_physDevices.m_devices[m_DeviceIndex], format, &props);
        
            if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
            {
                return format;
            }
            else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
            {
                return format;
            }
        }

        throw std::runtime_error(("failed to find supported format!"));
    }



    ////////////////////////////////////////////////////////////////////



    std::vector<VkExtensionProperties> VulkanEnumExtProps()
    {
        uint32_t numExt = 0;
        if (vkEnumerateInstanceExtensionProperties(NULL, &numExt, NULL) != VK_SUCCESS) {
            throw std::runtime_error(("failed to enumerate instance extension properties!"));
        }

        std::vector<VkExtensionProperties> result(numExt);

        if (vkEnumerateInstanceExtensionProperties(NULL, &numExt, &result[0]) != VK_SUCCESS) {
            throw std::runtime_error(("failed to enumerate instance extension properties!"));
        }

#ifdef PRINTF_DEVICE_INFO
        printf("Found %d extensions\n", numExt);
        for (uint32_t i = 0 ; i < numExt ; i++) {
            printf("Instance extension %d - %s\n", i, result[i].extensionName);
        }
#endif

        return result;
    }


    void VulkanPhysicalDevices::update(const VkSurfaceKHR& surface_)
    {
        for (size_t i = 0, mi = m_devices.size(); i < mi; i++)
        {
            const VkPhysicalDevice& curDevice = m_devices[i];

            vkGetPhysicalDeviceProperties(curDevice, &m_devProps[i]);

            uint32_t numQFamily = 0;
            vkGetPhysicalDeviceQueueFamilyProperties(curDevice, &numQFamily, nullptr);
            m_qFamilyProps[i].resize(numQFamily);
            m_qSupportPresent[i].resize(numQFamily);

            vkGetPhysicalDeviceQueueFamilyProperties(curDevice, &numQFamily, m_qFamilyProps[i].data());

            for (size_t q = 0; q < numQFamily; q++)
            {
                if (vkGetPhysicalDeviceSurfaceSupportKHR(curDevice, q, surface_, &(m_qSupportPresent[i][q])) != VK_SUCCESS)
                {
                    throw std::runtime_error(("failed to get physical Device surface support!"));
                }
            }

            uint32_t numFormats = 0;
            if (vkGetPhysicalDeviceSurfaceFormatsKHR(curDevice, surface_, &numFormats, nullptr) != VK_SUCCESS) {
                throw std::runtime_error(("failed to get physical Device surface formats!"));
            }
            assert(numFormats > 0);

            m_surfaceFormats[i].resize(numFormats);

            if (vkGetPhysicalDeviceSurfaceFormatsKHR(curDevice, surface_, &numFormats, m_surfaceFormats[i].data()) != VK_SUCCESS) {
                throw std::runtime_error(("failed to get physical Device surface formats!"));
            }

            if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(curDevice, surface_, &(m_surfaceCaps[i])) != VK_SUCCESS) {
                throw std::runtime_error(("failed to get physical Device surface Capabilities!"));
            }
#ifdef PRINTF_DEVICE_INFO
            printf("Device name: %s\n", m_devProps[i].deviceName);
            uint32_t apiVer = m_devProps[i].apiVersion;
            printf("    API version: %d.%d.%d\n", VK_VERSION_MAJOR(apiVer),
                VK_VERSION_MINOR(apiVer),
                VK_VERSION_PATCH(apiVer));

            printf("    Num of family queues: %d\n", numQFamily);

            for (uint32_t j = 0; j < numFormats; j++) {
                const VkSurfaceFormatKHR& SurfaceFormat = m_surfaceFormats[i][j];
                printf("    Format %d color space %d\n", SurfaceFormat.format, SurfaceFormat.colorSpace);
            }
#endif
        }
    }


    VulkanPhysicalDevices VulkanGetPhysicalDevices(
            const VkInstance &inst_,
            const VkSurfaceKHR &surface_)
    {
        VulkanPhysicalDevices result;

        uint32_t numDevices = 0;

        if (vkEnumeratePhysicalDevices(inst_, &numDevices, nullptr) != VK_SUCCESS){
            throw std::runtime_error(("failed to enumerate physical devices!"));
        }
#ifdef PRINTF_DEVICE_INFO
        printf("Num physical devices %d\n", numDevices);
#endif

        result.resize(numDevices);

        if (vkEnumeratePhysicalDevices(inst_, &numDevices, result.m_devices.data()) != VK_SUCCESS){
            throw std::runtime_error(("failed to enumerate physical devices!"));
        }

        result.update(surface_);

        return result;
    }


    std::vector<const char*> getRequiredExtensions() 
    {
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

#ifdef ENABLE_DEBUG_LAYERS
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
        //extensions.push_back(VK_EXT_DEBUG_MARKER_EXTENSION_NAME);
#endif

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