#pragma once

#include "RenderingSystem.hpp"
#include "VertexArray.hpp"

/**
 * just the basic example from https://learnopengl.com/Getting-started/Hello-Triangle
*/
class HelloTriangle : public Pass {
    
    VertexArray vao;

public:

    HelloTriangle();
    void execute(RenderingSystem &renderingSystem) override;
};