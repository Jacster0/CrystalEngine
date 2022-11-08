#include "Mouse.h"
#include "core/logging/Logger.h"
#include "core/lib/enum_utils.h"

using namespace crystal;

void Mouse::on_mouse_press(MouseDownEvent event) {
    Logger::Info("OnMousePress button: {}",  crylib::enum_to_string(event.button));
}

void Mouse::on_mouse_release(MouseUpEvent event) {
    Logger::Info("OnMouseRelease button: {}", crylib::enum_to_string(event.button));
}

void Mouse::on_mouse_move(MouseMoveEvent event) {
    Logger::Info("OnMouseMove Location: X:{} Y:{}", event.X, event.Y);
}
