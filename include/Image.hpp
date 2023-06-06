#pragma once

#include <string>
#include <Eigen/Dense>

#include "Blob.hpp"

enum class ImageType {
    RGB_u24, // 3 channels, 24bits rgb, no padding (GL_RGB)
    RGBA_u32, // 4 channels, 32bits rgba, no padding (GL_RGBA)
    GRAYSCALE_f32, // 1 channel, 32bits float, no padding (GL_RED)
};

class Image {
    int width;
    int height;
    ImageType type;
    Blob data;
    Image(int width, int height, ImageType type, void* data, size_t len, std::function<void(void *)> deleter = [](void *) {});
public:
    static Image create(int width, int height, ImageType type);
    static Image load(const std::string &filename, ImageType type);
    static Image load(const Blob &data, ImageType type);
    
    Image(const Image &other) = delete;
    Image(Image &&other) = default;
    
    Image cropped(int x, int y, int width, int height) const;
    Image resized(int width, int height) const;

    void flip_vertically();
    
    void flip_horizontally();
    
    void rotate_90();

    void save(const std::string &filename) const;
    
    Image convert(ImageType type) const;
    
    /**
     * @brief size of a single pixel in bytes
     * 
     * @return size_t 
     */
    size_t get_pixel_size() const;

    /**
     * @brief size of the image in bytes
     * 
     * @return size_t 
     */
    size_t get_size() const;

    Eigen::Vector2i get_dimensions() const;
    
};