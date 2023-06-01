#include <vector>
#include <Eigen/Dense>
#include <map>
#include <iostream>
struct Geometry {
    std::vector<Eigen::Vector3d> vertices;
    std::vector<uint32_t> indices;
    std::vector<Eigen::Vector3d> normals;
    void recompute_normals();
    void transform(const Eigen::Matrix4d& transform);
    void translate(const Eigen::Vector3d& translation);
    void scale(const Eigen::Vector3d& scale);
    void rotate(const Eigen::Vector3d& axis, double angle);
    void rotate(const Eigen::Matrix3d& rotation);
    void merge(const Geometry& other);
    Geometry transformed(const Eigen::Matrix4d& transform) const;
    Geometry copy() const;
    void to_obj(std::ostream& out, bool with_normals = false);
};

struct GeometryBuilder {
    struct VertexAndIndex {
        Eigen::Vector3d vertex;
        uint32_t index;
    };
    std::map<std::string, VertexAndIndex> vertexMap;
    std::vector<uint32_t> indices;
    void add_geometry(const Geometry& geometry);
    Geometry build();
    private:
    int add_vertex(const Eigen::Vector3d& vertex);
};

const Eigen::Vector3d X_AXIS;
const Eigen::Vector3d Y_AXIS;
const Eigen::Vector3d Z_AXIS;

namespace basegeometries {

    Geometry triangle(const Eigen::Vector3d& a, const Eigen::Vector3d& b, const Eigen::Vector3d& c);
    Geometry quad(const Eigen::Vector3d& a, const Eigen::Vector3d& b, const Eigen::Vector3d& c, const Eigen::Vector3d& d);

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