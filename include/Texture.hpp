#pragma once

#include <cstdint>
#include <string>
#include <functional>
#include <memory>

class Texture {

    uint32_t id;

    int width, height, channels;

    public:

    static std::shared_ptr<Texture> load(const std::string& path);

    void to_unit(uint32_t unit);

    ~Texture();	

};

