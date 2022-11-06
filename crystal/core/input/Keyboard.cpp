#include "Keyboard.h"
using namespace crystal;

void Keyboard::on_key_down(KeyDownEvent event) {
    //Logger::Info("{} {}", "on_key_down code: ", crylib::enum_to_string(event.keyCode));
    m_keyStates.set(static_cast<uint8_t>(event.keyCode), true);
}

void Keyboard::on_key_up(KeyUpEvent event) {
    //Logger::Info("{} {}", "on_key_up code: ", crylib::enum_to_string(event.keyCode));
    m_keyStates.set(static_cast<uint8_t>(event.keyCode), false);
}
