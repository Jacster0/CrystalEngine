#pragma once

#include <cstdint>
#include <utility>
#include <array>
#include <optional>
#include <xcb/xcb.h>
#include "core/input/Keyboard.h"

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
        std::optional<int> process_messages();
    private:
        Keyboard kbd;
        [[nodiscard]] std::pair<uint32_t, std::array<uint32_t,2>> get_event_info() const noexcept;
        void set_notifications() const noexcept;

        void on_key_notify(xcb_generic_event_t* event) noexcept;
    };
}


