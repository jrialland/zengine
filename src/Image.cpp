#include "Image.hpp"

#include <cstring>

#include "strutil.hpp"
#include "FileSystem.hpp"

#include "stb_image.h"
#include "stb_image_resize.h"
#include "stb_image_write.h"

Image::Image(int width_, int height_, ImageType type_, void *data_, size_t len, std::function<void(void *)> deleter_) : width(width_), height(height_), type(type_), data(data_, len, deleter_) {}

Image Image::create(int width, int height, ImageType type)
{
    size_t len;
    switch (type)
    {
    case ImageType::RGB_u24:
        len = width * height * 3 * sizeof(uint8_t);
        break;
    case ImageType::RGBA_u32:
        len = width * height * 4 * sizeof(uint8_t);
        break;
    case ImageType::GRAYSCALE_f32:
        len = width * height * sizeof(float);
        break;
    }
    return Image(width, height, type, malloc(len), len, [](void *ptr)
                 { free(ptr); });
}

Image Image::load(const std::string &filename, ImageType type)
{
    Blob data = FileSystem::get_entry(filename)->read();
    return load(data, type);
}

Image Image::load(const Blob &data, ImageType type)
{
    int width, height, channels;
    int desired_channels;
    switch (type)
    {
    case ImageType::RGB_u24:
        desired_channels = 3;
        break;
    case ImageType::RGBA_u32:
        desired_channels = 4;
        break;
    case ImageType::GRAYSCALE_f32:
        desired_channels = 1;
        break;
    }

    stbi_uc *img = stbi_load_from_memory(data.as<stbi_uc *>(), data.get_size(), &width, &height, &channels, desired_channels);

    if (!img)
    {
        throw std::runtime_error("Failed to load image: " + std::string(stbi_failure_reason()));
    }

    size_t len;
    switch (type)
    {
    case ImageType::RGB_u24:
        len = width * height * 3 * sizeof(uint8_t);
        break;
    case ImageType::RGBA_u32:
        len = width * height * 4 * sizeof(uint8_t);
        break;
    case ImageType::GRAYSCALE_f32:
        len = width * height * sizeof(float);
        float *float_data = (float *)malloc(len);
        for (int i = 0; i < width * height; i++)
        {
            float_data[i] = img[i] / 255.0f;
        }
        stbi_image_free(img);
        return Image(width, height, type, float_data, len, [](void *ptr)
                     { free(ptr); });
        break;
    }

    return Image(width, height, type, img, len, [](void *ptr)
                 { stbi_image_free(ptr); });
}

Image Image::cropped(int x, int y, int width, int height) const
{
    if (x < 0 || y < 0 || x + width > this->width || y + height > this->height)
    {
        throw std::runtime_error("Invalid crop size");
    }
    // create target image
    Image img = Image::create(width, height, type);

    uint8_t *src = data.as<uint8_t *>() + (y * this->width + x) * get_pixel_size();
    int src_stride = this->width * img.get_pixel_size();

    uint8_t *dst = img.data.as<uint8_t *>();
    int dst_stride = width * img.get_pixel_size();

    for (int j = y; j < y + height; j++)
    {
        memcpy(dst, src, dst_stride);
        src += src_stride;
        dst += dst_stride;
    }

    return img;
}

Image Image::resized(int width, int height) const
{
    Image resized = Image::create(width, height, type);
    switch (type)
    {
    case ImageType::RGB_u24:
        stbir_resize_uint8((uint8_t *)data.get_ptr(), this->width, this->height, 0, resized.data.as<uint8_t *>(), width, height, 0, 3);
        break;
    case ImageType::RGBA_u32:
        stbir_resize_uint8((uint8_t *)data.get_ptr(), this->width, this->height, 0, resized.data.as<uint8_t *>(), width, height, 0, 4);
        break;
    case ImageType::GRAYSCALE_f32:
        stbir_resize_float((float *)data.get_ptr(), this->width, this->height, 0, resized.data.as<float *>(), width, height, 0, 1);
        break;
    }
    return resized;
}

