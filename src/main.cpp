#include "overlay.h"

#include <string>
#include <chrono>
#include <thread>

// namespace {

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

// } // namespace

int main(int argc, char** argv) {
    const auto idleTimeoutMs = ParseIdleTimeoutFromArgs(argc, argv);

    overlay overlay;
    overlay.init(idleTimeoutMs, idleIntervalMs, activeIntervalMs);

    auto timeToSleep = overlay.activeIntervalMs;
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(timeToSleep));
        auto idleMs = overlay::GetIdleMilliseconds();

        if (idleMs >= idleTimeoutMs && !overlay.isVisible()) { // idle
            overlay.show();
            timeToSleep = overlay.idleIntervalMs;
        } else if (idleMs < idleTimeoutMs && overlay.isVisible()) { // active
            overlay.hide();
            timeToSleep = overlay.activeIntervalMs;
        }

        if (overlay.exit) break;
    }

    overlay.hide();
    return 0;
}
