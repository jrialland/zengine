#include "Texture.hpp"
#include "FileSystem.hpp"

#include <GL/glew.h>
#include <GL/gl.h>

#include "stb_image.h"

#include <stdexcept>
#include <cassert>
#include <cstring>

std::shared_ptr<Texture> Texture::load(const std::string& path) {
    Texture* texture = new Texture;
    Blob blob = FileSystem::get_entry(path)->read();

    glGenTextures(1, &texture->id);
    glBindTexture(GL_TEXTURE_2D, texture->id);

    stbi_uc* data = stbi_load_from_memory(blob.as<stbi_uc*>(), blob.get_size(), &texture->width, &texture->height, &texture->channels, 0);
    if(!data) {
        glDeleteTextures(1, &texture->id);
        throw std::runtime_error("Failed to load texture: " + path);
    }
    glTexImage2D(GL_TEXTURE_2D, 0, texture->channels==4 ? GL_RGBA : GL_RGB, texture->width, texture->height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);
    glGenerateMipmap(GL_TEXTURE_2D);

    return std::shared_ptr<Texture>(texture);
}

void Texture::to_unit(uint32_t unit) {
    assert(unit < 32);
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, id);
}



Texture::~Texture() {
    glDeleteTextures(1, &id);
}


