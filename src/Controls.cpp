#include "Controls.hpp"

util::signal<int/*key*/, int/*scancode*/, int/*modifiers*/> Controls::key_down;
util::signal<int/*key*/, int/*scancode*/, int/*modifiers*/> Controls::key_up;
util::signal<int, int> Controls::mouse_down;
util::signal<int, int> Controls::mouse_up;
util::signal<double, double> Controls::mouse_move;
util::signal<double, double> Controls::mouse_scroll;