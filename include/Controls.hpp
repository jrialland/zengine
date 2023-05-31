#pragma once
#include "signal.hpp"

class Controls {
    public:
    static util::signal<int/*key*/, int/*scancode*/, int/*modifiers*/> key_down;
    static util::signal<int/*key*/, int/*scancode*/, int/*modifiers*/> key_up;
    static util::signal<int, int> mouse_down;
    static util::signal<int, int> mouse_up;
    static util::signal<double, double> mouse_move;
    static util::signal<double, double> mouse_scroll;
};