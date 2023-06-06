#include "Texture.hpp"
#include "FileSystem.hpp"

#include <GL/glew.h>
#include <GL/gl.h>

#include "stb_image.h"

#include <stdexcept>
#include <cassert>
#include <cstring>

Texture::Texture(const std::string& path) {

    Blob blob = FileSystem::get_entry(path)->read();

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    stbi_uc* data = stbi_load_from_memory(blob.as<stbi_uc*>(), blob.get_size(), &width, &height, &channels, 0);
    if(!data) {
        glDeleteTextures(1, &id);
        throw std::runtime_error("Failed to load texture: " + path);
    }
    glTexImage2D(GL_TEXTURE_2D, 0, channels==4 ? GL_RGBA : GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);
    glGenerateMipmap(GL_TEXTURE_2D);
}

Texture::~Texture() {
    glDeleteTextures(1, &id);
}

void Texture::attach(uint32_t slot) {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, id);
}


static stbi_uc* crop(stbi_uc* data, int width, int height, int channels, int x, int y, int w, int h) {
    stbi_uc* cropped = new stbi_uc[w * h * channels];
    for(int i = 0; i < h; i++) {
        memcpy(cropped + i * w * channels, data + ((y + i) * width + x) * channels, w * channels);
    }
    return cropped;
}

