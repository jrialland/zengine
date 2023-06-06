#include "CpuRayTracer.hpp"

#include <cstdlib> // rand()
#include <cmath>
#include <fstream>

CpuRayTracer::CpuRayTracer(std::shared_ptr<PerspectiveCamera> camera_) : camera(camera_) {

}

void CpuRayTracer::add_geometry(const Geometry& geometry, const Eigen::Matrix4f& transform) {
    auto g = std::make_shared<Geometry>(geometry);
    g->transform(transform);
    geometries.push_back(g);
}

void CpuRayTracer::set_sun_direction(const Eigen::Vector3f& sun_direction) {
    this->sun_direction = sun_direction;
}

void CpuRayTracer::set_camera(std::shared_ptr<PerspectiveCamera> camera) {
    this->camera = camera;
}

std::shared_ptr<PerspectiveCamera> CpuRayTracer::get_camera() const {
    return camera;
}

static inline float random_float() {
    return rand() / (RAND_MAX + 1.0);
}

/**
 * @brief save the image to a file (in ppm format)
 * 
 * @param filename file name
 * @param pixels float colors of the pixels (in range [0, 1])
 * @param width of the image
 * @param height of the image
 */
void save_image(const std::string &filename, std::vector<Eigen::Vector3f>& pixels, int width, int height) {
    std::ofstream f(filename.c_str(), std::ios::out | std::ios::binary);
    f << "P2\n" << width << " " << height << "\n255\n";
    for (int i = 0; i < width * height; ++i) {
        Eigen::Vector3f pixel = pixels[i];
        float intensity = std::clamp((pixel.x() + pixel.y() + pixel.z()) / 3.0f, 0.0f, 1.0f);
        uint8_t color = intensity * 255;
        f << (int)color << " ";
        if(i % 10 == 0) {
            f << "\n";
        }
    }
    f.close();
}

struct HitRecord {
    float t;
    Eigen::Vector3f normal;
};

bool hit(const Ray& ray, const std::vector<std::shared_ptr<Geometry>>& geometries, HitRecord& hit_record) {
    bool hit_anything = false;
    float closest_so_far = std::numeric_limits<float>::max();
    for(const auto& geometry : geometries) {
        float t;
        Eigen::Vector3f normal;
        if(geometry->hit(ray, t, normal) && t < closest_so_far) {
            hit_anything = true;
            closest_so_far = t;
            hit_record.t = t;
            hit_record.normal = normal;
        }
    }
    return hit_anything;
}

Eigen::Vector3f ray_color(const Ray& ray, const std::vector<std::shared_ptr<Geometry>>& geometries, const Eigen::Vector3f& sun_direction) {
    HitRecord hit_record;
    if(hit(ray, geometries, hit_record)) {
        Eigen::Vector3f color = hit_record.normal.normalized();
        float intensity = std::max(0.0f, hit_record.normal.dot(-sun_direction));
        color *= intensity;
        return color;
    }
    Eigen::Vector3f unit_direction = ray.direction.normalized();
    float t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - t) * Eigen::Vector3f(1.0, 1.0, 1.0) + t * Eigen::Vector3f(0.5, 0.7, 1.0);
}

void CpuRayTracer::render(const std::string& filename, int width, int height, int samples) {
    std::vector<Eigen::Vector3f> framebuffer(width * height);
    for(int j = 0; j < height; j++) {
        for(int i = 0; i < width; i++) {
            Eigen::Vector3f color(0, 0, 0);
            for(int s = 0; s < samples; s++) {
                float u = (i + random_float()) / width;
                float v = (j + random_float()) / height;
                Ray ray = camera->get_ray(u, v);
                color += ray_color(ray, geometries, sun_direction);
            }
            color /= samples;
            color = Eigen::Vector3f(sqrt(color.x()), sqrt(color.y()), sqrt(color.z()));
            framebuffer[i + j * width] = color;
        }
    }
    save_image(filename, framebuffer, width, height);
}