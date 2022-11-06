#include "platform/platform.h"
#include "core/logging/Logger.h"
#include "core/logging/ConsoleSink.h"
#include "core/input/Keyboard.h"
#include <thread>

using namespace crystal;

[[noreturn]] void process() {
    while(true) {
        if(Keyboard::key_is_pressed(KeyCode::J)) {
            Logger::Info("Thread1 report: Key \"J\" was pressed");
        }
    }
}

[[noreturn]] void process2() {
    while(true) {
        if(Keyboard::key_is_pressed(KeyCode::J)) {
            Logger::Info("Thread2 report: Key \"J\" was pressed");
        }
    }
}

int main() {
    platform platform;
    Logger::add_sink<ConsoleSink>();
    Logger::Info("{} !!! {}", "Hello", "World");

    platform.create_main_window({100,100, 750, 512});

    std::jthread t1(&process);
    std::jthread t2(&process2);

    while(true) {
        if(const auto code = platform.process_messages()) {
            Logger::Info("Exit code: {}", code.value());
            break;
        }
    }

    crystal::Logger::remove_sink<ConsoleSink>();
}
