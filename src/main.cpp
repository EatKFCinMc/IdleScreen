#include "overlay.h"

#include <string>
#include <chrono>
#include <thread>
#include <iostream>


unsigned int kDefaultIdleTimeoutMs = 60000;
unsigned int idleIntervalMs = 10;
unsigned int activeIntervalMs = 100;
std::string path;

unsigned int ParseIdleTimeoutFromArgs(int argc, char** argv) {
    unsigned int timeout = kDefaultIdleTimeoutMs;

    if (argv) {
        path = argv[0];

        if (argc >= 2) {
            try {
                const std::string arg = argv[1];
                const unsigned int parsed = std::stoul(arg);
                if (parsed > 0) {
                    timeout = parsed * 1000;
                }
            } catch (...) {
                free(argv);
                return kDefaultIdleTimeoutMs;
            }
        }
        free(argv);
    }
    return timeout;
}


int main(int argc, char** argv) {
    const auto idleTimeoutMs = ParseIdleTimeoutFromArgs(argc, argv);

    overlay overlay;
    if (!overlay.init(idleTimeoutMs, idleIntervalMs, activeIntervalMs, path))
        return -1;


    auto timeToSleep = overlay.activeIntervalMs;
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(timeToSleep));
        auto idleMs = overlay::GetIdleMilliseconds();
        if (overlay.exit) break;
        if (overlay.pause) continue;

        if (idleMs >= overlay.IdleTimeoutMs && !overlay.isVisible()) { // idle
            overlay.show();
            timeToSleep = overlay.idleIntervalMs;
        } else if (idleMs < overlay.IdleTimeoutMs && overlay.isVisible()) { // active
            overlay.hide();
            timeToSleep = overlay.activeIntervalMs;
        }
    }

    overlay.hide();
    return 0;
}
