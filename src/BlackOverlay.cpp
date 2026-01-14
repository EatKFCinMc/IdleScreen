#include "BlackOverlay.h"

namespace {
const wchar_t kWindowClassName[] = L"BlackScreenIdleWatcherOverlay";
}

BlackOverlay::BlackOverlay() = default;

BlackOverlay::~BlackOverlay() {
    if (m_visible) {
        hide();
    }
    if (m_hwnd) {
        DestroyWindow(m_hwnd);
    }
    if (m_instance) {
        UnregisterClassW(kWindowClassName, m_instance);
    }
}

bool BlackOverlay::initialize(HINSTANCE instance) {
    m_instance = instance;

    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = BlackOverlay::WindowProc;
    wc.hInstance = m_instance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
    wc.lpszClassName = kWindowClassName;

    if (!RegisterClassExW(&wc)) {
        return false;
    }

    m_hwnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
        kWindowClassName,
        L"",
        WS_POPUP,
        0, 0, 0, 0,
        nullptr,
        nullptr,
        m_instance,
        this);

    if (!m_hwnd) {
        return false;
    }

    resizeToVirtualScreen();
    return true;
}

void BlackOverlay::resizeToVirtualScreen() {
    const int x = GetSystemMetrics(SM_XVIRTUALSCREEN);
    const int y = GetSystemMetrics(SM_YVIRTUALSCREEN);
    const int width = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    const int height = GetSystemMetrics(SM_CYVIRTUALSCREEN);

    SetWindowPos(m_hwnd, HWND_TOPMOST, x, y, width, height,
                 SWP_NOACTIVATE | SWP_NOOWNERZORDER);
}

void BlackOverlay::show() {
    if (!m_hwnd) {
        return;
    }
    resizeToVirtualScreen();
    ShowWindow(m_hwnd, SW_SHOW);
    UpdateWindow(m_hwnd);
    SetForegroundWindow(m_hwnd);
    hideCursor();
    m_visible = true;
}

void BlackOverlay::hide() {
    if (!m_hwnd) {
        return;
    }
    ShowWindow(m_hwnd, SW_HIDE);
    showCursor();
    m_visible = false;
}

LRESULT CALLBACK BlackOverlay::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
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
        // Any input hides the overlay; internal state stays in sync.
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

void BlackOverlay::hideCursor() {
    if (m_cursorHidden) {
        return;
    }
    // Balance ShowCursor calls to ensure the cursor is actually hidden.
    while (ShowCursor(FALSE) >= 0) {
    }
    m_cursorHidden = true;
}

void BlackOverlay::showCursor() {
    if (!m_cursorHidden) {
        return;
    }
    while (ShowCursor(TRUE) < 0) {
    }
    m_cursorHidden = false;
}
