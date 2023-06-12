#include "Overlay2D.hpp"
#include "VertexArray.hpp"

#include <GLFW/glfw3.h>

Overlay2D::Overlay2D()
{
    overlay2d_vao.set_program(R"(
        #version 330
        layout(location=0) in vec2 position;
        layout(location=1) in vec2 texCoord;
        out vec2 fTexCoord;
        main() {
            fTexCoord = texCoord;
            gl_Position = vec4(position, 0.0, 1.0);
        }
    )", R"(
        #version 330
        in vec2 fTexCoord;
        out vec4 color;
        uniform sampler2D tex;
        void main() {
            color = texture(tex, fTexCoord);
        }
    )");
    
    // vertices in clip space
    overlay2d_vao.bind_buffer(0, "v2", {
        -1.0, -1.0,
        1.0, -1.0,
        1.0, 1.0,
        -1.0, 1.0,
    });

    overlay2d_vao.set_ebo({
        0, 1, 2,
        2, 3, 0,
    });

    // texture coordinates
    overlay2d_vao.bind_buffer(1, "v2", {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0, 1.0f,
        0.0f, 1.0f,
    });
}

void Overlay2D::renderTexture(std::shared_ptr<Texture> texture, int x, int y, int width, int height) {

    auto texture_dimensions = texture->get_dimensions();
    int w = width < 0 ? texture_dimensions.x() : width;
    int h = height < 0 ? texture_dimensions.y() : height;

    // change viewport
    glViewport(x, y, w, h);

    // turn off depth test
    glDisable(GL_DEPTH_TEST);

    //bind texture
    texture->to_unit(0);
    overlay2d_vao.get_shader_program()->set_uniform("tex", 0);

    // draw the texture
    overlay2d_vao.render();

    // restore depth test
    glEnable(GL_DEPTH_TEST);

    // restore viewport
    auto dimensions = get_rendering_system()->get_application()->get_window()->get_dimensions();
    glViewport(0, 0, dimensions.x(), dimensions.y());
}
