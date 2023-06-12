#include "Application.hpp"
#include "RenderingSystem.hpp"
#include "Logging.hpp"
#include "BackTrace.hpp"

#include <GL/glew.h>
#include <GL/gl.h>

#include <GLFW/glfw3.h>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#define WINDOWS
#endif

#ifdef WINDOWS
#include <Windows.h>
#include <io.h>
#include <fcntl.h>
/**
 * this is a hacky fix for stdout not working in windows gui apps
*/
static void fixStdout() {
    static bool fixed = false;
    if (fixed) return;
	DWORD ret = AttachConsole(-1);
	if (ret != 0) {
		HANDLE lStdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
		int hConHandle = _open_osfhandle((intptr_t)lStdHandle, 0);
		FILE* fp = _fdopen( hConHandle, "w" );
		*stdout = *fp;
	}
    fixed = true;
}
#endif

Application::Application() {
    
    #ifdef WINDOWS
    fixStdout();
    #endif
    
    window = new Window;
    rendering_system = new RenderingSystem(this);

    // connect window resize signal to camera aspect ratio
    window->resized.connect([&](const Eigen::Vector2i &size) {
        rendering_system->get_camera()->set_aspect(size.x() / (float)size.y());
    });

    // configure opengl debug callback
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback([](GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam) {
            LOG(INFO) << "OpenGL message : " << message;
            print_backtrace(2);
    }, nullptr);

}

Application::~Application() {
    //delete rendering_system;
    delete window;
}

RenderingSystem* Application::get_rendering_system() {
    return rendering_system;
}

Window* Application::get_window(){
    return window;
}

void Application::init() {
    // set initial camera aspect ratio
    auto size = window->get_dimensions();
    rendering_system->get_camera()->set_aspect(size.x() / (float)size.y());
}

int Application::run(int argc, char **argv) {

    // initialize application
    init();

    // render loop
    GLFWwindow *glfw_window = (GLFWwindow *)window->handle;
    while (!glfwWindowShouldClose(glfw_window))
    {

        // clear buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        // render
        rendering_system->render();

        // present frame buffer
        glfwSwapBuffers(glfw_window);
        glfwPollEvents();
    }
    return 0;
}