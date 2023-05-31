#pragma once

#include "Window.hpp"
#include "RenderingSystem.hpp"

class Application {

    Window window;

    RenderingSystem rendering_system;

public:
    Application();
    ~Application();

    int run(int argc, char **argv);

    RenderingSystem &get_rendering_system();

    Window &get_window();

protected:

    virtual void init();
};