Image Image::convert(ImageType type) const
{
    Image converted = Image::create(width, height, type);
    const int pixel_size = get_pixel_size();

    if (type == this->type)
    {
        memcpy(converted.data.get_ptr(), data.get_ptr(), data.get_size());
    }
    else if (this->type == ImageType::GRAYSCALE_f32)
    {

        std::function<void(float, uint8_t *)> convert_pixel;

        if (type == ImageType::RGB_u24)
        {
            convert_pixel = [](float value, uint8_t *data)
            {
                data[0] = value * 255.0f;
                data[1] = value * 255.0f;
                data[2] = value * 255.0f;
            };
        }
        else if (type == ImageType::RGBA_u32)
        {
            convert_pixel = [](float value, uint8_t *data)
            {
                data[0] = value * 255.0f;
                data[1] = value * 255.0f;
                data[2] = value * 255.0f;
                data[3] = 255;
            };
        }
        float *src = data.as<float *>();
        for (int i = 0; i < width * height; i++)
        {
            uint8_t *dst = converted.data.as<uint8_t *>() + i * pixel_size;
            convert_pixel(src[i], dst);
        }
    }
    else
    {
        if (type == ImageType::GRAYSCALE_f32)
        {
            std::function<float(uint8_t *)> convert_pixel = [](uint8_t *data)
            {
                return (data[0] + data[1] + data[2]) / 3.0f / 255.0f;
            };
            float *dst = converted.data.as<float *>();
            for (int i = 0; i < width * height; i++)
            {
                uint8_t *src = data.as<uint8_t *>() + i * pixel_size;
                dst[i] = convert_pixel(src);
            }
        }
        else
        {
            std::function<void(uint8_t *, uint8_t *)> convert_pixel;
            if (type == ImageType::RGBA_u32)
            {
                // conversion 3 channels -> 4 channels
                convert_pixel = [](uint8_t *src, uint8_t *dst)
                {
                    dst[0] = src[0];
                    dst[1] = src[1];
                    dst[2] = src[2];
                    dst[3] = 255;
                };
            }
            else
            {
                // conversion 4 channels -> 3 channels (multiplication by alpha)
                convert_pixel = [](uint8_t *src, uint8_t *dst)
                {
                    dst[0] = src[0] * src[3] / 255;
                    dst[1] = src[1] * src[3] / 255;
                    dst[2] = src[2] * src[3] / 255;
                };
            }

            int out_pixel_size = converted.get_pixel_size();
            for (int i = 0; i < width * height; i++)
            {
                uint8_t *src = data.as<uint8_t *>() + i * pixel_size;
                uint8_t *dst = converted.data.as<uint8_t *>() + i * out_pixel_size;
                convert_pixel(src, dst);
            }
        }
    }
    return converted;
}

void Image::save(const std::string &filename) const
{
    if (type != ImageType::RGBA_u32)
    {
        convert(ImageType::RGBA_u32).save(filename);
        return;
    }
    std::string ext = FileSystem::get_extension(filename);
    if (ext == "png")
    {
        stbi_write_png(filename.c_str(), width, height, 4, data.as<uint8_t *>(), width * 4);
    }
    else if (ext == "jpg" || ext == "jpeg")
    {
        stbi_write_jpg(filename.c_str(), width, height, 4, data.as<uint8_t *>(), 100);
    }
    else if (ext == "bmp")
    {
        stbi_write_bmp(filename.c_str(), width, height, 4, data.as<uint8_t *>());
    }
    else if (ext == "tga")
    {
        stbi_write_tga(filename.c_str(), width, height, 4, data.as<uint8_t *>());
    }
    else
    {
        throw std::runtime_error("Unsupported image format: " + ext);
    }
}

struct PixelRGB
{
    uint8_t r, g, b;
    PixelRGB(int v = 0)
    {
        r = v >> 16;
        g = v >> 8;
        b = v;
    }
};

