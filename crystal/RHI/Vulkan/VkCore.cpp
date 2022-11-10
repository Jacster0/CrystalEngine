#include "VkCore.h"
#include "ValidationLayers.h"
#include "platform/platform.h"

using namespace crystal;

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

    auto extensionName = platform::get_extensions_name().data();

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = 1;
    createInfo.ppEnabledExtensionNames = &extensionName;
    createInfo.enabledLayerCount = 0;

    if constexpr (validationLayers::enableValidationLayers()) {
        auto validationLayerNames = validationLayers::getValidationLayers();
        createInfo.enabledLayerCount = validationLayerNames.size();
        createInfo.ppEnabledLayerNames = validationLayerNames.data();
    }
    else {
        createInfo.enabledLayerCount = 0;
    }

    if(vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
        Logger::Error("failed to create instance!");
        throw std::runtime_error("failed to create instance!");
    }
    else {
        Logger::Info("Success Creating VK instance!");
    }
}
