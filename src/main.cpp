// main.cpp
// Black-screen idle watcher: shows a full-screen black overlay after user inactivity.

#include "BlackOverlay.h"

#include <shellapi.h>
#include <windows.h>

#include <string>

namespace {

constexpr DWORD kDefaultIdleTimeoutMs = 60'000; // 1 minute default.
constexpr UINT kTimerIntervalMs = 100;

DWORD GetIdleMilliseconds() {
    LASTINPUTINFO info{};
    info.cbSize = sizeof(LASTINPUTINFO);
    if (!GetLastInputInfo(&info)) {
        return 0;
    }
    const DWORD now = GetTickCount();
    return now - info.dwTime;
}

DWORD ParseIdleTimeoutFromArgs() {
    int argc = 0;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    DWORD timeout = kDefaultIdleTimeoutMs;
    if (argv && argc >= 2) {
        try {
            const std::wstring arg = argv[1];
            const DWORD parsed = std::stoul(arg);
            if (parsed > 0) {
                timeout = parsed;
            }
        } catch (...) {
            // Keep default on parse failure.
        }
    }
    if (argv) {
        LocalFree(argv);
    }
    return timeout;
}

} // namespace

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int) {
    const DWORD idleTimeoutMs = ParseIdleTimeoutFromArgs();

    BlackOverlay overlay;
    if (!overlay.initialize(hInstance)) {
        MessageBoxW(nullptr, L"Failed to create overlay window.", L"Black Screen Idle Watcher", MB_ICONERROR | MB_OK);
        return 1;
    }

    const UINT_PTR timerId = SetTimer(nullptr, 0, kTimerIntervalMs, nullptr);
    if (timerId == 0) {
        MessageBoxW(nullptr, L"Failed to start idle timer.", L"Black Screen Idle Watcher", MB_ICONERROR | MB_OK);
        return 1;
    }

    MSG msg{};
    while (GetMessageW(&msg, nullptr, 0, 0)) {
        if (msg.message == WM_TIMER && msg.wParam == timerId) {
            const DWORD idleMs = GetIdleMilliseconds();
            if (idleMs >= idleTimeoutMs && !overlay.isVisible()) {
                overlay.show();
            } else if (idleMs < idleTimeoutMs && overlay.isVisible()) {
                overlay.hide();
            }
            continue;
        }

        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    KillTimer(nullptr, timerId);
    overlay.hide();
    return 0;
}
