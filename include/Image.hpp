#pragma once

#include <string>
#include <Eigen/Dense>

class Image {
    Blob blob;

public:
    Image(int width, int height, int channels, void *data=nullptr);
    Image(const std::string &filename);
    Image(const Image &image);
    Image(Image &&image);
    ~Image();

    Image croped(int x, int y, int width, int height);
    Image resized(int width, int height);

    void save(const std::string &filename);
    
    int width() const;
    int height() const;
    Eigen::Vector2i size() const;
    int channels() const;
};