#include "Texture.hpp"

#include <GL/glew.h>
#include <GL/gl.h>

static uint32_t get_format(int channels) {
        switch(channels) {
                case 1: return GL_RED;
                case 2: return GL_RG;
                case 3: return GL_RGB;
                case 4: return GL_RGBA;
                default: throw std::runtime_error("invalid number of channels");
        }
}

std::shared_ptr<Texture> Texture::load(const std::string& path) {
        return std::make_shared<Texture>(Image::load(path));
}

Texture::Texture(const Image &image) : dimensions(image.get_dimensions()), channels(image.get_channels()) {

        uint32_t format = get_format(channels);

        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);

        // set the texture wrapping/filtering options (on the currently bound texture object)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, format, dimensions.x(), dimensions.y(), 0, format, GL_UNSIGNED_BYTE, image.get_data());
        glGenerateMipmap(GL_TEXTURE_2D);
}

void Texture::to_unit(uint32_t unit) {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, id);
}

Eigen::Vector2i Texture::get_dimensions() const {
        return dimensions;
}

Image Texture::to_image() {
        glBindTexture(GL_TEXTURE_2D, id);
        Image image(dimensions.x(), dimensions.y(), channels);
        uint32_t format = get_format(channels);
        glGetTexImage(GL_TEXTURE_2D, 0, format, GL_UNSIGNED_BYTE, image.get_data());
        return image;
}

Texture::~Texture() {
    glDeleteTextures(1, &id);
}	