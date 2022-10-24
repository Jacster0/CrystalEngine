#include <iostream>
#include "platform/linux/platform.h"

int main() {
    crystal::platform platform;
    platform.create_main_window({100,100, 512, 1024});
    while(true) {
        platform.process_messages();
    }
}
