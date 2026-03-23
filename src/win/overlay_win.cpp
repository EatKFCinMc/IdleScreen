#include "overlay.h"

#include <windows.h>
#include "resources.h"

namespace {
    const wchar_t kWindowClassName[] = L"IdleScreen";

    LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
        if (msg == WM_NCCREATE) {
            auto createStruct = reinterpret_cast<LPCREATESTRUCT>(lparam);
            auto self = reinterpret_cast<overlay*>(createStruct->lpCreateParams);
            SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
        }

        auto* self = reinterpret_cast<overlay*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

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
            case WM_TRAYMESSAGE:

            default:
                return DefWindowProc(hwnd, msg, wparam, lparam);
        }
    }
}

struct overlay::Impl {
    HINSTANCE m_instance;
    HWND m_hwnd;
    bool m_visible;
    bool m_cursorHidden;
    NOTIFYICONDATA nid;
};

overlay::overlay() {
    var = std::make_unique<Impl>();
    var->m_instance = nullptr;
    var->m_hwnd = nullptr;
    var->m_visible = false;
    var->m_cursorHidden = false;
    kDefaultIdleTimeoutMs = 6000;
    idleIntervalMs = 100;
    activeIntervalMs = 1000;
    exit = false;
}

overlay::~overlay() {
    if (var->m_visible) hide();
    if (var->m_hwnd) DestroyWindow(var->m_hwnd);
    if (var->m_instance) UnregisterClassW(kWindowClassName, var->m_instance);
}

bool overlay::init(unsigned int timeout = 6000, unsigned int idle = 100,
    unsigned int active = 1000) {

    kDefaultIdleTimeoutMs = timeout;
    idleIntervalMs = idle;
    activeIntervalMs = active;

    var->m_instance = GetModuleHandle(nullptr);

    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = var->m_instance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
    wc.lpszClassName = kWindowClassName;

    if (!RegisterClassExW(&wc))
        return false;

    var->m_hwnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
        kWindowClassName,
        L"",
        WS_POPUP,
        0, 0, 0, 0,
        nullptr,
        nullptr,
        var->m_instance,
        this);

    if (!var->m_hwnd)
        return false;

    var->nid.cbSize = sizeof(NOTIFYICONDATA);
    var->nid.hWnd = var->m_hwnd;
    var->nid.uVersion = NOTIFYICON_VERSION;
    var->nid.uCallbackMessage = WM_TRAYMESSAGE;
    var->nid.hIcon = (HICON)LoadImageW(
        GetModuleHandleW(nullptr),
        MAKEINTRESOURCEW(IDI_ICON),
        IMAGE_ICON,
        16, 16,
        LR_DEFAULTCOLOR
    );
    var->nid.uID = 239; // random makeup number
    wcscpy_s(var->nid.szTip, L"Idle Screen");
    var->nid.uFlags = NIF_TIP | NIF_ICON | NIF_MESSAGE;

    resizeToVirtualScreen();
    ShowTrayIcon();
    return true;
}

void overlay::resizeToVirtualScreen() {
    const int x = GetSystemMetrics(SM_XVIRTUALSCREEN);
    const int y = GetSystemMetrics(SM_YVIRTUALSCREEN);
    const int width = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    const int height = GetSystemMetrics(SM_CYVIRTUALSCREEN);

    SetWindowPos(var->m_hwnd, HWND_TOPMOST, x, y, width, height,
                 SWP_NOACTIVATE | SWP_NOOWNERZORDER);
}

void overlay::show() {
    if (!var->m_hwnd) {
        return;
    }
    resizeToVirtualScreen();
    ShowWindow(var->m_hwnd, SW_SHOW);
    UpdateWindow(var->m_hwnd);
    SetForegroundWindow(var->m_hwnd);
    hideCursor();
    var->m_visible = true;
}

void overlay::hide() {
    if (!var->m_hwnd) {
        return;
    }
    ShowWindow(var->m_hwnd, SW_HIDE);
    showCursor();
    var->m_visible = false;
}

bool overlay::isVisible() const {
    return var->m_visible;
}

unsigned int overlay::GetIdleMilliseconds() {
    LASTINPUTINFO info{};
    info.cbSize = sizeof(LASTINPUTINFO);
    if (!GetLastInputInfo(&info)) {
        return 0;
    }
    const DWORD now = GetTickCount();
    return now - info.dwTime;
}

void overlay::hideCursor() {
    if (var->m_cursorHidden)
        return;
    while (ShowCursor(FALSE) >= 0) {}
    var->m_cursorHidden = true;
}

void overlay::showCursor() {
    if (!var->m_cursorHidden)
        return;
    while (ShowCursor(TRUE) < 0) {}
    var->m_cursorHidden = false;
}

void overlay::ShowTrayIcon() {
    Shell_NotifyIcon(NIM_ADD, &var->nid);
}

void overlay::RemoveTrayIcon() {
    Shell_NotifyIcon(NIM_DELETE, &var->nid);
}