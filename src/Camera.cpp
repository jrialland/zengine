#include "Camera.hpp"

void Camera::set_near(float near)
{
    this->near = near;
}

void Camera::set_far(float far)
{
    this->far = far;
}

float Camera::get_near() const
{
    return near;
}

float Camera::get_far() const
{
    return far;
}

Eigen::Vector3f Camera::get_position() const
{
    return position;
}

void Camera::set_position(const Eigen::Vector3f &position_)
{
    position = position_;
}

void Camera::set_rotation(const Eigen::Vector3f euler_angles)
{
    auto rotation = Eigen::AngleAxisf(euler_angles.x(), Eigen::Vector3f::UnitX()) * Eigen::AngleAxisf(euler_angles.y(), Eigen::Vector3f::UnitY()) * Eigen::AngleAxisf(euler_angles.z(), Eigen::Vector3f::UnitZ());
    direction = rotation * Eigen::Vector3f::UnitZ();
    up = rotation * Eigen::Vector3f::UnitY();
    right = rotation * Eigen::Vector3f::UnitX();
}

Eigen::Vector3f Camera::get_direction() const
{
    return direction;
}

Eigen::Vector3f Camera::get_up() const
{
    return up;
}

Eigen::Vector3f Camera::get_right() const
{
    return right;
}

void Camera::set_aspect(float aspectRatio_)
{
    aspectRatio = aspectRatio_;
}

void Camera::set_aspect(int width, int height)
{
    set_aspect(static_cast<float>(width) / static_cast<float>(height));
}

void Camera::set_aspect(const Eigen::Vector2i &size)
{
    set_aspect(size.x(), size.y());
}

float Camera::get_aspect() const
{
    return aspectRatio;
}

void Camera::move(const Eigen::Vector3f &offset)
{
    position += offset;
}

void Camera::move_forward(float offset)
{
    position += offset * direction;
}

void Camera::move_right(float offset)
{
    position += offset * right;
}

void Camera::move_up(float offset)
{
    position += offset * up;
}

void Camera::look_at(const Eigen::Vector3f &target)
{
    if (target == position)
    {
        return;
    }
    direction = (target - position).normalized();
    right = direction.cross(Eigen::Vector3f::UnitY());
    up = right.cross(direction);
}

void Camera::rotate(angle_t roll, angle_t pitch, angle_t yaw)
{
    Eigen::AngleAxisf rollAngle(roll, direction);
    Eigen::AngleAxisf pitchAngle(pitch, right);
    Eigen::AngleAxisf yawAngle(yaw, up);
    Eigen::Quaternionf q = rollAngle * pitchAngle * yawAngle;
    direction = q * direction;
    right = q * right;
    up = q * up;
}

Eigen::Matrix4f Camera::get_view() const
{
    return ::look_at(position, position + direction, up);
}

Eigen::Vector3f Camera::get_screen_center() const
{
    return position + direction * near;
}

Eigen::Vector3f Camera::get_screen_top_left() const
{
    return get_screen_center() + up * get_screen_height()/2 - right * get_screen_width()/2;
}

Eigen::Vector3f Camera::get_screen_top_right() const
{
    return get_screen_center() + up * get_screen_height()/2 + right * get_screen_width()/2;
}
Eigen::Vector3f Camera::get_screen_bottom_left() const
{
    return get_screen_center() - up * get_screen_height()/2 - right * get_screen_width()/2;
}

Eigen::Vector3f Camera::get_screen_bottom_right() const
{
    return get_screen_center() - up * get_screen_height()/2 + right * get_screen_width()/2;
}

PerspectiveCamera::PerspectiveCamera(angle_t fov_, float aspectRatio, float near, float far) : fov(fov_)
{
    set_aspect(aspectRatio);
    set_near(near);
    set_far(far);
    projection = create_perspective_projection(fov, aspectRatio, near, far);
}

void PerspectiveCamera::set_near(float near) {
    Camera::set_near(near);
    proj_dirty = true;
}

void PerspectiveCamera::set_far(float far)  {
    Camera::set_far(far);
    proj_dirty = true;
}

void PerspectiveCamera::set_fov(angle_t fov)
{
    this->fov = fov;
    proj_dirty = true;
}

angle_t PerspectiveCamera::get_fov() const
{
    return fov;
}

Eigen::Matrix4f PerspectiveCamera::get_projection() const
{
    if(proj_dirty) {
        PerspectiveCamera* nonconst = const_cast<PerspectiveCamera*>(this);
        nonconst->projection = create_perspective_projection(fov, aspectRatio, near, far);
        nonconst->proj_dirty = false;
    }
    return projection;
}

float PerspectiveCamera::get_screen_width() const
{
    return get_screen_height() * aspectRatio;
}

float PerspectiveCamera::get_screen_height() const
{
    return 2 * tan(fov / 2) * near;
}


Ray PerspectiveCamera::get_ray(float u, float v) const
{
    auto top_left = get_screen_top_left();
    auto target = top_left + right * get_screen_width() * u - up * get_screen_height() * v;
    return Ray(position, (target - position).normalized());
}

OrthographicCamera::OrthographicCamera(float width_, float height, float near = 0.1f, float far = 1000.0f) : width(width_)
{
    aspectRatio = width / height;
    direction = Eigen::Vector3f::UnitZ();
    right = Eigen::Vector3f::UnitX();
    up = Eigen::Vector3f::UnitY();
    set_near(near);
    set_far(far);
}

Eigen::Matrix4f OrthographicCamera::get_projection() const{
    return create_orthographic_projection(-width / 2, width/2, -width / 2 / aspectRatio, width / 2 / aspectRatio, near, far);
}

float OrthographicCamera::get_screen_width() const {
    return width;
}

float OrthographicCamera::get_screen_height() const {
    return width / aspectRatio;
}