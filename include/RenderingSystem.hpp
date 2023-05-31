#pragma once

#include <vector>
#include <string>
#include <memory>

#include "Camera.hpp"

struct Pass;

class RenderingSystem {

    struct PassInfo {
        std::string name;
        std::shared_ptr<Pass> pass;
        bool enabled = true;
    };

    std::vector<PassInfo> passes;

    std::shared_ptr<Camera> camera;

    public:

    RenderingSystem();

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
};

struct Pass {
    virtual void execute(RenderingSystem &renderingSystem)=0;
};