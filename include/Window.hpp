#pragma once
#include <string>
#include <Eigen/Dense>

#include "signal.hpp"

class Window {
    void *handle;
    public:
    Window(const std::string& title="", int width=800, int height=600, bool fullscreen=false);
    ~Window();
    util::signal<const Eigen::Vector2i &> resized;
    int render_loop(int argc=0, char **argv=nullptr);
    Eigen::Vector2i get_size() const;
};