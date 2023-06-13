#pragma once

#include "Math.hpp"
#include "Ray.hpp"

class Camera
{
protected:
    /**
    * By default, the camera is at the origin, looking along the positive z axis, with the positive y axis as up, and the positive x axis as right
    */
    Eigen::Vector3f position = Eigen::Vector3f::Zero();
    Eigen::Vector3f direction = Eigen::Vector3f::UnitZ();
    Eigen::Vector3f up = Eigen::Vector3f::UnitY();
    Eigen::Vector3f right = Eigen::Vector3f::UnitX();
    float near;
    float far;
    float aspectRatio;
public:
    Eigen::Vector3f get_position() const;

    virtual void set_near(float near);
    virtual void set_far(float far);
    float get_near() const;
    float get_far() const;

    Eigen::Vector3f get_direction() const;
    Eigen::Vector3f get_up() const;
    Eigen::Vector3f get_right() const;

    void set_position(const Eigen::Vector3f &position);
    
    /**
     * @brief Set absolute rotation of the camera, in euler angles
     * (0,0,0) corresponds to the camera looking along the positive z axis, with the positive y axis as up, and the positive x axis as right
     * 
     * @param euler_angles 
     */
    void set_rotation(const Eigen::Vector3f euler_angles);

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
    void rotate(angle_t roll, angle_t pitch, angle_t yaw);

    /**
     * the transformation matrix from world space to camera space
     */
    Eigen::Matrix4f get_view() const;

    /**
     * the transformation matrix from camera space to clip space
     */
    virtual Eigen::Matrix4f get_projection() const = 0;

    Eigen::Vector3f get_screen_center() const;
    
    /**
     * @brief get the position of the top left corner of the "screen", in world space
     * 
     * @return Eigen::Vector3f 
     */
    virtual Eigen::Vector3f get_screen_top_left() const;
    virtual Eigen::Vector3f get_screen_top_right() const;
    virtual Eigen::Vector3f get_screen_bottom_left() const;
    virtual Eigen::Vector3f get_screen_bottom_right() const;

    virtual float get_screen_width() const = 0;
    virtual float get_screen_height() const = 0;
    

};

class PerspectiveCamera : public Camera
{

private:
    float fov;
    Eigen::Matrix4f projection;
    bool proj_dirty = false;
public:
    /**
    * By default, the camera is at the origin, looking along the positive z axis, with the positive y axis as up, and the positive x axis as right
    */
    PerspectiveCamera(angle_t fov = 45.0_deg, float aspectRatio = 1.0f, float near = 0.1f, float far = 1000.0f);

    void set_fov(angle_t fov);

    angle_t get_fov() const;

    void set_near(float near) override;
    void set_far(float far) override;

    /**
     * @brief create a ray from the camera to the given pixel
     * 
     * @param u texture coordinate u, 0 <= u <= 1, and 0 is the left edge of the image
     * @param v texture coordinate v, 0 <= v <= 1, and 0 is the bottom edge of the image
     * @return Ray 
     */
    Ray get_ray(float u, float v) const;

    Eigen::Matrix4f get_projection() const override;
    float get_screen_width() const override;
    float get_screen_height() const override;
};

class OrthographicCamera : public Camera
{
    float width;
private:
    OrthographicCamera(float width, float height, float near, float far);
    Eigen::Matrix4f get_projection() const override;
    float get_screen_width() const override;
    float get_screen_height() const override;
};