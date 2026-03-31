#include "overlay.h"

#include <string>
#include <chrono>
#include <thread>
#include <iostream>


unsigned int kDefaultIdleTimeoutMs = 60000;
unsigned int idleIntervalMs = 10;
unsigned int activeIntervalMs = 100;

unsigned int ParseIdleTimeoutFromArgs(int argc, char** argv) {
    unsigned int timeout = kDefaultIdleTimeoutMs;
    if (argv && argc >= 2) {
        try {
            const std::string arg = argv[1];
            const unsigned int parsed = std::stoul(arg);
            if (parsed > 0) {
                timeout = parsed * 1000;
            }
        } catch (...) {
            if (argv)
                free(argv);
            return 60000;
        }
    }
    if (argv)
        free(argv);
    return timeout;
}


int main(int argc, char** argv) {
    const auto idleTimeoutMs = ParseIdleTimeoutFromArgs(argc, argv);

    overlay overlay;
    if (!overlay.init(idleTimeoutMs, idleIntervalMs, activeIntervalMs))
        return -1;


    auto timeToSleep = overlay.activeIntervalMs;
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(timeToSleep));
        auto idleMs = overlay::GetIdleMilliseconds();
        if (overlay.exit) break;
        if (overlay.pause) continue;

        if (idleMs >= idleTimeoutMs && !overlay.isVisible()) { // idle
            overlay.show();
            timeToSleep = overlay.idleIntervalMs;
        } else if (idleMs < idleTimeoutMs && overlay.isVisible()) { // active
            overlay.hide();
            timeToSleep = overlay.activeIntervalMs;
        }
    }

    overlay.hide();
    return 0;
}
