#pragma once

#include <memory>

class overlay {
public:
    overlay();
    ~overlay();

    bool init();
    void show();
    void hide();
    bool isVisible() const;
    static unsigned int GetIdleMilliseconds();


private:
    void resizeToVirtualScreen();
    void hideCursor();
    void showCursor();

    struct Impl;
    std::unique_ptr<Impl> var;
};
