#pragma once
#include "core/eventsystem/EventListener.h"
#include "core/logging/Logger.h"
#include "KeyCodes.h"
#include "core/lib/enum_utils.h"
#include "core/lib/atomic_bitset.h"

namespace crystal {
    struct KeyEvent {
        KeyCode keyCode;
    };

    struct KeyDownEvent : KeyEvent {};
    struct KeyUpEvent : KeyEvent {};

    class Keyboard : public Singleton<Keyboard>, public EventListener {
    public:
        static auto key_is_pressed(KeyCode keyCode) noexcept {
            return get().m_keyStates[static_cast<size_t>(keyCode)];
        }

        Keyboard() {
            register_event_handlers<&Keyboard::on_key_down, &Keyboard::on_key_up>();
        }
    private:
        void on_key_down(KeyDownEvent event);
        void on_key_up(KeyUpEvent event);

        static constexpr auto m_numKeys{256u};
        crylib::atomic_bitset<m_numKeys> m_keyStates;
    };
}