struct PixelRGBA
{
    uint8_t r, g, b, a;
    PixelRGBA(int v = 0)
    {
        r = v >> 24;
        g = v >> 16;
        b = v >> 8;
        a = v;
    }
};

void Image::flip_horizontally()
{
#define _IMAGE_DO_FLIP_H(type)                                                                                                  \
    {                                                                                                                           \
        Eigen::Map<Eigen::Matrix<type, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>> mat(data.as<type *>(), height, width); \
        mat.rowwise().reverseInPlace();                                                                                         \
    }
    switch (type)
    {
    case ImageType::RGB_u24:
        _IMAGE_DO_FLIP_H(PixelRGB);
        break;
    case ImageType::RGBA_u32:
        _IMAGE_DO_FLIP_H(PixelRGBA);
        break;
    case ImageType::GRAYSCALE_f32:
        _IMAGE_DO_FLIP_H(float);
        break;
    }
#undef _IMAGE_DO_FLIP_H
}

void Image::flip_vertically()
{
#define _IMAGE_DO_FLIP_V(type)                                                                                                  \
    {                                                                                                                           \
        Eigen::Map<Eigen::Matrix<type, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>> mat(data.as<type *>(), height, width); \
        mat.colwise().reverseInPlace();                                                                                         \
    }
    switch (type)
    {
    case ImageType::RGB_u24:
        _IMAGE_DO_FLIP_V(PixelRGB);
        break;
    case ImageType::RGBA_u32:
        _IMAGE_DO_FLIP_V(PixelRGBA);
        break;
    case ImageType::GRAYSCALE_f32:
        _IMAGE_DO_FLIP_V(float);
        break;
    }
#undef _IMAGE_DO_FLIP_V
}

void Image::rotate_clockwise()
{
#define _IMAGE_DO_ROTATE_CW(type)                                                                                               \
    {                                                                                                                           \
        Eigen::Map<Eigen::Matrix<type, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>> mat(data.as<type *>(), height, width); \
        mat.transposeInPlace();                                                                                                 \
        mat.colwise().reverseInPlace();                                                                                         \
    }
    switch (type)
    {
    case ImageType::RGB_u24:
        _IMAGE_DO_ROTATE_CW(PixelRGB);
        break;
    case ImageType::RGBA_u32:
        _IMAGE_DO_ROTATE_CW(PixelRGBA);
        break;
    case ImageType::GRAYSCALE_f32:
        _IMAGE_DO_ROTATE_CW(float);
        break;
    }
#undef _IMAGE_DO_ROTATE_CW
    std::swap(width, height);
}

void Image::rotate_counterclockwise()
{
#define _IMAGE_DO_ROTATE_CCW(type)                                                                                              \
    {                                                                                                                           \
        Eigen::Map<Eigen::Matrix<type, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>> mat(data.as<type *>(), height, width); \
        mat.transposeInPlace();                                                                                                 \
        mat.rowwise().reverseInPlace();                                                                                         \
    }

    switch (type)
    {
    case ImageType::RGB_u24:
        _IMAGE_DO_ROTATE_CCW(PixelRGB);
        break;
    case ImageType::RGBA_u32:
        _IMAGE_DO_ROTATE_CCW(PixelRGBA);
        break;
    case ImageType::GRAYSCALE_f32:
        _IMAGE_DO_ROTATE_CCW(float);
        break;
    }
#undef _IMAGE_DO_ROTATE_CCW
    std::swap(width, height);
}

size_t Image::get_pixel_size() const
{
    switch (type)
    {
    case ImageType::RGB_u24:
        return 3 * sizeof(uint8_t);
    case ImageType::RGBA_u32:
        return 4 * sizeof(uint8_t);
    case ImageType::GRAYSCALE_f32:
        return 1 * sizeof(float);
    }
    return 0;
}

size_t Image::get_size() const
{
    return data.get_size();
}

Eigen::Vector2i Image::get_dimensions() const
{
    return Eigen::Vector2i(width, height);
}