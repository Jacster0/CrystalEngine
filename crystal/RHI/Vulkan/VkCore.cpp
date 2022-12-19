#include <map>
#include "VkCore.h"
#include "ValidationLayers.h"
#include "platform/platform.h"

using namespace crystal;

void RHICore::initialize() {
    createInstance();
    validationLayers::setupDebugMessenger(m_instance, m_debugMessenger);
    selectPhysicalDevice();
    createLogicalDevice();
}

void RHICore::createInstance() {
    if(validationLayers::enableValidationLayers() && !validationLayers::checkSupport()) {
        Logger::Error("validation layers requested, but not available!");
        throw std::runtime_error("validation layers requested, but not available!");
    }

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Vulkan Test";
    appInfo.applicationVersion = VK_MAKE_VERSION(1,0,0);
    appInfo.pEngineName = "Crystal";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    std::vector extensions {
        "VK_KHR_surface", platform::get_extensions_name().data()
    };

    if(validationLayers::enableValidationLayers()) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = extensions.size();
    createInfo.ppEnabledExtensionNames = extensions.data();
    createInfo.enabledLayerCount = 0;

    const std::vector<const char*> validationLayers = {
            "VK_LAYER_KHRONOS_validation"
    };

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if constexpr (validationLayers::enableValidationLayers()) {
        constexpr auto validationLayerNames = validationLayers::getValidationLayers();
        constexpr auto pName = validationLayerNames.front();

        createInfo.enabledLayerCount = validationLayerNames.size();
        createInfo.ppEnabledLayerNames = &pName;

        validationLayers::populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = &debugCreateInfo;
    }
    else {
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
    }

    if(vkCreateInstance(&createInfo, nullptr, &m_instance) != VK_SUCCESS) {
        Logger::Error("failed to create instance!");
        throw std::runtime_error("failed to create instance!");
    }
    else {
        Logger::Info("Success Creating VK instance!");
    }
}

void RHICore::cleanUp() {
    vkDestroyDevice(m_logicalDevice, nullptr);

    if(validationLayers::enableValidationLayers()) {
        validationLayers::DestroyDebugUtilsMessengerEXT(m_instance, m_debugMessenger, nullptr);
    }
    vkDestroyInstance(m_instance, nullptr);
}

void RHICore::selectPhysicalDevice() {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);

    if (deviceCount == 0) {
        Logger::Error("failed to find GPUs with Vulkan support!");
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data());

    std::multimap<int32_t,VkPhysicalDevice> candidates;
    for(const auto& device : devices) {
        auto score = ratePhyscialDevice(device);

        auto indices = findQueueFamilies(device);

        if(indices.graphicsFamily.has_value()) {
            candidates.insert({score, device});
        }
    }

    if(candidates.rbegin()->first > 0) {
        physicalDevice = candidates.rbegin()->second;
        Logger::Info("Found a suitable GPU!");
    }
    else {
        Logger::Error("failed to find a suitable GPU!");
        throw std::runtime_error("failed to find a suitable GPU!");
    };
}

int32_t RHICore::ratePhyscialDevice(VkPhysicalDevice device) {
    int32_t score{};

    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    if(!deviceFeatures.geometryShader) {
        return score;
    }

    if(deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
        score += 1000;
    }
    score += deviceProperties.limits.maxImageDimension2D;

    return score;
}

QueueFamilyIndices RHICore::findQueueFamilies(VkPhysicalDevice device) {
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount{};
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    uint32_t index{};
    for(const auto& queueFamily : queueFamilies) {
        if(queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = index++;
            break;
        }
    }

    return indices;
}

void RHICore::createLogicalDevice() {
    auto queueFamilyIndices = findQueueFamilies(physicalDevice);
    float queuePriority{1.0f};

    VkDeviceQueueCreateInfo queueCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .queueFamilyIndex = queueFamilyIndices.graphicsFamily.value(),
        .queueCount = 1,
        .pQueuePriorities = &queuePriority
    };

    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo createInfo {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos = &queueCreateInfo,
        .enabledExtensionCount = 0,
        .pEnabledFeatures = &deviceFeatures
    };

    if(vkCreateDevice(physicalDevice, &createInfo, nullptr, &m_logicalDevice) != VK_SUCCESS) {
        Logger::Error("failed to create logical device!");
        throw std::runtime_error("failed to create logical device!");
    }
    else {
        Logger::Info("success creating logical device!");
    }

    vkGetDeviceQueue(m_logicalDevice, queueFamilyIndices.graphicsFamily.value(), 0, &m_graphicsQueue);
}

