#pragma once
#include "core/eventsystem/EventListener.h"
#include "core/logging/Logger.h"

namespace crystal {
    struct KeyboardInfo { };

    class Keyboard : public EventListener {
    public:
        Keyboard() {
            register_event_handlers<&Keyboard::on_key>();
        }
    private:
        void on_key(KeyboardInfo kbdInfo) {
            Logger::Info("{} {}", "on_key event");
        }
    };
}
