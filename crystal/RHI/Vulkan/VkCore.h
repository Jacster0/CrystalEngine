#pragma once
#include <vulkan/vulkan.h>
#include <optional>
#include "core/logging/Logger.h"

namespace crystal {
    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;
    };

    class RHICore {
    public:
        void initialize();
        void cleanUp();
    private:
        void createInstance();
        void selectPhysicalDevice();
        void createLogicalDevice();

        int32_t ratePhyscialDevice(VkPhysicalDevice device);
        QueueFamilyIndices findQueueFamilies(VkPhysicalDevice  device);

        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        VkDevice m_logicalDevice;
        VkQueue m_graphicsQueue;
        VkInstance m_instance;
        VkDebugUtilsMessengerEXT m_debugMessenger;
    };
}
