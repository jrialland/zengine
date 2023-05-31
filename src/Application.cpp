#include "Application.hpp"
#include <glog/logging.h>
#include <GLFW/glfw3.h>

Application::Application() {

    // connect window resize signal to camera aspect ratio
    window.resized.connect([&](const Eigen::Vector2i &size) {
        rendering_system.get_camera()->set_aspect(size.x() / (float)size.y());
    });

}

Application::~Application() {
}

RenderingSystem &Application::get_rendering_system() {
    return rendering_system;
}

Window &Application::get_window(){
    return window;
}

void Application::init() {
    // does nothing by default
}

int Application::run(int argc, char **argv) {

	// Initialize Google's logging library.
	google::InitGoogleLogging(argv[0]);

    // set initial camera aspect ratio
    auto size = window.get_size();
    rendering_system.get_camera()->set_aspect(size.x() / (float)size.y());

    // initialize application
    init();

    // render loop
    GLFWwindow *glfw_window = (GLFWwindow *)window.handle;
    while (!glfwWindowShouldClose(glfw_window))
    {

        // clear buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        rendering_system.render();

        // present frame buffer
        glfwSwapBuffers(glfw_window);
        glfwPollEvents();
    }
    return 0;
}