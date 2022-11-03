#pragma once
#include "core/eventsystem/EventListener.h"
#include "core/logging/Logger.h"
#include "platform/KeyCodes.h"
#include "core/lib/enum_utils.h"

namespace crystal {
    struct KeyEvent {
        KeyCode keyCode;
    };

    struct KeyDownEvent : KeyEvent {};
    struct KeyUpEvent : KeyEvent {};

    class Keyboard : public EventListener {
    public:
        Keyboard() {
            register_event_handlers<&Keyboard::on_key_down, &Keyboard::on_key_up>();
        }
    private:
        void on_key_down(KeyDownEvent event) {
            Logger::Info("{} {}", "on_key_down code: ", static_cast<int>(event.keyCode));
        }
        void on_key_up(KeyUpEvent event) {
            Logger::Info("{} {}", "on_key_up code: ", static_cast<int>(event.keyCode));
        }
    };
}
