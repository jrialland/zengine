#include "Window.hpp"

#include <glog/logging.h>

#include <GL/glew.h>
#include <GL/gl.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <stdexcept>

static int window_instances = 0;
static bool glew_inited = false;

static void error_callback(int error, const char *description)
{
    DLOG(ERROR) << "GLFW error " << error << " : " << description;
}

Window::Window(const std::string &title, int width, int height, bool fullscreen)
{

    // initialize glfw if necessary
    if (window_instances == 0)
    {
        if (!glfwInit())
        {
            throw std::runtime_error("GLFW library initialization failed");
        }
        glfwSetErrorCallback(error_callback);
    }
    window_instances += 1;

    // create the GWLF window
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    handle = (void *)glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!handle)
    {
        throw std::runtime_error("GLFW window or context creation failed");
    }

    glfwMakeContextCurrent((GLFWwindow *)handle);

    // GLEW can be inited once we have a GL context
    if (!glew_inited)
    {
        glewExperimental = true;
        GLenum err = glewInit();
        if (err != GLEW_OK)
        {
            std::string msg("GLEW library intialization failed : ");
            msg += std::string((char *)glewGetErrorString(err));
            throw std::runtime_error(msg);
        }
        else
        {
            glew_inited = true;
        }
    }
}

Window::~Window()
{
    glfwDestroyWindow((GLFWwindow *)handle);
    window_instances -= 1;
    if (window_instances == 0)
    {
        glfwTerminate();
    }
}

int Window::render_loop(int argc, char **argv)
{
    GLFWwindow *window = (GLFWwindow *)handle;
    while (!glfwWindowShouldClose(window))
    {

        glClear(GL_COLOR_BUFFER_BIT);

        // render code goes here

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    return 0;
}