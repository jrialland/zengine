#pragma once

#include "Window.hpp"
#include <memory>

class RenderingSystem;

class Application {

    Window* window;

    RenderingSystem* rendering_system;

public:
    Application();
    virtual ~Application();
    int run(int argc, char **argv);

    RenderingSystem* get_rendering_system();

    Window* get_window();

protected:

    virtual void init();
};