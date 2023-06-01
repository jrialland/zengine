#pragma once

#include <cstdint>
#include <string>
#include <functional>

#include "Canvas.hpp"

class Texture {

    uint32_t id;

    int width, height, channels;

    public:

    Texture(const std::string& path);

    ~Texture();	

    void attach(uint32_t slot = 0);

    /**
     * @brief modify the texture using a function that takes a Canvas as argument.
     * 
     * @param f canvas operations
     * @param readback the texture data is read from the gpu before drawing starts. 
     */
    void modify(std::function<void(Canvas&)> f, bool readgpu = false);

    /**
     * @brief create a texture from a canvas.
     * 
     * @param width 
     * @param height 
     * @param f canvas operations
     * @return Texture 
     */
    static Texture from_canvas(int width, int height, std::function<void(Canvas&)> f);
};

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