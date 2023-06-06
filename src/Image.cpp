#include "Image.hpp"

#include "strutil.hpp"
#include "FileSystem.hpp"

#include "stb_image.h"
#include "stb_image_resize.h"
#include "stb_image_write.h"

    Image::Image(int width_, int height_, int channels_, void *data) :width(width_), height(height_), channels(channels_) {
        size_t bytes = width * height * channels * sizeof(float);
        if(data != nullptr) {
            blob = Blob(data, bytes, false);
        } else {
            blob = Blob(bytes);
        }
    }

    Image::Image(const Blob& data) {
        int w, h, c;
        stbi_uc *img = stbi_load_from_memory(data.as<const stbi_uc *>(), data.get_size(), &w, &h, &c, 0);
        if(img == nullptr) {
            throw std::runtime_error("Failed to load image");
        }
        width = w;
        height = h;
        channels = c;
        blob = Blob(img, width * height * channels, true);
    }

    Image::Image(const Image &image) : width(image.width), height(image.height), channels(image.channels) {
        blob = image.blob.copy();
    }

    Image::Image(Image &&image) : width(image.width), height(image.height), channels(image.channels) {
        blob = std::move(image.blob);
    }
    
    Image Image::croped(int x, int y, int width, int height) {
        stbi_uc *data = blob.as<stbi_uc *>();
        stbi_uc *croped = static_cast<stbi_uc *>(malloc(width * height * channels));
        stbir_resize_uint8(data, this->width, this->height, 0, croped, width, height, 0, channels);
        return Image(width, height, channels, croped);
    }

    Image Image::resized(int width, int height) {
        stbi_uc *data = blob.as<stbi_uc *>();
        stbi_uc *resized = static_cast<stbi_uc *>(malloc(width * height * channels));
        stbir_resize_uint8(data, this->width, this->height, 0, resized, width, height, 0, channels);
        return Image(width, height, channels, resized);
    }

    void Image::save(const std::string &filename) {
        auto lcase = strutil::to_lower(filename);
        if(strutil::ends_with(lcase, ".png")) {
            stbi_write_png(filename.c_str(), width, height, channels, blob.as<const void*>(), width * channels);
        } else if(strutil::ends_with(lcase, ".jpg") || strutil::ends_with(lcase, ".jpeg")) {
            stbi_write_jpg(filename.c_str(), width, height, channels, blob.as<const void*>(), 100);
        } else if(strutil::ends_with(lcase, ".bmp")) {
            stbi_write_bmp(filename.c_str(), width, height, channels, blob.as<const void*>());
        } else {
            throw std::runtime_error("only the following formats are supported: png, jpg, jpeg, bmp");
        }
    }
    
    Eigen::Vector2i Image::size() const {
        return Eigen::Vector2i(width, height);
    }

    int Image::get_channels() const {
        return channels;
    }

    Image Image::load(const std::string &filename) {
            return Image(FileSystem::get_entry(filename)->read());
    }