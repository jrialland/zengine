#pragma once
#include <string>

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
class Cubemap {

    uint32_t id;

    int face_size;

    int channels;

    public:

    Cubemap(const std::string& path);

    ~Cubemap();

    void attach(uint32_t slot = 0);

};