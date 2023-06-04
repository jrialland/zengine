#include "Logging.hpp"
#include "HelloTriangle.hpp"

HelloTriangle::HelloTriangle() {
    
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f,  0.5f, 0.0f
    };  

    LOG(INFO) << "setting up vbo"; 
    auto vbo = std::make_shared<VertexBuffer>(vertices, sizeof(vertices));
    vao.set_buffer(0, "v3", vbo);

    LOG(INFO) << "setting up shader program";
    auto program = std::make_shared<ShaderProgram>(R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;

        void main()
        {
            gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
        }
    )", R"(
        #version 330 core
        out vec4 FragColor;

        void main()
        {
            FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
        } 
    )");
    
    vao.set_shader_program(program);
    LOG(INFO) << "done setting up HelloTriangle";
}

void HelloTriangle::execute(RenderingSystem &renderingSystem) {
    vao.render();
}