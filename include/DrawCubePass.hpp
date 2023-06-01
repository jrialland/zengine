#pragma once

#include "RenderingSystem.hpp"
#include "VertexArray.hpp"

class DrawCubePass : public Pass
{
    private:
    VertexArray vao;
    std::shared_ptr<VertexBuffer> vertexBuffer;
    std::shared_ptr<VertexBuffer> elementBuffer; // TODO: may change the class name to 'Buffer' and add a 'type' field
    public:    
    DrawCubePass();
    void execute(RenderingSystem &renderingSystem) override;
};
