#include "Application.hpp"
#include <GLFW/glfw3.h>

#if defined(MSVC) || defined(__MINGW32__)
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
#else
static void fixStdout() {}
#endif

Application::Application() {
    
    fixStdout();

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