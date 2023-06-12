#include "Logging.hpp"
#include "Application.hpp"
#include "RenderingSystem.hpp"
#include "VertexArray.hpp"

class ColorTriangle : public Pass
{
	private:
	VertexArray vao;
	public:
	ColorTriangle() {
    
    // vertices and associated colors
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,   1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.0f,    0.0f, 1.0f, 0.0f,
        0.0f,  0.5f, 0.0f,    0.0f, 0.0f, 1.0f
    };  

    LOG(INFO) << "setting up vbo"; 
    auto vbo = std::make_shared<VertexBuffer>(vertices, sizeof(vertices));

    vao.bind_buffer(0, "v3 (v3)", vbo); // bind the first 3 floats to the first attribute, and the next 3 floats to the second attribute
    vao.bind_buffer(1, "(v3) v3", vbo);

    LOG(INFO) << "setting up shader program";
    vao.set_program(R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aColor;
        out vec3 ourColor;
        void main()
        {
            ourColor = aColor;
            gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
        }
    )", R"(
        #version 330 core
        in vec3 ourColor;
        out vec4 FragColor;

        void main()
        {
            FragColor = vec4(ourColor, 1.0f);
        } 
    )");
    
}

void execute() override {
    vao.render();
}
};


class Example3 : public Application
{

	void init() override
	{

		get_rendering_system()->add_pass("drawcube", std::make_shared<ColorTriangle>());
	}
};

int main(int argc, char **argv)
{
	try
	{
		Example3 example3;
		return example3.run(argc, argv);
	}
	catch (std::exception &e)
	{
		LOG(ERROR) << e.what();
		return 1;
	}
}