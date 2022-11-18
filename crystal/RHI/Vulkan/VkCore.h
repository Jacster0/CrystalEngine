#pragma once
#include <vulkan/vulkan.h>
#include "core/logging/Logger.h"

namespace crystal {
    class RHICore {
    public:
        void initialize();
        void cleanUp();
    private:
        void createInstance();

        VkInstance m_instance;
        VkDebugUtilsMessengerEXT m_debugMessenger;
    };
}
