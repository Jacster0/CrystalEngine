#pragma once

#include <cstdint>
#include <utility>
#include <array>
#include <optional>
#include <xcb/xcb.h>
#include "core/input/Keyboard.h"
#include "core/input/Mouse.h"

namespace crystal {
    struct application_create_info {
        int16_t x;
        int16_t y;
        int32_t width;
        int32_t height;
    };

    class platform {
    public:
        void create_main_window(application_create_info createInfo);
        static constexpr std::string_view get_extensions_name() noexcept { return "VK_KHR_xcb_surface";}
        std::optional<int> process_messages();
    private:
        Keyboard m_kbd;
        Mouse m_mouse;
        [[nodiscard]] std::pair<uint32_t, std::array<uint32_t,2>> get_event_info() const noexcept;
        void set_notifications() const noexcept;

        void on_key_notify(xcb_generic_event_t* event) noexcept;
        void on_mouse_notify(xcb_generic_event_t *event) noexcept;
        void on_mouse_move(xcb_generic_event_t *event) noexcept;
    };
}


