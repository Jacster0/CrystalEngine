#include "platform/platform.h"
#include "core/logging/Logger.h"
#include "core/logging/ConsoleSink.h"

using namespace crystal;
int main() {
    platform platform;
    Logger::add_sink<ConsoleSink>();
    Logger::Info("{} !!! {}", "Hello", "World");

    platform.create_main_window({100,100, 750, 512});

    while(true) {
        if(const auto code = platform.process_messages()) {
            Logger::Info("Exit code: {}", code.value());
            break;
        }
    }

    crystal::Logger::remove_sink<ConsoleSink>();
}
