#include "overlay.h"

namespace {
    const wchar_t kWindowClassName[] = L"IdleScreen";
}

struct overlay::Impl {
};

overlay::overlay() {
}

overlay::~overlay() {
}

bool overlay::init() {
    return true;
}

void overlay::resizeToVirtualScreen() {
}

void overlay::show() {
}

void overlay::hide() {
}

bool overlay::isVisible() const {
}

unsigned int BlackOverlay::GetIdleMilliseconds() {
}

void BlackOverlay::hideCursor() {
}

void BlackOverlay::showCursor() {
}
