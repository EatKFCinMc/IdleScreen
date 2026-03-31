#include "overlay.h"

#include <windows.h>
#include <shellapi.h>
#include <iostream>
#include <thread>
#include "resources.h"

#define WMTRAYMESSAGE (WM_USER + 100)
#define IDM_PAUSE (WM_USER + 110)
#define IDM_EXIT (WM_USER + 111)
#define IDM_1MIN (WM_USER + 112)
#define IDM_5MIN (WM_USER + 113)
#define IDM_10MIN (WM_USER + 114)
#define IDM_30MIN (WM_USER + 115)

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

            case WMTRAYMESSAGE:{
                if (lparam == WM_RBUTTONUP || lparam == WM_RBUTTONDOWN ||
                    lparam == WM_LBUTTONUP || lparam == WM_LBUTTONDOWN) {
                    self->setPopup(true);

                    POINT pt;
                    GetCursorPos(&pt);

                    SetForegroundWindow(hwnd);

                    HMENU hmenu = CreatePopupMenu();
                    UINT flag_pause = self->pause ? MF_CHECKED : MF_UNCHECKED;
                    UINT flag_1min = self->IdleTimeoutMs == 60000 ? MF_CHECKED : MF_UNCHECKED;
                    UINT flag_5min = self->IdleTimeoutMs == 300000 ? MF_CHECKED : MF_UNCHECKED;
                    UINT flag_10min = self->IdleTimeoutMs == 600000 ? MF_CHECKED : MF_UNCHECKED;
                    UINT flag_30min = self->IdleTimeoutMs == 1800000 ? MF_CHECKED : MF_UNCHECKED;
                    AppendMenuA(hmenu, MF_STRING | flag_1min, IDM_1MIN, "1 Minutes");
                    AppendMenuA(hmenu, MF_STRING | flag_5min, IDM_5MIN, "5 Minutes");
                    AppendMenuA(hmenu, MF_STRING | flag_10min, IDM_10MIN, "10 Minutes");
                    AppendMenuA(hmenu, MF_STRING | flag_30min, IDM_30MIN, "30 Minutes");
			        AppendMenuA(hmenu, MF_SEPARATOR, 0, nullptr);
                    AppendMenuA(hmenu, MF_STRING | flag_pause, IDM_PAUSE, "Pause");
                    AppendMenuA(hmenu, MF_STRING, IDM_EXIT, "Exit");

                    TrackPopupMenu(hmenu, TPM_RIGHTBUTTON | TPM_NONOTIFY, pt.x, pt.y, 0, hwnd, nullptr);

                    DestroyMenu(hmenu);
                    PostMessage(hwnd, WM_NULL, 0, 0);

                    self->setPopup(false);
                }
                return 0;
            }

            case WM_COMMAND:
                switch (wparam) {
                    case IDM_EXIT:
                        if (self)
                            self->exit = true;
                        return 0;
                    case IDM_PAUSE:
                        if (self)
                            self->pause = !self->pause;
                        return 0;
                    case IDM_1MIN:
                        if (self)
                            self->IdleTimeoutMs = 60000;
                        return 0;
                    case IDM_5MIN:
                        if (self)
                            self->IdleTimeoutMs = 300000;
                        return 0;
                    case IDM_10MIN:
                        if (self)
                            self->IdleTimeoutMs = 600000;
                        return 0;
                    case IDM_30MIN:
                        if (self)
                            self->IdleTimeoutMs = 1800000;
                        return 0;
                    default:
                        return DefWindowProc(hwnd, msg, wparam, lparam);
                }

            case WM_DESTROY:
                PostQuitMessage(0);
                return 0;
            case WM_SETCURSOR:
                if (self && self->popupActive()) {
                    return DefWindowProc(hwnd, msg, wparam, lparam);
                }
                SetCursor(nullptr);
                return TRUE;

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
    bool m_popupActive;
};

overlay::overlay() {
    var = std::make_unique<Impl>();
    var->m_instance = nullptr;
    var->m_hwnd = nullptr;
    var->m_visible = false;
    var->m_cursorHidden = false;
    var->m_popupActive = false;
    IdleTimeoutMs = 6000;
    idleIntervalMs = 100;
    activeIntervalMs = 1000;
    exit = false;
    pause = false;
}

overlay::~overlay() {
    if (var->m_visible) hide();
    if (var->m_hwnd) DestroyWindow(var->m_hwnd);
    if (var->m_instance) UnregisterClassW(kWindowClassName, var->m_instance);
    Shell_NotifyIcon(NIM_DELETE, &var->nid);
}

bool overlay::init(const unsigned int timeout = 6000,
    const unsigned int idle = 100, const unsigned int active = 1000) {

    IdleTimeoutMs = timeout;
    idleIntervalMs = idle;
    activeIntervalMs = active;

    SetProcessDPIAware();
    HANDLE hEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);

    std::thread([this, hEvent]() {
        var->m_instance = GetModuleHandle(nullptr);

        WNDCLASSEXW wc{};
        wc.cbSize = sizeof(WNDCLASSEXW);
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = WindowProc;
        wc.hInstance = var->m_instance;
        // wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wc.hCursor = nullptr;
        wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
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
        var->nid.uVersion = 0;
        var->nid.uCallbackMessage = WMTRAYMESSAGE;
        var->nid.hIcon = (HICON)LoadImageW(
            GetModuleHandleW(nullptr),
            MAKEINTRESOURCEW(IDI_ICON),
            IMAGE_ICON,
            16, 16,
            LR_DEFAULTCOLOR
        );
        var->nid.uID = 239; // random number
        wcscpy_s(var->nid.szTip, L"Idle Screen");
        var->nid.uFlags = NIF_TIP | NIF_ICON | NIF_MESSAGE;

        resizeToVirtualScreen();
        Shell_NotifyIcon(NIM_ADD, &var->nid);
        SetEvent(hEvent);

        MSG msg;
        while (GetMessage(&msg, nullptr, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        return true;
    }).detach();

    WaitForSingleObject(hEvent, INFINITE);
    CloseHandle(hEvent);
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
    var->m_visible = true;

    if (var->m_cursorHidden)
        return;
    while (ShowCursor(FALSE) >= 0) {}
    var->m_cursorHidden = true;
}

void overlay::hide() {
    if (!var->m_hwnd) {
        return;
    }
    ShowWindow(var->m_hwnd, SW_HIDE);
    var->m_visible = false;

    if (!var->m_cursorHidden)
        return;
    while (ShowCursor(TRUE) < 0) {}
    var->m_cursorHidden = false;
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

void overlay::setPopup(bool tf) {
    var->m_popupActive = tf;
}

bool overlay::popupActive() {
    return var->m_popupActive;
}