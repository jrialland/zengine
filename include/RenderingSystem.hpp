#pragma once

#include <vector>
#include <string>
#include <memory>

#include "Application.hpp"
#include "Camera.hpp"

struct Pass;

class RenderingSystem {

    struct PassInfo {
        std::string name;
        std::shared_ptr<Pass> pass;
        bool enabled = true;
    };

    std::vector<PassInfo> passes;

    Application* application;

    std::shared_ptr<Camera> camera;

    public:

    RenderingSystem(Application* application);

    ~RenderingSystem();

    virtual void init();

    virtual void init_passes();

    void add_pass(const std::string &name, std::shared_ptr<Pass> pass, const std::string &after = "", const std::string &before = "");

    void add_pass_before(const std::string &name, std::shared_ptr<Pass> pass, const std::string &before);

    void add_pass_after(const std::string &name, std::shared_ptr<Pass> pass, const std::string &after);

    void remove_pass(const std::string &name);

    void move_pass_before(const std::string &name, const std::string &before);

    void move_pass_after(const std::string &name, const std::string &after);

    void move_pass_to_front(const std::string &name);

    void move_pass_to_back(const std::string &name);

    std::shared_ptr<Pass> get_pass(const std::string &name);

    void render();

    std::shared_ptr<Camera> get_camera() const;

    Application* get_application() const;
};

struct Pass {
    private:
        std::shared_ptr<RenderingSystem> renderingSystem;
    public:
    virtual void init(std::shared_ptr<RenderingSystem> renderingSystem);
    std::shared_ptr<RenderingSystem> get_rendering_system() const;
    virtual void execute()=0;
};