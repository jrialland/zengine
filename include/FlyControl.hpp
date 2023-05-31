#pragma once

#include "Camera.hpp"
#include <memory>

/*
* control a camera with the mouse and keyboard
*/
class FlyControl {

    std::shared_ptr<Camera> camera;

    public:

    void set_camera(std::shared_ptr<Camera> camera);

    void activate();
};