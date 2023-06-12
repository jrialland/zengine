#include "DrawCubePass.hpp"
#include "Geometry.hpp"


DrawCubePass::DrawCubePass()
{

    vao.set_program(
        R"(
        #version 460 core
        layout(location = 0) in vec3 position;
        layout(location = 0) uniform mat4 mvp;
        void main() {
            gl_Position = mvp * vec4(position, 1.0);
        }
    )",
        R"(
        #version 460 core
        out vec4 fragColor;
        void main() {
            fragColor = vec4(1.0, 0.0, 0.0, 1.0);
        }
    )");

    Geometry cube = basegeometries::cube();

    vao.bind_buffer(0, cube.vertices);
    vao.set_ebo(cube.indices);
}


void DrawCubePass::execute()
{
    auto camera = get_rendering_system()->get_camera();
    Eigen::Matrix4f mvp = camera->get_projection() * camera->get_view(); // no model, just the cube geometry 1*1*1 centered at 0,0,0
    vao.get_shader_program()->set_uniform("mvp", mvp); // TODO get_shader_program() => get_program() ?
    vao.render();
}