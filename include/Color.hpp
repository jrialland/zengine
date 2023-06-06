#pragma once

#include <Eigen/Dense>

class Color {
public:
    static Eigen::Vector4f hsl_to_rgb(const Eigen::Vector3f &hsl);
    static Eigen::Vector3f rgb_to_hsl(const Eigen::Vector4f &rgb);
    
    static Eigen::Vector4f rgba32_to_floats(uint32_t);
    static uint32_t floats_to_rgba32(const Eigen::Vector4f &);
    static uint32_t floats_to_rgba32(float r, float g, float b, float a=1.0f);
    static uint32_t floats_to_rgb24(float r, float g, float b);
    static Eigen::Vector4f for_name(const std::string &name); 
};