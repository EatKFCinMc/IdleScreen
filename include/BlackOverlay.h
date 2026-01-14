#pragma once

#include <windows.h>

class BlackOverlay {
public:
    BlackOverlay();
    ~BlackOverlay();

    bool initialize(HINSTANCE instance);
    void show();
    void hide();
    bool isVisible() const { return m_visible; }

    HWND handle() const { return m_hwnd; }

private:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
    void resizeToVirtualScreen();
    void hideCursor();
    void showCursor();

    HINSTANCE m_instance{nullptr};
    HWND m_hwnd{nullptr};
    bool m_visible{false};
    bool m_cursorHidden{false};
};
