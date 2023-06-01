#include "Geometry.hpp"
#include <fmt/format.h>

void Geometry::recompute_normals()
{
    normals.clear();
    normals.resize(vertices.size(), Eigen::Vector3d::Zero());
    for (int i = 0; i < indices.size(); i += 3)
    {
        Eigen::Vector3d a = vertices[indices[i]];
        Eigen::Vector3d b = vertices[indices[i + 1]];
        Eigen::Vector3d c = vertices[indices[i + 2]];
        Eigen::Vector3d normal = (b - a).cross(c - a);
        normals[indices[i]] += normal;
        normals[indices[i + 1]] += normal;
        normals[indices[i + 2]] += normal;
    }
    for (auto &normal : normals)
    {
        normal.normalize();
    }
}

void Geometry::transform(const Eigen::Matrix4d &transform)
{
    for (auto &vertex : vertices)
    {
        vertex = transform * vertex;
    }
}

void Geometry::translate(const Eigen::Vector3d &translation)
{
    for (auto &vertex : vertices)
    {
        vertex += translation;
    }
}

void Geometry::scale(const Eigen::Vector3d &scale)
{
    for (auto &vertex : vertices)
    {
        vertex.x() *= scale.x();
        vertex.y() *= scale.y();
        vertex.z() *= scale.z();
    }
}

void Geometry::rotate(const Eigen::Vector3d &axis, double angle)
{
    Eigen::Matrix3d rotation = Eigen::AngleAxisd(angle, axis).toRotationMatrix();
    rotate(rotation);
}

void Geometry::rotate(const Eigen::Matrix3d &rotation)
{
    for (auto &vertex : vertices)
    {
        vertex = rotation * vertex;
    }
}

void Geometry::merge(const Geometry &other)
{
    GeometryBuilder builder;
    builder.add_geometry(*this);
    builder.add_geometry(other);
    *this = builder.build();
}

Geometry Geometry::transformed(const Eigen::Matrix4d &transform) const
{
    Geometry copy = this->copy();
    copy.transform(transform);
    return copy;
}

Geometry Geometry::copy() const
{
    Geometry copy;
    copy.vertices = vertices;
    copy.indices = indices;
    copy.normals = normals;
    return copy;
}

void Geometry::to_obj(std::ostream &out, bool with_normals)
{
    for (auto vertex : vertices)
    {
        out << "v " << vertex.x() << " " << vertex.y() << " " << vertex.z() << std::endl;
    }
    if (normals)
    {
        recompute_normals();
    }
    for (auto normal : normals)
    {
        out << "vn " << normal.x() << " " << normal.y() << " " << normal.z() << std::endl;
    }
    for (int i = 0; i < indices.size(); i += 3)
    {
        out << "f " << indices[i] + 1 << "//" << indices[i] + 1 << " " << indices[i + 1] + 1 << "//" << indices[i + 1] + 1 << " " << indices[i + 2] + 1 << "//" << indices[i + 2] + 1 << std::endl;
    }
}

int GeometryBuilder::add_vertex(const Eigen::Vector3d &vertex)
{
    std::string key = fmt::format("{:.6f},{:.6f},{:.6f}", vertex.x(), vertex.y(), vertex.z());
    auto it = vertexMap.find(key);
    if (it != vertexMap.end())
    {
        return it->second.index;
    }
    else
    {
        int index = vertices.size();
        vertexMap[key] = {index, vertex};
        vertices.push_back(vertex);
        return index;
    }
}

void GeometryBuilder::add_geometry(const Geometry &geometry)
{
    for (int index : geometry.indices)
    {
        indices.push_back(add_vertex(geometry.vertices[index]));
    }
}

Geometry GeometryBuilder::build()
{
    std::vector<Eigen::Vector3d> vertices;
    for (auto &pair : vertexMap)
    {
        vertices.push_back(pair.second.vertex);
    }
    assert(indices.size() % 3 == 0) return {vertices, indices};
}

const Eigen::Vector3d X_AXIS(1, 0, 0);
const Eigen::Vector3d Y_AXIS(0, 1, 0);
const Eigen::Vector3d Z_AXIS(0, 0, 1);

namespace basegeometries
{

    Geometry triangle(const Eigen::Vector3d &a, const Eigen::Vector3d &b, const Eigen::Vector3d &c)
    {
        return Geometry({a, b, c}, {0, 1, 2});
    }

    Geometry quad(const Eigen::Vector3d &a, const Eigen::Vector3d &b, const Eigen::Vector3d &c, const Eigen::Vector3d &d)
    {
        return Geometry({a, b, c, d}, {0, 1, 2, 2, 3, 0});
    }

    Geometry cube()
    {
        Eigen::Vector3d a = {-0.5, -0.5, -0.5};
        Eigen::Vector3d b = {0.5, -0.5, -0.5};
        Eigen::Vector3d c = {0.5, 0.5, -0.5};
        Eigen::Vector3d d = {-0.5, 0.5, -0.5};
        Eigen::Vector3d e = {-0.5, -0.5, 0.5};
        Eigen::Vector3d f = {0.5, -0.5, 0.5};
        Eigen::Vector3d g = {0.5, 0.5, 0.5};
        Eigen::Vector3d h = {-0.5, 0.5, 0.5};
        GeometryBuilder builder;
        builder.add_geometry(quad(a, b, c, d)); // front
        builder.add_geometry(quad(e, f, g, h)); // back
        builder.add_geometry(quad(a, d, h, e)); // left
        builder.add_geometry(quad(b, f, g, c)); // right
        builder.add_geometry(quad(d, c, g, h)); // top
        builder.add_geometry(quad(a, b, f, e)); // bottom
        return builder.build();
    }

    Geometry disc(int n)
    {
        assert(n >= 3);
        if (n == 3)
        {
            return triangle(
                {1, 0, 0},
                {cos(M_PI_2 / 3), sin(M_PI_2 / 3), 0},
                {cos(2 * M_PI_2 / 3), sin(2 * M_PI_2 / 3), 0})
        }
        else if (n == 4)
        {
            return quad(
                {1, 0, 0},
                {cos(M_PI_2 / 2), sin(M_PI_2 / 2), 0},
                {cos(M_PI_2), sin(M_PI_2), 0},
                {cos(3 * M_PI_2 / 2), sin(3 * M_PI_2 / 2), 0});
        }
        else
        {
            std::vector<Eigen::Vector3d> vertices;
            std::vector<uint32_t> indices;
            vertices.push_back(Eigen::Vector3d::Zero());
            for (int i = 0; i < n; i++)
            {
                auto angle = 2 * M_PI * i / n;
                auto vertex = Eigen::Vector3d(cos(angle), sin(angle), 0);
                vertices.push_back(vertex);
                indices.push_back(0);
                indices.push_back(i + 1);
                indices.push_back((i + 1) % n + 1);
            }
            return {vertices, indices};
        }
    }

}