#pragma once

#include <cstdint>
#include <string>
#include <functional>
#include <memory>
#include <Eigen/Dense>

#include "Image.hpp"

/**
 * @brief A texture is a 2D image that can be used in a shader
 * this class just wraps the opengl texture object, the actual data is stored on the gpu.
 * the method 'to_image' can be used to load the data back from the gpu.
 * 
 */
class Texture {

    // the opengl texture id
    uint32_t id;

    int channels;

    // the dimensions of the texture
    Eigen::Vector2i dimensions;

    public:

    /** read an image, and create a texture from its data */
    static std::shared_ptr<Texture> load(const std::string& path);

    /** create a texture from an image */
    Texture(const Image &image);

    /**
     * @brief bind the texture to a texture unit
     * 
     * @param unit a number between 0 and 31 (we limit the number of bound textures to 32)
     */
    void to_unit(uint32_t unit);

    /**
     * @brief get the dimensions of the texture
     * 
     * @return Eigen::Vector2i 
     */
    Eigen::Vector2i get_dimensions() const;

    /**
     * @brief load back data from the gpu
     * 
     * @return Image 
     */
    Image to_image();

    ~Texture();	

};

