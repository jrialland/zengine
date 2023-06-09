#include "Geometry.hpp"
#include <fmt/format.h>

void Geometry::recompute_normals()
{
    normals.clear();
    normals.resize(vertices.size(), Eigen::Vector3f::Zero());
    for (int i = 0; i < indices.size(); i += 3)
    {
        Eigen::Vector3f a = vertices[indices[i]];
        Eigen::Vector3f b = vertices[indices[i + 1]];
        Eigen::Vector3f c = vertices[indices[i + 2]];
        Eigen::Vector3f normal = (b - a).cross(c - a);
        normals[indices[i]] += normal;
        normals[indices[i + 1]] += normal;
        normals[indices[i + 2]] += normal;
    }
    for (auto &normal : normals)
    {
        normal.normalize();
    }
}

void Geometry::transform(const Eigen::Matrix4f &transform)
{
    for (auto &vertex : vertices)
    {
        vertex = (transform * vertex.homogeneous()).hnormalized();
    }
    normals.clear();
}

void Geometry::translate(const Eigen::Vector3f &translation)
{
    for (auto &vertex : vertices)
    {
        vertex += translation;
    }
    // normals are not affected by translation
}

void Geometry::scale(const Eigen::Vector3f &scale)
{
    for (auto &vertex : vertices)
    {
        vertex.x() *= scale.x();
        vertex.y() *= scale.y();
        vertex.z() *= scale.z();
    }
    // normals are not affected by scaling
}

void Geometry::rotate(const Eigen::Vector3f &axis, double angle)
{
    Eigen::Matrix3f rotation = Eigen::AngleAxisf(angle, axis).toRotationMatrix();
    rotate(rotation);
}

void Geometry::rotate(const Eigen::Matrix3f &rotation)
{
    for (auto &vertex : vertices)
    {
        vertex = rotation * vertex;
    }
    normals.clear();
}

void Geometry::merge(const Geometry &other)
{
    GeometryBuilder builder;
    builder.add_geometry(*this);
    builder.add_geometry(other);
    *this = builder.build();
    normals.clear();
}

Geometry Geometry::transformed(const Eigen::Matrix4f &transform) const
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

void Geometry::to_obj(std::ostream &out, const std::string &name, bool with_normals)
{
    if (!name.empty())
    {
        out << "o " << name << std::endl;
    }

    for (auto vertex : vertices)
    {
        out << "v " << vertex.x() << " " << vertex.y() << " " << vertex.z() << std::endl;
    }
    if (with_normals)
    {
        if (normals.empty())
        {
            recompute_normals();
        }
        for (auto normal : normals)
        {
            out << "vn " << normal.x() << " " << normal.y() << " " << normal.z() << std::endl;
        }
    }

    for (int i = 0; i < indices.size(); i += 3)
    {
        out << "f " << indices[i] + 1 << "//" << indices[i] + 1 << " " << indices[i + 1] + 1 << "//" << indices[i + 1] + 1 << " " << indices[i + 2] + 1 << "//" << indices[i + 2] + 1 << std::endl;
    }
}

void Geometry::for_each_triangle(std::function<bool(const Eigen::Vector3f &, const Eigen::Vector3f &, const Eigen::Vector3f &)> callback) const
{
    for (int i = 0; i < indices.size(); i += 3)
    {
        bool go_on = callback(vertices[indices[i]], vertices[indices[i + 1]], vertices[indices[i + 2]]);
        if (!go_on)
        {
            break;
        }
    }
}

void Geometry::for_each_triangle(std::function<bool(Eigen::Vector3f &, Eigen::Vector3f &, Eigen::Vector3f &)> callback)
{
    for (int i = 0; i < indices.size(); i += 3)
    {
        bool go_on = callback(vertices[indices[i]], vertices[indices[i + 1]], vertices[indices[i + 2]]);
        if (!go_on)
        {
            break;
        }
    }
}

Eigen::Vector3f Geometry::get_centroid() const
{
    Eigen::Vector3f centroid = Eigen::Vector3f::Zero();
    for (auto &vertex : vertices)
    {
        centroid += vertex;
    }
    centroid /= vertices.size();
    return centroid;
}

