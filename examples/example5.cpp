#include "Logging.hpp"
#include "Application.hpp"
#include "RenderingSystem.hpp"
#include "VertexArray.hpp"
#include "Geometry.hpp"

class DrawCube : public Pass
{
private:
    VertexArray vao;
public:
    DrawCube()
    {
        auto cube = basegeometries::cube();

        vao.bind_buffer(0, cube.vertices);
        vao.set_ebo(cube.indices);

        vao.set_program(R"(
        #version 330 core
        
        layout (location = 0) in vec3 aPos;
        
        uniform mat4 mvp;
        
        void main()
        {
            gl_Position = mvp * vec4(aPos.x, aPos.y, aPos.z, 1.0);
        }
    )",
                        R"(
        #version 330 core
        out vec4 FragColor;
        void main()
        {
            FragColor = vec4(1.0, 0.0, 0.0, 1.0);
        } 
    )");
    }

    void execute() override
    {
        Eigen::Matrix4f mvp = get_rendering_system()->get_camera()->get_projection() * get_rendering_system()->get_camera()->get_view();
        vao.get_shader_program()->set_uniform("mvp", mvp);
        vao.render();
    }
};

class Example5 : public Application
{

    void init() override
    {
        get_rendering_system()->add_pass("drawcube", std::make_shared<DrawCube>());
        get_rendering_system()->get_camera()->set_position({3,3,3});
        get_rendering_system()->get_camera()->look_at({0, 0, 0});
        Eigen::Matrix4f mvp = get_rendering_system()->get_camera()->get_projection() * get_rendering_system()->get_camera()->get_view();
        auto cube = basegeometries::cube();
        cube.recompute_normals();
        for(auto vertice : cube.vertices) {
            Eigen::Vector4f v = mvp * Eigen::Vector4f(vertice.x(), vertice.y(), vertice.z(), 1.0);
            LOG(INFO) << "---------------------------------------";
            LOG(INFO) << vertice.transpose();
            LOG(INFO) << v.transpose();
        }
    }
};

int main(int argc, char **argv)
{
    try
    {
        Example5 example5;
        return example5.run(argc, argv);
    }
    catch (std::exception &e)
    {
        LOG(ERROR) << e.what();
        return 1;
    }
}