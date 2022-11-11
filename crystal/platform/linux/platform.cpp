#include "platform.h"
#include "core/eventsystem/EventSystem.h"
#include "core/logging/Logger.h"
#include "core/input/Keyboard.h"
#include "core/input/KeyCodes.h"
#include "core/input/MouseCodes.h"
#include <xcb/xcb.h>
#include <X11/keysym.h>
#include <X11/XKBlib.h>
#include <X11/Xlib.h>
#include <X11/Xlib-xcb.h>
#include <stdexcept>
#include <array>

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
        Logger::Error("An error occurred when flushing the stream. Result: {}", stream_result);
    }
}

std::optional<int> platform::process_messages() {
    xcb_generic_event_t* event;
    xcb_client_message_event_t* cm;

    while ((event = xcb_poll_for_event(platformInfo.connection))) {
        switch (event->response_type & ~0x80) {
            case XCB_KEY_PRESS:
            case XCB_KEY_RELEASE:
                on_key_notify(event);
                break;
            case XCB_BUTTON_PRESS:
            case XCB_BUTTON_RELEASE:
                on_mouse_notify(event);
                break;
            case XCB_MOTION_NOTIFY:
                on_mouse_move(event);
            case XCB_CLIENT_MESSAGE:
                auto msg = reinterpret_cast<xcb_client_message_event_t*>(event);

                if(msg->data.data32[0] == platformInfo.wm_delete_win) {
                    return 1;
                }
        }
        free(event);
    }

    return {};
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

crystal::KeyCode translate_keycode(uint32_t keySym) {
    switch (keySym) {
        case XK_BackSpace:
            return crystal::KeyCode::Back;
        case XK_Return:
            return crystal::KeyCode::Enter;
        case XK_Tab:
            return crystal::KeyCode::Tab;
        case XK_Pause:
            return crystal::KeyCode::Pause;
        case XK_Caps_Lock:
            return crystal::KeyCode::CapsLock;
        case XK_Escape:
            return crystal::KeyCode::Escape;
        case XK_space:
            return crystal::KeyCode::Space;
        case XK_End:
            return crystal::KeyCode::End;
        case XK_Home:
            return crystal::KeyCode::Home;
        case XK_Left:
            return crystal::KeyCode::Left;
        case XK_Up:
            return crystal::KeyCode::Up;
        case XK_Right:
            return crystal::KeyCode::Right;
        case XK_Down:
            return crystal::KeyCode::Down;
        case XK_Select:
            return crystal::KeyCode::Select;
        case XK_Print:
            return crystal::KeyCode::Print;
        case XK_Execute:
            return crystal::KeyCode::Execute;
        case XK_Insert:
            return crystal::KeyCode::Insert;
        case XK_Delete:
            return crystal::KeyCode::Delete;
        case XK_Help:
            return crystal::KeyCode::Help;
        case XK_KP_0:
            return crystal::KeyCode::NumPad0;
        case XK_KP_1:
            return crystal::KeyCode::NumPad1;
        case XK_KP_2:
            return crystal::KeyCode::NumPad2;
        case XK_KP_3:
            return crystal::KeyCode::NumPad3;
        case XK_KP_4:
            return crystal::KeyCode::NumPad4;
        case XK_KP_5:
            return crystal::KeyCode::NumPad5;
        case XK_KP_6:
            return crystal::KeyCode::NumPad6;
        case XK_KP_7:
            return crystal::KeyCode::NumPad7;
        case XK_KP_8:
            return crystal::KeyCode::NumPad8;
        case XK_KP_9:
            return crystal::KeyCode::NumPad9;
        case XK_KP_Multiply:
            return crystal::KeyCode::Multiply;
        case XK_KP_Add:
            return crystal::KeyCode::Add;
        case XK_KP_Separator:
            return crystal::KeyCode::Separator;
        case XK_KP_Subtract:
            return crystal::KeyCode::Subtract;
        case XK_KP_Decimal:
            return crystal::KeyCode::Decimal;
        case XK_KP_Divide:
            return crystal::KeyCode::Divide;
        case XK_F1:
            return crystal::KeyCode::F1;
        case XK_F2:
            return crystal::KeyCode::F2;
        case XK_F3:
            return crystal::KeyCode::F3;
        case XK_F4:
            return crystal::KeyCode::F4;
        case XK_F5:
            return crystal::KeyCode::F5;
        case XK_F6:
            return crystal::KeyCode::F6;
        case XK_F7:
            return crystal::KeyCode::F7;
        case XK_F8:
            return crystal::KeyCode::F8;
        case XK_F9:
            return crystal::KeyCode::F9;
        case XK_F10:
            return crystal::KeyCode::F10;
        case XK_F11:
            return crystal::KeyCode::F11;
        case XK_F12:
            return crystal::KeyCode::F12;
        case XK_F13:
            return crystal::KeyCode::F13;
        case XK_F14:
            return crystal::KeyCode::F14;
        case XK_F15:
            return crystal::KeyCode::F15;
        case XK_F16:
            return crystal::KeyCode::F16;
        case XK_F17:
            return crystal::KeyCode::F17;
        case XK_F18:
            return crystal::KeyCode::F18;
        case XK_F19:
            return crystal::KeyCode::F19;
        case XK_F20:
            return crystal::KeyCode::F20;
        case XK_F21:
            return crystal::KeyCode::F21;
        case XK_F22:
            return crystal::KeyCode::F22;
        case XK_F23:
            return crystal::KeyCode::F23;
        case XK_F24:
            return crystal::KeyCode::F24;
        case XK_Num_Lock:
            return crystal::KeyCode::NumLock;
        case XK_Scroll_Lock:
            return crystal::KeyCode::Scroll;
        case XK_Shift_L:
            return crystal::KeyCode::LShift;
        case XK_Shift_R:
            return crystal::KeyCode::RShift;
        case XK_Control_L:
            return crystal::KeyCode::LControl;
        case XK_Control_R:
            return crystal::KeyCode::RControl;
        case XK_Alt_L:
            return crystal::KeyCode::Alt;
        case XK_semicolon:
            return crystal::KeyCode::OemSemicolon;
        case XK_plus:
            return crystal::KeyCode::OemPlus;
        case XK_minus:
            return crystal::KeyCode::OemMinus;
        case XK_comma:
            return crystal::KeyCode::OemComma;
        case XK_period:
            return crystal::KeyCode::OemPeriod;
        case XK_0:
            return crystal::KeyCode::D0;
        case XK_1:
            return crystal::KeyCode::D1;
        case XK_2:
            return crystal::KeyCode::D2;
        case XK_3:
            return crystal::KeyCode::D3;
        case XK_4:
            return crystal::KeyCode::D4;
        case XK_5:
            return crystal::KeyCode::D5;
        case XK_6:
            return crystal::KeyCode::D6;
        case XK_7:
            return crystal::KeyCode::D7;
        case XK_8:
            return crystal::KeyCode::D8;
        case XK_9:
            return crystal::KeyCode::D9;
        case XK_a:
        case XK_A:
            return crystal::KeyCode::A;
        case XK_b:
        case XK_B:
            return crystal::KeyCode::B;
        case XK_c:
        case XK_C:
            return crystal::KeyCode::C;
        case XK_d:
        case XK_D:
            return crystal::KeyCode::D;
        case XK_e:
        case XK_E:
            return crystal::KeyCode::E;
        case XK_f:
        case XK_F:
            return crystal::KeyCode::F;
        case XK_g:
        case XK_G:
            return crystal::KeyCode::G;
        case XK_h:
        case XK_H:
            return crystal::KeyCode::H;
        case XK_i:
        case XK_I:
            return crystal::KeyCode::I;
        case XK_j:
        case XK_J:
            return crystal::KeyCode::J;
        case XK_k:
        case XK_K:
            return crystal::KeyCode::K;
        case XK_l:
        case XK_L:
            return crystal::KeyCode::L;
        case XK_m:
        case XK_M:
            return crystal::KeyCode::M;
        case XK_n:
        case XK_N:
            return crystal::KeyCode::N;
        case XK_o:
        case XK_O:
            return crystal::KeyCode::O;
        case XK_p:
        case XK_P:
            return crystal::KeyCode::P;
        case XK_q:
        case XK_Q:
            return crystal::KeyCode::Q;
        case XK_r:
        case XK_R:
            return crystal::KeyCode::R;
        case XK_s:
        case XK_S:
            return crystal::KeyCode::S;
        case XK_t:
        case XK_T:
            return crystal::KeyCode::T;
        case XK_u:
        case XK_U:
            return crystal::KeyCode::U;
        case XK_v:
        case XK_V:
            return crystal::KeyCode::V;
        case XK_w:
        case XK_W:
            return crystal::KeyCode::W;
        case XK_x:
        case XK_X:
            return crystal::KeyCode::X;
        case XK_y:
        case XK_Y:
            return crystal::KeyCode::Y;
        case XK_z:
        case XK_Z:
            return crystal::KeyCode::Z;;
    }
}

crystal::MouseButton translate_mousecode(xcb_button_t btn) {
    switch (btn) {
        case XCB_BUTTON_INDEX_1:
            return MouseButton::Left;
        case XCB_BUTTON_INDEX_2:
            return MouseButton::Middle;
        case XCB_BUTTON_INDEX_3:
            return MouseButton::Right;
    }
}

void platform::on_key_notify(xcb_generic_event_t* event) noexcept {
    auto keyPressEvent = reinterpret_cast<xcb_key_press_event_t*>(event);

    auto keySym = XkbKeycodeToKeysym(platformInfo.display, static_cast<::KeyCode>(keyPressEvent->detail),0, 0);
    auto keyCode = translate_keycode(keySym);

    if(event->response_type == XCB_KEY_PRESS) {
        EventSystem::notify<KeyDownEvent>(keyCode);
    }
    else {
        EventSystem::notify<KeyUpEvent>(keyCode);
    } 
}

void platform::on_mouse_notify(xcb_generic_event_t *event) noexcept {
    auto mouseEvent = reinterpret_cast<xcb_button_press_event_t*>(event);
    auto buttonPressed = translate_mousecode(mouseEvent->detail);

    if(event->response_type == XCB_BUTTON_PRESS) {
        EventSystem::notify<MouseDownEvent>(buttonPressed);
    }
    else {
        EventSystem::notify<MouseUpEvent>(buttonPressed);
    }
}

void platform::on_mouse_move(xcb_generic_event_t *event) noexcept {
    auto mouseEvent = reinterpret_cast<xcb_motion_notify_event_t*>(event);

    EventSystem::notify<MouseMoveEvent>(static_cast<uint32_t>(mouseEvent->event_x), static_cast<uint32_t>(mouseEvent->event_y));
}