static inline float sign(const Eigen::Vector2f &p1, const Eigen::Vector2f &p2, const Eigen::Vector2f &p3)
{
    return (p1.x() - p3.x()) * (p2.y() - p3.y()) - (p2.x() - p3.x()) * (p1.y() - p3.y());
}

static inline bool point_in_triangle(const Eigen::Vector2f &pt, const Eigen::Vector2f &v1, const Eigen::Vector2f &v2, const Eigen::Vector2f &v3)
{
    float d1 = sign(pt, v1, v2);
    float d2 = sign(pt, v2, v3);
    float d3 = sign(pt, v3, v1);
    bool has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
    bool has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);
    return !(has_neg && has_pos);
}

bool Geometry::hit(const Ray &ray, float &t, Eigen::Vector3f &normal) const
{

    struct Hit
    {
        float t;
        Eigen::Vector3f normal;
    };

    std::vector<Hit> hits;
    // transform vertices to ray space
    Eigen::Vector3f right = ray.direction.isApprox(Eigen::Vector3f::UnitY()) ? Eigen::Vector3f::UnitX() : ray.direction.cross(Eigen::Vector3f::UnitY());
    Eigen::Vector3f up = right.cross(ray.direction);
    Eigen::Matrix4f transform;
    transform.setZero();
    transform(0, 0) = right.x();
    transform(0, 1) = right.y();
    transform(0, 2) = right.z();
    transform(1, 0) = up.x();
    transform(1, 1) = up.y();
    transform(1, 2) = up.z();
    transform(2, 0) = ray.direction.x();
    transform(2, 1) = ray.direction.y();
    transform(2, 2) = ray.direction.z();
    transform(0, 3) = -right.dot(ray.origin);
    transform(1, 3) = -up.dot(ray.origin);
    transform(2, 3) = -ray.direction.dot(ray.origin);
    transform(3, 3) = 1;

    for_each_triangle([ray, &hits, &transform](const Eigen::Vector3f &a, const Eigen::Vector3f &b, const Eigen::Vector3f &c)
                      {
        static const Eigen::Vector2f origin(0, 0);
        auto at = (transform * a.homogeneous()).head<3>();
        auto bt = (transform * b.homogeneous()).head<3>();
        auto ct = (transform * c.homogeneous()).head<3>();
        if(point_in_triangle(origin, at.head<2>(), bt.head<2>(), ct.head<2>())){
            float distance_a = sqrtf(at.x() * at.x() + at.y() * at.y());
            float distance_b = sqrtf(bt.x() * bt.x() + bt.y() * bt.y());
            float distance_c = sqrtf(ct.x() * ct.x() + ct.y() * ct.y());
            float t = (at.z() * distance_a + bt.z() * distance_b + ct.z() * distance_c) / (distance_a + distance_b + distance_c);
            Eigen::Vector3f normal = (bt - at).cross(ct - at).normalized();
            hits.push_back({t, normal});
        }
        return true; });
    
    // no hits
    if (hits.empty())
    {
        t = -1;
        return false;
    }

    // sort hits by distance
    std::sort(hits.begin(), hits.end(), [](const Hit &a, const Hit &b)
              { return a.t < b.t; });
    t = hits[0].t;
    normal = hits[0].normal;
    return true;
}

bool GeometryBuilder::TriangleIndices::operator<(const GeometryBuilder::TriangleIndices &other) const
{
    if (a < other.a)
    {
        return true;
    }
    else if (a == other.a)
    {
        if (b < other.b)
        {
            return true;
        }
        else if (b == other.b)
        {
            return c < other.c;
        }
    }
    return false;
}

uint32_t GeometryBuilder::get_index(const Eigen::Vector3f &vertex)
{
    std::string key = fmt::format("{:.6f},{:.6f},{:.6f}", vertex.x(), vertex.y(), vertex.z());
    auto it = vertexMap.find(key);
    if (it != vertexMap.end())
    {
        return it->second.index;
    }
    else
    {
        uint32_t index = vertexMap.size();
        vertexMap[key] = VertexAndIndex{vertex, index};
        return index;
    }
}

