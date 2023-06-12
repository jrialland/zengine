#pragma once

#include <string>
#include <Eigen/Dense>
#include <functional>

#include "Blob.hpp"

/**
 * @brief An image is a 2D array of pixels, in cpu memory
 *
 */
class Image
{
    uint8_t *data;
    Eigen::Vector2i dimensions;
    int channels;
    std::function<void(void *)> deleter;
    Image() = default;
public:
    
    Image(int width, int height, int channels);

    /**
     * @brief load an image from a file, using the Filesystem facilities
     *
     * @param filename
     * @param type
     * @return Image
     */
    static Image load(const std::string &filename);

    /**
     * @brief load an image from a blob that contains the data, in its original format (png, jpg, etc)
     *
     * @param data
     * @param type
     * @return Image
     */
    static Image load(const Blob &data);

    Image(const Image &other) = delete;

    /**
     * @brief move constructor
     *
     * @param other
     */
    Image(Image &&other) = default;

    ~Image();

    /**
     * @brief copies a portion of the image into a new image
     *
     * @param x
     * @param y
     * @param width
     * @param height
     * @return Image
     */
    Image cropped(int x, int y, int width, int height) const;

    /**
     * @brief shrinks or expands the image to the given dimensions
     *
     * @param width
     * @param height
     * @return Image
     */
    Image resized(int width, int height) const;

    /**
     * @brief flips the image vertically (implementation uses Eigen)
     *
     */
    void flip_vertically();

    /**
     * @brief flips the image horizontally (implementation uses Eigen)
     *
     */
    void flip_horizontally();

    /**
     * @brief rotates the image 90 degrees clockwise (implementation uses Eigen matrix transposition)
     *
     */
    void rotate_clockwise();

    /**
     * @brief rotates the image 90 degrees counterclockwise (implementation uses Eigen)
     *
     */
    void rotate_counterclockwise();

    /**
     * @brief saves the image to a file, using the Filesystem facilities
     *
     * @param filename
     */
    void save(const std::string &filename) const;

    /**
     * @brief number of channels in the image (1 for grayscale, 3 for RGB, 4 for RGBA)
     *
     * @return int
    */
    int get_channels() const;

    size_t get_pixel_size() const;

    /**
     * @brief size of the image in bytes
     *
     * @return size_t
     */
    size_t get_size() const;

    /**
     * @brief width and height of the image
     *
     * @return Eigen::Vector2i
     */
    Eigen::Vector2i get_dimensions() const;

    /**
     * @brief the raw pixel data
     *
     * @return void*
     */
    void *get_data() const;
};