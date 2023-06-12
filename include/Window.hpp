#pragma once
#include <string>
#include <Eigen/Dense>

#include "signal.hpp"

class Window {
    void *handle;
    friend class Application;
    public:
    Window(const std::string& title="", int width=800, int height=600, bool fullscreen=false);
    ~Window();
    util::signal<const Eigen::Vector2i &> resized;
    Eigen::Vector2i get_dimensions() const;
};