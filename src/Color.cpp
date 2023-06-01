#include "Color.hpp"

Eigen::Vector4f Color::rgba32_to_floats(uint32_t color) {
    int r = (color >> 24) & 0xFF;
    int g = (color >> 16) & 0xFF;
    int b = (color >> 8) & 0xFF;
    int a = (color >> 0) & 0xFF;
    return Eigen::Vector4f(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
}

uint32_t Color::floats_to_rgba32(const Eigen::Vector4f &color) {
    return floats_to_rgba32(color.x(), color.y(), color.z(), color.w());
}

uint32_t Color::floats_to_rgba32(float r, float g, float b, float a) {
    int _r = (int)(r * 255.0f);
    int _g = (int)(g * 255.0f);
    int _b = (int)(b * 255.0f);
    int _a = (int)(a * 255.0f);
    return (_r << 24) | (_g << 16) | (_b << 8) | (_a << 0);
}