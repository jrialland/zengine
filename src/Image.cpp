#include "Image.hpp"
#include "strutil.hpp"
#include "FileSystem.hpp"

#include "stb_image.h"
#include "stb_image_resize.h"
#include "stb_image_write.h"

Image::~Image()
{
    deleter(data);
}

Image::Image(int width, int height, int channels)
{
    assert(channels > 0 && channels <= 4);
    Image image;
    image.dimensions = Eigen::Vector2i(width, height);
    image.channels = channels;
    image.data = (uint8_t*) malloc(width * height * channels);
    image.deleter = [](void *data)
    { free(data); };
}

Image Image::load(const std::string &filename)
{
    return load(FileSystem::get_entry(filename)->read());
}

Image Image::load(const Blob &data)
{
    int width, height, channels;
    stbi_uc *img = stbi_load_from_memory(data.as<stbi_uc *>(), data.get_size(), &width, &height, &channels, 0);
    if (!img)
    {
        throw std::runtime_error("failed to load image");
    }
    Image image;
    image.dimensions = Eigen::Vector2i(width, height);
    image.channels = channels;
    image.data = img;
    image.deleter = [](void *data)
    { stbi_image_free(data); };
    return image;
}

struct Rgb
{
    uint8_t r, g, b;
    Rgb() = default;
    Rgb(int value)
    {
        r = value >> 16;
        g = value >> 8;
        b = value;
    }
};

struct Rgba
{
    uint8_t r, g, b, a;
    Rgba() = default;
    Rgba(int value)
    {
        r = value >> 24;
        g = value >> 16;
        b = value >> 8;
        a = value;
    }
};

#define CROPPED(TYPE)                                                                                                  \
    Eigen::Map<Eigen::Matrix<TYPE, Eigen::Dynamic, Eigen::Dynamic>> src((TYPE *)data, dimensions.y(), dimensions.x()); \
    src.block(y, x, height, width).swap(Eigen::Map<Eigen::Matrix<TYPE, Eigen::Dynamic, Eigen::Dynamic>>((TYPE *)cropped_image.data, height, width));

Image Image::cropped(int x, int y, int width, int height) const
{
    Image cropped_image(width, height, channels);
    if (channels == 1)
    {
        CROPPED(uint8_t)
    }
    else if (channels == 2)
    {
        CROPPED(uint16_t)
    }
    else if (channels == 3)
    {
        CROPPED(Rgb)
    }
    else if (channels == 4)
    {
        CROPPED(Rgba)
    }
    return cropped_image;
}

Image Image::resized(int width, int height) const
{
    Image resized_image(width, height, channels);
    stbir_resize_uint8((uint8_t *)data, dimensions.x(), dimensions.y(), 0, (uint8_t *)resized_image.data, width, height, 0, channels);
    return resized_image;
}

#define FLIP_V(TYPE)                                                                                                   \
    Eigen::Map<Eigen::Matrix<TYPE, Eigen::Dynamic, Eigen::Dynamic>> mat((TYPE *)data, dimensions.x(), dimensions.y()); \
    mat.colwise().reverseInPlace();

void Image::flip_horizontally()
{
    if (channels == 1)
    {
        FLIP_V(uint8_t)
    }
    else if (channels == 2)
    {
        FLIP_V(uint16_t)
    }
    else if (channels == 3)
    {
        FLIP_V(Rgb)
    }
    else if (channels == 4)
    {
        FLIP_V(Rgba)
    }
}

#define FLIP_H(TYPE)                                                                                                   \
    Eigen::Map<Eigen::Matrix<TYPE, Eigen::Dynamic, Eigen::Dynamic>> mat((TYPE *)data, dimensions.x(), dimensions.y()); \
    mat.rowwise().reverseInPlace();

void Image::flip_vertically()
{
    if (channels == 1)
    {
        FLIP_H(uint8_t)
    }
    else if (channels == 2)
    {
        FLIP_H(uint16_t)
    }
    else if (channels == 3)
    {
        FLIP_H(Rgb)
    }
    else if (channels == 4)
    {
        FLIP_H(Rgba)
    }
}

#define ROT_CW(TYPE)                                                                                                   \
    Eigen::Map<Eigen::Matrix<TYPE, Eigen::Dynamic, Eigen::Dynamic>> mat((TYPE *)data, dimensions.x(), dimensions.y()); \
    mat.transposeInPlace();

void Image::rotate_clockwise()
{
    if (channels == 1)
    {
        ROT_CW(uint8_t)
    }
    else if (channels == 2)
    {
        ROT_CW(uint16_t)
    }
    else if (channels == 3)
    {
        ROT_CW(Rgb)
    }
    else if (channels == 4)
    {
        ROT_CW(Rgba)
    }
    dimensions = Eigen::Vector2i(dimensions.y(), dimensions.x());
}

#define ROT_CCW(TYPE)                                                                                                  \
    Eigen::Map<Eigen::Matrix<TYPE, Eigen::Dynamic, Eigen::Dynamic>> mat((TYPE *)data, dimensions.x(), dimensions.y()); \
    mat.transposeInPlace();                                                                                            \
    mat.colwise().reverseInPlace();

void Image::rotate_counterclockwise()
{
    if (channels == 1)
    {
        ROT_CCW(uint8_t)
    }
    else if (channels == 2)
    {
        ROT_CCW(uint16_t)
    }
    else if (channels == 3)
    {
        ROT_CCW(Rgb)
    }
    else if (channels == 4)
    {
        ROT_CCW(Rgba)
    }
    dimensions = Eigen::Vector2i(dimensions.y(), dimensions.x());
}

int Image::get_channels() const
{
    return channels;
}

size_t Image::get_pixel_size() const
{
    return channels * sizeof(uint8_t);
}

size_t Image::get_size() const
{
    return dimensions.x() * dimensions.y() * get_pixel_size();
}

void* Image::get_data() const
{
    return data;
}

Eigen::Vector2i Image::get_dimensions() const
{
    return dimensions;
}

void Image::save(const std::string &filename) const
{
    // TODO
}