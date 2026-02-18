#include "overlay.h"

#include <windows.h>

namespace {
    const wchar_t kWindowClassName[] = L"IdleScreen";
    LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
        if (msg == WM_NCCREATE) {
            auto createStruct = reinterpret_cast<LPCREATESTRUCT>(lparam);
            auto self = reinterpret_cast<BlackOverlay*>(createStruct->lpCreateParams);
            SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
        }

        auto* self = reinterpret_cast<BlackOverlay*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

        switch (msg) {
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_XBUTTONDOWN:
        case WM_MOUSEWHEEL:
        case WM_MOUSEHWHEEL:
            if (self) {
                self->hide();
            } else {
                ShowWindow(hwnd, SW_HIDE);
            }
            return 0;
        default:
            return DefWindowProc(hwnd, msg, wparam, lparam);
        }
    }
}

struct BlackOverlay::Impl {
    HINSTANCE m_instance;
    HWND m_hwnd;
    bool m_visible;
    bool m_cursorHidden;
};

BlackOverlay::BlackOverlay() {
    impl = std::make_unique<Impl>();
    impl->m_instance = nullptr;
    impl->m_hwnd = nullptr;
    impl->m_visible = false;
    impl->m_cursorHidden = false;
}

BlackOverlay::~BlackOverlay() {
    if (impl->m_visible) hide();
    if (impl->m_hwnd) DestroyWindow(impl->m_hwnd);
    if (impl->m_instance) UnregisterClassW(kWindowClassName, impl->m_instance);
}

bool BlackOverlay::initialize() {
    impl->m_instance = GetModuleHandle(nullptr);

    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = impl->m_instance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
    wc.lpszClassName = kWindowClassName;

    if (!RegisterClassExW(&wc))
        return false;

    impl->m_hwnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
        kWindowClassName,
        L"",
        WS_POPUP,
        0, 0, 0, 0,
        nullptr,
        nullptr,
        impl->m_instance,
        this);

    if (!impl->m_hwnd)
        return false;

    resizeToVirtualScreen();
    return true;
}

void BlackOverlay::resizeToVirtualScreen() {
    const int x = GetSystemMetrics(SM_XVIRTUALSCREEN);
    const int y = GetSystemMetrics(SM_YVIRTUALSCREEN);
    const int width = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    const int height = GetSystemMetrics(SM_CYVIRTUALSCREEN);

    SetWindowPos(impl->m_hwnd, HWND_TOPMOST, x, y, width, height,
                 SWP_NOACTIVATE | SWP_NOOWNERZORDER);
}

void BlackOverlay::show() {
    if (!impl->m_hwnd) {
        return;
    }
    resizeToVirtualScreen();
    ShowWindow(impl->m_hwnd, SW_SHOW);
    UpdateWindow(impl->m_hwnd);
    SetForegroundWindow(impl->m_hwnd);
    hideCursor();
    impl->m_visible = true;
}

void BlackOverlay::hide() {
    if (!impl->m_hwnd) {
        return;
    }
    ShowWindow(impl->m_hwnd, SW_HIDE);
    showCursor();
    impl->m_visible = false;
}

bool BlackOverlay::isVisible() const {
    return impl->m_visible;
}

unsigned int BlackOverlay::GetIdleMilliseconds() {
    LASTINPUTINFO info{};
    info.cbSize = sizeof(LASTINPUTINFO);
    if (!GetLastInputInfo(&info)) {
        return 0;
    }
    const DWORD now = GetTickCount();
    return now - info.dwTime;
}

void BlackOverlay::hideCursor() {
    if (impl->m_cursorHidden)
        return;
    while (ShowCursor(FALSE) >= 0) {}
    impl->m_cursorHidden = true;
}

void BlackOverlay::showCursor() {
    if (!impl->m_cursorHidden)
        return;
    while (ShowCursor(TRUE) < 0) {}
    impl->m_cursorHidden = false;
}
