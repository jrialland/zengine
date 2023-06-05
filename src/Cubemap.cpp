#include "Cubemap.hpp"
#include "FileSystem.hpp"
#include "stb_image.h"
#include <cassert>
#include <cstring>
#include <stdexcept>

#include <GL/glew.h>
#include <GL/gl.h>


/**
 * cubemap textures are stored in big images containing all 6 faces.
 * the faces are stored in the following layout:
 * 
 *        +------+
 *        |      |
 *        |  +Z  |
 *        |      |
 * +------+------+------+------+
 * |      |      |      |      |
 * |  -X  |  -Y  |  +X  |  +Y  |
 * |      |      |      |      |
 * +------+------+------+------+
 *        |      |
 *        |  -Z  |
 *        |      |
 *        +------+
*/
Cubemap::Cubemap(const std::string& path) {
    Blob blob = FileSystem::get_entry(path)->read();

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, id);

    int image_width, image_height;
    stbi_uc* data = stbi_load_from_memory(blob.as<stbi_uc*>(), blob.get_size(), &image_width, &image_height, &channels, 0);
    if(!data) {
        glDeleteTextures(1, &id);
        throw std::runtime_error("Failed to load texture: " + path);
    }

    assert(image_width % 4 == 0 && "Cubemap width must be a multiple of 4");
    face_size = image_width / 4;
    assert(face_size == image_height / 3 && "Cubemap width must be 3 times its height");

    stbi_uc* tmp = new stbi_uc[face_size * face_size * channels];
    auto assignTexture = [&](GLenum role, int x, int y) {
        for(int i = 0; i < face_size; i++) {
            memcpy(tmp + i * face_size * channels, data + ((y + i) * image_width + x) * channels, face_size * channels);
        }
        glTexImage2D(role, 0, channels==4 ? GL_RGBA : GL_RGB, face_size, face_size, 0, GL_RGB, GL_UNSIGNED_BYTE, tmp);
    };
    
    assignTexture(GL_TEXTURE_CUBE_MAP_POSITIVE_X, face_size * 2, face_size);
    assignTexture(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, face_size);
    assignTexture(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, face_size, 0);
    assignTexture(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, face_size, face_size * 2);
    assignTexture(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, face_size, face_size);
    assignTexture(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, face_size * 3, face_size);

    delete[] tmp;


    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    stbi_image_free(data);
}

Cubemap::~Cubemap() {
    glDeleteTextures(1, &id);
}

void Cubemap::attach(uint32_t slot) {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_CUBE_MAP, id);
}