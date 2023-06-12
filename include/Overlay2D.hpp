#pragma once

#include "RenderingSystem.hpp"
#include "VertexArray.hpp"
#include "Texture.hpp"
#include <memory>

class Overlay2D : public Pass
{
    private:

    VertexArray overlay2d_vao;

    public:

    Overlay2D();

    void renderTexture(std::shared_ptr<Texture> texture, int x=0, int y=0, int width=-1, int height=-1);

};