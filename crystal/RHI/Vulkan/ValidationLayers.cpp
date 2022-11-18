#include <cstdint>
#include <vector>
#include <array>
#include <string_view>
#include "ValidationLayers.h"
#include "vulkan/vulkan.h"
#include "core/logging/Logger.h"

using namespace crystal;

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

VkBool32 validationLayers::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                         VkDebugUtilsMessageTypeFlagsEXT messageType,
                                         const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
                                         void* userData)
{
    if(messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        Logger::Info("validation layer: {}", callbackData->pMessage);
    }

    return VK_FALSE;
}

void validationLayers::setupDebugMessenger(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger) {
    if(!enableValidationLayers()) {
        return;
    }

    VkDebugUtilsMessengerCreateInfoEXT createInfo{};
    populateDebugMessengerCreateInfo(createInfo);

    if(CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
        Logger::Error("failed to set up debug messenger!");
        throw std::runtime_error("failed to set up debug messenger!");
    }
}

VkResult validationLayers::CreateDebugUtilsMessengerEXT(VkInstance instance,
                                                        const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                                        const VkAllocationCallbacks *pAllocator,
                                                        VkDebugUtilsMessengerEXT *pDebugMessenger)
{
    auto function = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance,"vkCreateDebugUtilsMessengerEXT");

    if(function) {
        return function(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void validationLayers::DestroyDebugUtilsMessengerEXT(
        VkInstance instance,
        VkDebugUtilsMessengerEXT debugMessenger,
        const VkAllocationCallbacks *pAllocator)
{
    auto function = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (function != nullptr) {
        function(instance, debugMessenger, pAllocator);
    }
}

void validationLayers::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfoExt) {
    createInfoExt = {};
    createInfoExt.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfoExt.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfoExt.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfoExt.pfnUserCallback = debugCallback;
}
