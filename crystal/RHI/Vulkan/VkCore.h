#pragma once
#include <vulkan/vulkan.h>
#include "core/logging/Logger.h"

namespace crystal {
    class RHICore {
    public:
        void initialize() {
            createInstance();
        }
        void cleanUp() {
            vkDestroyInstance(instance, nullptr);
        }
    private:
        void createInstance();
        VkInstance instance;
    };
}
