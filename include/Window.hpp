#pragma once
#include <string>

class Window {
    void *handle;
    public:
    Window(const std::string& title="", int width=800, int height=600, bool fullscreen=false);
    ~Window();
    int render_loop(int argc=0, char **argv=nullptr);
};