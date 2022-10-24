#include "platform.h"
#include <xcb/xcb.h>
#include <X11/Xlib.h>
#include <X11/Xlib-xcb.h>
#include <stdexcept>
#include <array>
#include <iostream>

using namespace crystal;

static struct platform_info {
    Display* display;
    xcb_window_t window;
    xcb_screen_t* screen;
    xcb_connection_t* connection;
    xcb_atom_t wm_protocols;
    xcb_atom_t wm_delete_win;
} platformInfo;

void platform::create_main_window(application_create_info createInfo) {
    platformInfo.display = XOpenDisplay(nullptr);
    XAutoRepeatOff(platformInfo.display);

    platformInfo.connection = XGetXCBConnection(platformInfo.display);

    if(xcb_connection_has_error(platformInfo.connection)) {
        throw std::runtime_error("Failed to connect to X server via XCB");
    }

    const auto setup = xcb_get_setup(platformInfo.connection);

    platformInfo.screen = xcb_setup_roots_iterator(setup).data;
    platformInfo.window = xcb_generate_id(platformInfo.connection);

    uint32_t event_mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;

    auto[event_values, value_list] = get_event_info();

    xcb_create_window(
            platformInfo.connection,
            XCB_COPY_FROM_PARENT,
            platformInfo.window,
            platformInfo.screen->root,
            createInfo.x,
            createInfo.y,
            createInfo.width,
            createInfo.height,
            0,
            XCB_WINDOW_CLASS_INPUT_OUTPUT,
            platformInfo.screen->root_visual,
            event_mask,
            value_list.data()
    );

    set_notifications();

    xcb_map_window(platformInfo.connection, platformInfo.window);
    auto stream_result = xcb_flush(platformInfo.connection);

    if (stream_result <= 0) {
        std::cout << "An error occurred when flushing the stream " << stream_result;
    }
}

bool platform::process_messages() {
    xcb_generic_event_t* event;
    xcb_client_message_event_t* cm;

    while ((event = xcb_poll_for_event(platformInfo.connection))) {
        switch (event->response_type & ~0x80) { }
        free(event);
    }

    return true;
}

std::pair<uint32_t, std::array<uint32_t, 2>> platform::get_event_info() const noexcept {
    uint32_t event_mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;

    uint32_t event_values = XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE |
                            XCB_EVENT_MASK_KEY_PRESS    | XCB_EVENT_MASK_KEY_RELEASE    |
                            XCB_EVENT_MASK_EXPOSURE     | XCB_EVENT_MASK_POINTER_MOTION |
                            XCB_EVENT_MASK_STRUCTURE_NOTIFY;

    std::array value_list = { platformInfo.screen->black_pixel, event_values };

    return { event_mask, value_list };
}

void platform::set_notifications() const noexcept {
    constexpr std::string_view delete_window = "WM_DELETE_WINDOW";
    constexpr std::string_view wm_protocols = "WM_PROTOCOLS";

    xcb_intern_atom_cookie_t wm_delete_cookie = xcb_intern_atom(
            platformInfo.connection,
            0,
            delete_window.length(),
            delete_window.data()
    );
    xcb_intern_atom_cookie_t wm_protocols_cookie = xcb_intern_atom(
            platformInfo.connection,
            0,
            wm_protocols.length(),
            wm_protocols.data()
    );

    xcb_intern_atom_reply_t* wm_delete_reply = xcb_intern_atom_reply(
            platformInfo.connection,
            wm_delete_cookie,
            nullptr);

    xcb_intern_atom_reply_t* wm_protocols_reply = xcb_intern_atom_reply(
            platformInfo.connection,
            wm_protocols_cookie,
            nullptr);

    platformInfo.wm_delete_win = wm_delete_reply->atom;
    platformInfo.wm_protocols = wm_protocols_reply->atom;

    xcb_change_property(
            platformInfo.connection,
            XCB_PROP_MODE_REPLACE,
            platformInfo.window,
            platformInfo.wm_protocols,
            4,
            32,
            1,
            &platformInfo.wm_delete_win);
}
