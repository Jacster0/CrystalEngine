#pragma once
#include <vulkan/vulkan.h>

namespace crystal::validationLayers {
    constexpr std::array validationLayerNames = { "VK_LAYER_KHRONOS_validation"};

    constexpr auto enableValidationLayers() noexcept {
#ifdef NDEBUG
        return false;
#else
        return true;
#endif
    }

    bool checkSupport() noexcept;
    constexpr auto getValidationLayers() noexcept { return validationLayerNames; }

    void setupDebugMessenger(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger);

    VkResult CreateDebugUtilsMessengerEXT(
            VkInstance instance,
            const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
            const VkAllocationCallbacks* pAllocator,
            VkDebugUtilsMessengerEXT* pDebugMessenger);

    void DestroyDebugUtilsMessengerEXT(
            VkInstance instance,
            VkDebugUtilsMessengerEXT debugMessenger,
            const VkAllocationCallbacks* pAllocator);

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
            void* userData);

    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfoExt);
}
