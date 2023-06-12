#include "Logging.hpp"
#include "Application.hpp"
#include "RenderingSystem.hpp"
#include "VertexArray.hpp"

class HelloEbo : public Pass
{
	private:
	VertexArray vao;
	public:
	HelloEbo() {
    
    LOG(INFO) << "setting up vbo";
    vao.bind_buffer(0, "v3", {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.5f,  0.5f, 0.0f,
        -0.5f,  0.5f, 0.0f
    });
    
    LOG(INFO) << "setting up ebo";
    vao.set_ebo({0, 1, 2, 0, 2, 3});

    LOG(INFO) << "setting up shader program";
    vao.set_program(R"(
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
    LOG(INFO) << "done";
}

void execute() override {
    vao.render();
}
};


class Example2 : public Application
{

	void init() override
	{

		get_rendering_system()->add_pass("drawcube", std::make_shared<HelloEbo>());
	}
};

int main(int argc, char **argv)
{
	try
	{
		Example2 example2;
		return example2.run(argc, argv);
	}
	catch (std::exception &e)
	{
		LOG(ERROR) << e.what();
		return 1;
	}
}