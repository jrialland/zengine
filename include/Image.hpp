#pragma once

#include <string>
#include <Eigen/Dense>

#include "Blob.hpp"

class Image {
    int width;
    int height;
    int channels;
    Blob blob;
    friend class Canvas;
public:
    Image(int width, int height, int channels, void *data=nullptr);
    Image(const std::string &filename);
    Image(const Blob &data);
    Image(const Image &image);
    Image(Image &&image);

    Image croped(int x, int y, int width, int height);
    Image resized(int width, int height);

    void save(const std::string &filename);
    
    Eigen::Vector2i size() const;
    int get_channels() const;

    static Image load(const std::string &filename);
};