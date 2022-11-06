#pragma once
#include "core/eventsystem/EventListener.h"
#include "core/lib/atomic_bitset.h"
#include "MouseCodes.h"

namespace crystal {
    struct MouseClickEvent {
        MouseButton button;
    };

    struct MouseDownEvent : MouseClickEvent {

    };

    struct MouseUpEvent : MouseClickEvent {

    };

    class Mouse : public Singleton<Mouse>, public EventListener {
    public:
        Mouse() {
            register_event_handlers<&Mouse::on_mouse_press, &Mouse::on_mouse_release>();
        }
    private:
        void on_mouse_press(MouseDownEvent event);
        void on_mouse_release(MouseUpEvent event);

        static constexpr auto m_numKeys{3};
        crylib::atomic_bitset<m_numKeys> m_mouseStates;
    };
}
