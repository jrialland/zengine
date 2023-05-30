#pragma once

#include <Eigen/Dense>

inline float degToRad(float deg) {
    return deg * M_PI / 180.0f;
}

class Camera {
    protected:
    Eigen::Vector3f position;
    Eigen::Vector3f direction;
    Eigen::Vector3f up;
    Eigen::Vector3f right;
    float near;
    float far;
    float aspectRatio;

    public:

    Eigen::Vector3f get_position() const;

    void set_near(float near);
    void set_far(float far);
    float get_near() const;
    float get_far() const;

    void set_position(const Eigen::Vector3f &position);

    void set_aspect(float aspectRatio);
    void set_aspect(int width, int height);
    void set_aspect(const Eigen::Vector2i &size);

    float get_aspect() const;

    void move(const Eigen::Vector3f &offset);

    void move_forward(float offset);

    void move_right(float offset);

    void move_up(float offset);

    void look_at(const Eigen::Vector3f &target);

    /**
     * rotate the camera around its own axis
     * @param roll rotation around the forward axis (in radians)
     * @param pitch rotation around the right axis (in radians)
     * @param yaw rotation around the up axis (in radians)
    */
    void rotate(float roll, float pitch, float yaw);

    /**
     * the transformation matrix from world space to camera space
     */
    Eigen::Matrix4f get_view() const;

    /**
     * the transformation matrix from camera space to clip space
     */
    virtual Eigen::Matrix4f get_projection() const = 0;
};

class PerspectiveCamera : public Camera {

    private:
    
    float fov;
    
    public:
    
    PerspectiveCamera(float fov=M_PI/4, float aspectRatio=1.0f, float near=0.1f, float far=1000.0f);
    
    void set_fov(float fov);

    float get_fov() const;

    Eigen::Matrix4f get_projection() const override;
};

class OrthographicCamera : public Camera {
    private:
    float left, right, bottom, top;
    OrthographicCamera(float left, float right, float bottom, float top, float near=0.1f, float far=1000.0f);
    Eigen::Matrix4f get_projection() const override;
};