void GeometryBuilder::add_triangle(const Eigen::Vector3f &a, const Eigen::Vector3f &b, const Eigen::Vector3f &c)
{
    triangles.insert({get_index(a), get_index(b), get_index(c)});
}

void GeometryBuilder::add_geometry(const Geometry &geometry)
{
    geometry.for_each_triangle([this](const Eigen::Vector3f &a, const Eigen::Vector3f &b, const Eigen::Vector3f &c)
                               { add_triangle(a, b, c); return true; });
}

Geometry GeometryBuilder::build()
{
    Geometry geometry;
    for (auto &pair : vertexMap)
    {
        geometry.vertices.push_back(pair.second.vertex);
    }
    for (auto &triangle : triangles)
    {
        geometry.indices.push_back(triangle.a);
        geometry.indices.push_back(triangle.b);
        geometry.indices.push_back(triangle.c);
    }
    return geometry;
}

const Eigen::Vector3f X_AXIS(1, 0, 0);
const Eigen::Vector3f Y_AXIS(0, 1, 0);
const Eigen::Vector3f Z_AXIS(0, 0, 1);

namespace geometryops
{

    Geometry extrude(const Geometry &base, const Eigen::Vector3f &direction)
    {
        GeometryBuilder builder;
        base.for_each_triangle([&](const Eigen::Vector3f &a, const Eigen::Vector3f &b, const Eigen::Vector3f &c)
                               {
            builder.add_triangle(a, b, c);
            builder.add_geometry(basegeometries::quad(a, b, b + direction, a + direction));
            builder.add_geometry(basegeometries::quad(b, c, c + direction, b + direction));
            builder.add_geometry(basegeometries::quad(c, a, a + direction, c + direction));
            builder.add_triangle(a + direction, b + direction, c+direction); 
            return true; });
        return builder.build();
    }
}

namespace basegeometries
{

    Geometry triangle(const Eigen::Vector3f &a, const Eigen::Vector3f &b, const Eigen::Vector3f &c)
    {
        return Geometry{{a, b, c}, {0, 1, 2}};
    }

    Geometry quad(const Eigen::Vector3f &a, const Eigen::Vector3f &b, const Eigen::Vector3f &c, const Eigen::Vector3f &d)
    {
        return Geometry{{a, b, c, d}, {0, 1, 2, 2, 3, 0}};
    }

    Geometry cube()
    {
        return Geometry{
            {
                {-0.5, -0.5, -0.5},
                {0.5, -0.5, -0.5},
                {0.5, 0.5, -0.5},
                {-0.5, 0.5, -0.5},
                {-0.5, -0.5, 0.5},
                {0.5, -0.5, 0.5},
                {0.5, 0.5, 0.5},
                {-0.5, 0.5, 0.5},
            },
            {
                0, 1, 2, 2, 3, 0, // front
                1, 5, 6, 6, 2, 1, // right
                5, 4, 7, 7, 6, 5, // back
                4, 0, 3, 3, 7, 4, // left
                3, 2, 6, 6, 7, 3, // top
                4, 5, 1, 1, 0, 4, // bottom
            }};
    }

    Geometry disc(int n)
    {
        assert(n >= 3);
        if (n == 3)
        {
            return triangle(
                {1, 0, 0},
                {cos(M_PI_2 / 3), sin(M_PI_2 / 3), 0},
                {cos(2 * M_PI_2 / 3), sin(2 * M_PI_2 / 3), 0});
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
            std::vector<Eigen::Vector3f> vertices;
            std::vector<uint32_t> indices;
            vertices.push_back(Eigen::Vector3f::Zero());
            for (int i = 0; i < n; i++)
            {
                auto angle = 2 * M_PI * i / n;
                auto vertex = Eigen::Vector3f(cos(angle), sin(angle), 0);
                vertices.push_back(vertex);
                indices.push_back(0);
                indices.push_back(i + 1);
                indices.push_back((i + 1) % n + 1);
            }
            return {vertices, indices};
        }
    }

}