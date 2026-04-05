#pragma once

#include <memory>
#include <string>

class overlay {
public:
    overlay();
    ~overlay();

    bool init(unsigned int timeout, unsigned int idle, unsigned int active, std::string path);
    void show();
    void hide();
    bool isVisible() const;
    static unsigned int GetIdleMilliseconds();
    void setPopup(bool tf);
    bool popupActive() const;
    bool check_startup() const;
    void add_startup() const;
    void remove_startup() const;

    unsigned int IdleTimeoutMs;
    unsigned int idleIntervalMs;
    unsigned int activeIntervalMs;
    bool exit;
    bool pause;
    bool is_startup;
    std::string path;

private:
    void resizeToVirtualScreen();

    struct Impl;
    std::unique_ptr<Impl> var;
};
