#pragma once

#include <memory>

class overlay {
public:
    overlay();
    ~overlay();

    bool init(unsigned int timeout, unsigned int idle, unsigned int active);
    void show();
    void hide();
    bool isVisible() const;
    static unsigned int GetIdleMilliseconds();
    void setPopup(bool tf);
    bool popupActive();

    unsigned int kDefaultIdleTimeoutMs;
    unsigned int idleIntervalMs;
    unsigned int activeIntervalMs;
    bool exit;
    bool pause;

private:
    void resizeToVirtualScreen();

    struct Impl;
    std::unique_ptr<Impl> var;
};
