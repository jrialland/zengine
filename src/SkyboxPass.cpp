#include "SkyboxPass.hpp"

#include <GL/glew.h>
#include <GL/gl.h>

SkyboxPass::SkyboxPass(const std::string &cubemapPath)
{
    skybox = std::make_unique<Cubemap>(cubemapPath);

    skyboxShader = std::unique_ptr<ShaderProgram>(new ShaderProgram({
        {
            ShaderType::Vertex,
            R"(
            #version 460 core
            uniform mat4 mvp;

            const float vertices[8 * 3] = float[](
                -1.0, -1.0, -1.0, // 0
                -1.0, -1.0,  1.0, // 1
                -1.0,  1.0, -1.0, // 2
                -1.0,  1.0,  1.0, // 3
                 1.0, -1.0, -1.0, // 4
                 1.0, -1.0,  1.0, // 5
                 1.0,  1.0, -1.0, // 6
                 1.0,  1.0,  1.0  // 7
            );

            const float indices[6 * 6] = float[](
                0, 1, 2, 2, 1, 3,
                4, 6, 5, 5, 6, 7,
                0, 4, 1, 1, 4, 5,
                2, 3, 6, 6, 3, 7,
                0, 2, 4, 4, 2, 6,
                1, 5, 3, 3, 5, 7
            );

            out vec3 texCoords;

            void main() {
                int index = gl_VertexID;
                vec3 position = vec3(vertices[index * 3], vertices[index * 3 + 1], vertices[index * 3 + 2]);
                texCoords = position;
                gl_Position = mvp * vec4(position, 1.0);
            }

        )"
        },
        {
            ShaderType::Fragment,
            R"( 
            #version 460 core

            uniform samplerCube skybox;
            
            in vec3 texCoords;
            
            out vec4 color;
            
            void main() {
                color = texture(skybox, texCoords);
            }
        )"
        }
    }));
}

struct temporary_set_depth_func_to_lequal
{
    temporary_set_depth_func_to_lequal()
    {
        glDepthFunc(GL_LEQUAL);
    }
    ~temporary_set_depth_func_to_lequal()
    {
        glDepthFunc(GL_LESS);
    }
};

void SkyboxPass::execute(RenderingSystem &renderingSystem)
{
    temporary_set_depth_func_to_lequal tmp;

    auto camera = renderingSystem.get_camera();

    skyboxShader->use();

    // set mvp matrix
    Eigen::Matrix4f mvp = camera->get_projection() * camera->get_view();
    skyboxShader->set_uniform("mvp", mvp);

    // using texture unit 0
    skybox->attach(0);
    skyboxShader->set_uniform("skybox", 0);

    // draw skybox
    skyboxShader->run(36);
}