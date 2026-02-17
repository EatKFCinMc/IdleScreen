#include "BlackOverlay.h"

#include <string>
#include <chrono>
#include <thread>

namespace {

unsigned int kDefaultIdleTimeoutMs = 6000;
unsigned int idleIntervalMs = 100;
unsigned int activeIntervalMs = 1000;

unsigned int ParseIdleTimeoutFromArgs(int argc, char** argv) {
    unsigned int timeout = kDefaultIdleTimeoutMs;
    if (argv && argc >= 2) {
        try {
            const std::string arg = argv[1];
            const unsigned int parsed = std::stoul(arg);
            if (parsed > 0) {
                timeout = parsed;
            }
        } catch (...) { }
    }
    if (argv)
        free(argv);
    return timeout;
}

} // namespace

int main(int argc, char** argv) {
    const auto idleTimeoutMs = ParseIdleTimeoutFromArgs(argc, argv);

    BlackOverlay overlay;
    overlay.initialize();

    auto timeToSleep = activeIntervalMs;
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(timeToSleep));
        auto idleMs = BlackOverlay::GetIdleMilliseconds();

        if (idleMs >= idleTimeoutMs && !overlay.isVisible()) { // idle
            overlay.show();
            timeToSleep = idleIntervalMs;
        } else if (idleMs < idleTimeoutMs && overlay.isVisible()) { // active
            overlay.hide();
            timeToSleep = activeIntervalMs;
        }
    }

    // overlay.hide();
    // return 0;
}
