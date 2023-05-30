#include "Window.hpp"

#include <glog/logging.h>

#include <GL/glew.h>
#include <GL/gl.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <map>
#include <stdexcept>

static bool glew_inited = false;

static std::map<GLFWwindow *, Window *> window_map;

static void error_callback(int error, const char *description)
{
    DLOG(ERROR) << "GLFW error " << error << " : " << description;
}

static void resize_callback(GLFWwindow *window, int width, int height)
{
    auto it = window_map.find(window);
    if (it != window_map.end())
    {
        Eigen::Vector2i size(width, height);
        it->second->resized(size);
    }
}

Window::Window(const std::string &title, int width, int height, bool fullscreen)
{

    // initialize glfw if necessary
    if (window_map.empty())
    {
        if (!glfwInit())
        {
            throw std::runtime_error("GLFW library initialization failed");
        }
        glfwSetErrorCallback(error_callback);
    }

    // create the GWLF window
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    handle = (void *)glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!handle)
    {
        throw std::runtime_error("GLFW window or context creation failed");
    }
    else
    {
        window_map[(GLFWwindow *)handle] = this;
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

    // set up callbacks
    glfwSetFramebufferSizeCallback((GLFWwindow *)handle, resize_callback);
}

Window::~Window()
{
    glfwDestroyWindow((GLFWwindow *)handle);
    window_map.erase((GLFWwindow *)handle);
    if (window_map.empty())
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

Eigen::Vector2i Window::get_size() const
{
    int width, height;
    glfwGetFramebufferSize((GLFWwindow *)handle, &width, &height);
    return Eigen::Vector2i(width, height);
}