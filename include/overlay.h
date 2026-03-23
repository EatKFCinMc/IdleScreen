#pragma once

#include <memory>
#include <windows.h>
#include <shellapi.h>


#define WM_TRAYMESSAGE (WM_USER + 1)

class overlay {
public:
    overlay();
    ~overlay();

    //overlay.h
    bool init(unsigned int timeout, unsigned int idle, unsigned int active);
    void show();
    void hide();
    bool isVisible() const;
    static unsigned int GetIdleMilliseconds();

    //tray.h
    void ShowTrayIcon();
    void RemoveTrayIcon();

    unsigned int kDefaultIdleTimeoutMs;
    unsigned int idleIntervalMs;
    unsigned int activeIntervalMs;
    bool exit;

private:
    void resizeToVirtualScreen();
    void hideCursor();
    void showCursor();

    struct Impl;
    std::unique_ptr<Impl> var;
};
