#pragma once

#include <memory>

class BlackOverlay {
public:
    BlackOverlay();
    ~BlackOverlay();

    bool initialize();
    void show();
    void hide();
    bool isVisible() const;
    static unsigned int GetIdleMilliseconds();


private:
    void resizeToVirtualScreen();
    void hideCursor();
    void showCursor();

    struct Impl;
    std::unique_ptr<Impl> impl;
};
