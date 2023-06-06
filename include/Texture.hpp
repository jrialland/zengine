#pragma once

#include <cstdint>
#include <string>
#include <functional>

class Texture {

    uint32_t id;

    int width, height, channels;

    public:

    Texture(const std::string& path);

    ~Texture();	

    void attach(uint32_t slot = 0);

};

