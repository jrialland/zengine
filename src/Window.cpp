#include "Window.hpp"
#include "Logging.hpp"

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
    LOG(ERROR) << "GLFW error " << error << " : " << description;
}

static void resize_callback(GLFWwindow *window, int width, int height)
{
    auto it = window_map.find(window);
    if (it != window_map.end())
    {
        glViewport(0, 0, width, height);
        glScissor(0, 0, width, height);
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

        // opgenl debug output
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback([](GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam) {
            LOG(INFO) << "OpenGL debug message : " << message;
        }, nullptr);


    }

    // set up callbacks
    glfwSetFramebufferSizeCallback((GLFWwindow *)handle, resize_callback);
}

Window::~Window()
{
    glfwSetFramebufferSizeCallback((GLFWwindow *)handle, nullptr);
    glfwDestroyWindow((GLFWwindow *)handle);
    window_map.erase((GLFWwindow *)handle);
    if (window_map.empty())
    {
        glfwTerminate();
    }
}

Eigen::Vector2i Window::get_size() const
{
    int width, height;
    glfwGetFramebufferSize((GLFWwindow *)handle, &width, &height);
    return Eigen::Vector2i(width, height);
}