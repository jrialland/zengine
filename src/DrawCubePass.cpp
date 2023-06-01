#include "DrawCubePass.hpp"
#include "Geometry.hpp"


DrawCubePass::DrawCubePass()
{

    // just a red cube
    auto program = std::make_shared<ShaderProgram>(
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

    std::vector<Eigen::Vector3f> vertices;
    for(auto& v : cube.vertices)
    {
        vertices.emplace_back(v.x(), v.y(), v.z()); // TODO wtf converting doubles to floats !?
    }

    vertexBuffer = std::make_shared<VertexBuffer>(vertices);
    elementBuffer = std::make_shared<VertexBuffer>(cube.indices);

    vao.set_shader_program(program);       // TODO maybe add a shortcut method to directly assign shader code
    vao.set_buffer(0, "v3", vertexBuffer); // TODO maybe add a shortcut method to directly assign vertex data, and maybe add a shortcut method to directly assign index data
    vao.set_ebo(elementBuffer);

    // TODO maybe the vao could maintain a list of all the buffers it uses so that we should not have to keep them alive here
}


void DrawCubePass::execute(RenderingSystem &renderingSystem)
{
    Eigen::Matrix4f mvp = renderingSystem.get_camera()->get_projection() * renderingSystem.get_camera()->get_view(); // no model, just the cube geometry 1*1*1 centered at 0,0,0
    vao.get_shader_program()->set_uniform("mvp", mvp); // TODO get_shader_program() => get_program() ?
    vao.render();
}