#include "RenderingSystem.hpp"

#include <GL/glew.h>
#include <GL/gl.h>

RenderingSystem::RenderingSystem()
{
    camera = std::shared_ptr<Camera>(new PerspectiveCamera(degToRad(60.0f), 1.0, 0.1f, 1000.0f));
    init();
    init_passes();
}

RenderingSystem::~RenderingSystem()
{
}

void RenderingSystem::init() {
    // set some common opengl defaults
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL); // because we render the skybox after everything else
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void RenderingSystem::init_passes() {
    // add passes here
}

void RenderingSystem::add_pass(const std::string &name, std::shared_ptr<Pass> pass, const std::string &after , const std::string &before)
{
    auto passInfo = PassInfo{name, pass};
    if (after.empty() && before.empty())
    {
        passes.push_back(passInfo);
    }
    else if (!after.empty())
    {
        auto it = std::find_if(passes.begin(), passes.end(), [&after](const PassInfo &passInfo)
                               { return passInfo.name == after; });
        if (it == passes.end())
        {
            throw std::runtime_error("Pass " + after + " not found");
        }
        passes.insert(it, passInfo);
    }
    else
    {
        auto it = std::find_if(passes.begin(), passes.end(), [&before](const PassInfo &passInfo)
                               { return passInfo.name == before; });
        if (it == passes.end())
        {
            throw std::runtime_error("Pass " + before + " not found");
        }
        passes.insert(it + 1, passInfo);
    }
}

void RenderingSystem::add_pass_before(const std::string &name, std::shared_ptr<Pass> pass, const std::string &before)
{
    add_pass(name, pass, "", before);
}

void RenderingSystem::add_pass_after(const std::string &name, std::shared_ptr<Pass> pass, const std::string &after)
{
    add_pass(name, pass, after, "");
}

void RenderingSystem::remove_pass(const std::string &name)
{
    auto it = std::find_if(passes.begin(), passes.end(), [&name](const PassInfo &passInfo)
                           { return passInfo.name == name; });
    if (it == passes.end())
    {
        throw std::runtime_error("Pass " + name + " not found");
    }
    passes.erase(it);
}

void RenderingSystem::move_pass_before(const std::string &name, const std::string &before)
{
    auto it = std::find_if(passes.begin(), passes.end(), [&name](const PassInfo &passInfo)
                           { return passInfo.name == name; });
    if (it == passes.end())
    {
        throw std::runtime_error("Pass " + name + " not found");
    }
    auto passInfo = *it;
    passes.erase(it);
    add_pass_before(passInfo.name, passInfo.pass, before);
}

void RenderingSystem::move_pass_after(const std::string &name, const std::string &after)
{
    auto it = std::find_if(passes.begin(), passes.end(), [&name](const PassInfo &passInfo)
                           { return passInfo.name == name; });
    if (it == passes.end())
    {
        throw std::runtime_error("Pass " + name + " not found");
    }
    auto passInfo = *it;
    passes.erase(it);
    add_pass_after(passInfo.name, passInfo.pass, after);
}

void RenderingSystem::move_pass_to_front(const std::string &name)
{
    auto it = std::find_if(passes.begin(), passes.end(), [&name](const PassInfo &passInfo)
                           { return passInfo.name == name; });
    if (it == passes.end())
    {
        throw std::runtime_error("Pass " + name + " not found");
    }
    auto passInfo = *it;
    passes.erase(it);
    passes.insert(passes.begin(), passInfo);
}

void RenderingSystem::move_pass_to_back(const std::string &name)
{
    auto it = std::find_if(passes.begin(), passes.end(), [&name](const PassInfo &passInfo)
                           { return passInfo.name == name; });
    if (it == passes.end())
    {
        throw std::runtime_error("Pass " + name + " not found");
    }
    auto passInfo = *it;
    passes.erase(it);
    passes.push_back(passInfo);
}

std::shared_ptr<Pass> RenderingSystem::get_pass(const std::string &name) {
    auto it = std::find_if(passes.begin(), passes.end(), [&name](const PassInfo &passInfo)
                           { return passInfo.name == name; });
    if (it == passes.end())
    {
        throw std::runtime_error("Pass " + name + " not found");
    }
    return it->pass;
}

void RenderingSystem::render()
{
    for (auto &passInfo : passes)
    {
        if (passInfo.enabled)
        {
            passInfo.pass->execute(*this);
        }
    }
}

std::shared_ptr<Camera> RenderingSystem::get_camera() const
{
    return camera;
}