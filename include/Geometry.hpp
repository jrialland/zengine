#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>
#include <iostream>
#include <functional>
#include <utility>
#include <Eigen/Dense>

#include "Ray.hpp"

struct Geometry {
    std::vector<Eigen::Vector3f> vertices;
    std::vector<uint32_t> indices;
    std::vector<Eigen::Vector3f> normals;
    void recompute_normals();
    void transform(const Eigen::Matrix4f& transform);
    void translate(const Eigen::Vector3f& translation);
    void scale(const Eigen::Vector3f& scale);
    void rotate(const Eigen::Vector3f& axis, double angle);
    void rotate(const Eigen::Matrix3f& rotation);
    void merge(const Geometry& other);
    Geometry transformed(const Eigen::Matrix4f& transform) const;
    Geometry copy() const;
    void to_obj(std::ostream& out, const std::string& name="", bool with_normals = false);
    void for_each_triangle(std::function<bool(const Eigen::Vector3f&, const Eigen::Vector3f&, const Eigen::Vector3f&)> callback) const;
    void for_each_triangle(std::function<bool(Eigen::Vector3f&, Eigen::Vector3f&, Eigen::Vector3f&)> callback);
    Eigen::Vector3f get_centroid() const;

    /**
     * @brief check if a ray hits the geometry
     * 
     * This method checks for every triangles in a way that is not very efficient, so it is for validating the correctness of geometries only.
     * 
     * @param ray casted ray
     * @param t the distance from the ray origin to the hit point, set to -1 if no hit
     * @param normal the normal of the hit point, set to (0, 0, 0) if no hit
     * @return true as long as the ray hits the geometry
     * @return false otherwise
     */
    bool hit(const Ray& ray, float &t, Eigen::Vector3f &normal) const;
};

struct GeometryBuilder {
    struct VertexAndIndex {
        Eigen::Vector3f vertex;
        uint32_t index;
    };
    struct TriangleIndices {
        uint32_t a;
        uint32_t b;
        uint32_t c;
        bool operator<(const TriangleIndices& other) const;
    };
    std::map<std::string, VertexAndIndex> vertexMap;
    std::set<TriangleIndices> triangles;
    void add_triangle(const Eigen::Vector3f& a, const Eigen::Vector3f& b, const Eigen::Vector3f& c);
    void add_geometry(const Geometry& geometry);
    Geometry build();
    private:

    uint32_t get_index(const Eigen::Vector3f& vertex);
};

extern const Eigen::Vector3f X_AXIS;
extern const Eigen::Vector3f Y_AXIS;
extern const Eigen::Vector3f Z_AXIS;

namespace geometryops {
    /**
     * @brief Extrudes a polygon along a direction
    */
    Geometry extrude(const Geometry& base, const Eigen::Vector3f &direction);
}

namespace basegeometries {

    Geometry triangle(const Eigen::Vector3f& a, const Eigen::Vector3f& b, const Eigen::Vector3f& c);
    Geometry quad(const Eigen::Vector3f& a, const Eigen::Vector3f& b, const Eigen::Vector3f& c, const Eigen::Vector3f& d);

    /*
    * returns a cube of size 1 centered at the origin
    */
    Geometry cube();

    Geometry disc(int n = 32);

    /*
    * returns a sphere of radius 1 centered at the origin
    */
    Geometry sphere(int n = 32);

    /*
    * returns a cylinder of radius 1 and height 1 centered at the origin
    */
    Geometry cylinder(int n = 32);
    Geometry cone(int n = 32);
    Geometry torus(int n = 32, int m = 32);
}