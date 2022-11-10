#include "platform/platform.h"
#include "core/logging/Logger.h"
#include "core/logging/ConsoleSink.h"
#include "core/input/Keyboard.h"
#include <thread>
#include "RHI/RHICore.h"

using namespace crystal;

int main() {
    platform platform;
    Logger::add_sink<ConsoleSink>();
    Logger::Info("{} !!! {}", "Hello", "World");

    platform.create_main_window({100,100, 750, 512});

    RHICore rhiCore{};
    rhiCore.initialize();

    while(true) {
        if(const auto code = platform.process_messages()) {
            Logger::Info("Exit code: {}", code.value());
            break;
        }
    }

    rhiCore.cleanUp();
    crystal::Logger::remove_sink<ConsoleSink>();
}
