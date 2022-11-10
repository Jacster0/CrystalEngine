#include <cstdint>
#include <vector>
#include <string_view>
#include "ValidationLayers.h"
#include "vulkan/vulkan.h"

using namespace crystal;

const std::vector validationLayerNames = { "VK_LAYER_KHRONOS_validation" };

bool validationLayers::checkSupport() noexcept {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const auto layerName : validationLayerNames) {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers) {
            if (std::string_view(layerName) == std::string_view(layerProperties.layerName)) {
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

std::vector<const char*> validationLayers::getValidationLayers() noexcept {
    return validationLayerNames;
}
