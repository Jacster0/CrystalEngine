#include "platform/platform.h"
#include "core/logging/Logger.h"
#include "core/logging/ConsoleSink.h"
#include "core/input/Keyboard.h"
#include <thread>
#include <vulkan/vulkan.h>

using namespace crystal;

class Vulkan {
public:
    void init() {
        createInstance();
    }
    void cleanUp() {
        vkDestroyInstance(instance, nullptr);
    }
private:
    void createInstance() {
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Vulkan Test";
        appInfo.applicationVersion = VK_MAKE_VERSION(1,0,0);
        appInfo.pEngineName = "Crystal";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        const char* extensionName = "VK_KHR_xcb_surface";
        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledExtensionCount = 1;
        createInfo.ppEnabledExtensionNames = &extensionName;
        createInfo.enabledLayerCount = 0;

        if(vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
            Logger::Error("failed to create instance!");
            throw std::runtime_error("failed to create instance!");
        }
        else {
            Logger::Info("Success Creating VK instance!");
        }
    }
    VkInstance instance;
};

int main() {
    platform platform;
    Logger::add_sink<ConsoleSink>();
    Logger::Info("{} !!! {}", "Hello", "World");

    platform.create_main_window({100,100, 750, 512});

    Vulkan vulkan{};
    vulkan.init();

    while(true) {
        if(const auto code = platform.process_messages()) {
            Logger::Info("Exit code: {}", code.value());
            break;
        }
    }

    vulkan.cleanUp();
    crystal::Logger::remove_sink<ConsoleSink>();
}
