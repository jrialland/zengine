#include "Logging.hpp"
#include "Application.hpp"
#include "RenderingSystem.hpp"
#include "VertexArray.hpp"
#include "Texture.hpp"

class HelloTexture : public Pass
{
private:
    VertexArray vao;

    std::shared_ptr<Texture> texture;

public:
    HelloTexture()
    {

        float vertices[] = {
            // positions         // texture coords
            -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
            0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.5f, 0.0f, 0.5f, 1.0f};

        auto vbo = std::make_shared<VertexBuffer>(vertices, sizeof(vertices));
        vao.bind_buffer(0, "v3 (v2)", vbo);
        vao.bind_buffer(1, "(v3) v2", vbo);

        vao.set_program(R"(
        #version 330 core
        
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec2 aTexCoord;
        
        out vec2 TexCoord;
        
        void main()
        {
            TexCoord = aTexCoord;
            gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
        }
    )",
                        R"(
        #version 330 core
        out vec4 FragColor;
        in vec2 TexCoord;
        uniform sampler2D texture1;
        void main()
        {
            FragColor = texture(texture1, TexCoord);
        } 
    )");

        texture = Texture::load("assets://wall.jpg");
        texture->to_unit(0);
        vao.get_shader_program()->set_uniform("texture1", 0);
    }

    void execute() override
    {
        vao.render();
    }
};

class Example4 : public Application
{

    void init() override
    {

        get_rendering_system()->add_pass("drawcube", std::make_shared<HelloTexture>());
    }
};

int main(int argc, char **argv)
{
    try
    {
        Example4 example4;
        return example4.run(argc, argv);
    }
    catch (std::exception &e)
    {
        LOG(ERROR) << e.what();
        return 1;
    }
}