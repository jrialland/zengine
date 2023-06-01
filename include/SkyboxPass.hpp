#pragma once

#include "RenderingSystem.hpp"
#include "ShaderProgram.hpp"
#include "Cubemap.hpp"

#include <string>
#include <memory>

class SkyboxPass : public Pass
{
    std::unique_ptr<ShaderProgram> skyboxShader;

    std::shared_ptr<Cubemap> skybox;

    public:

    SkyboxPass(const std::string& cubemapPath = "assets://skybox.jpg");

    void execute(RenderingSystem &renderingSystem) override;
};
