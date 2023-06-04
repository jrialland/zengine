#pragma once

#include <string>
#include <vector>
#include <iostream>

#include <Eigen/Dense>

struct VertexStructure
{
    unsigned int offset=0; // the offset between the beginning of the data and the beginning of this attribute
    unsigned int size=0;   // the size of The used attribute in bytes
    unsigned int components_count=0;  // the number of elements in this attribute
    unsigned int gl_type=0; // the type of the attribute in opengl
    unsigned int stride=0; // the size of the entire vertex in bytes

    /* a small dsl that helps defining the format of a vertex buffer.
     * the format is made of space separated attributes
     * - the first character is the type of the attribute (f for float, i for int, u for unsigned int, b for byte, v2 for 2 floats, v3 for 3 floats, v4 for 4 floats, m2 for 2x2 matrix, m3 for 3x3 matrix, m4 for 4x4 matrix)
     * - if an attribute is followed by a number between square brackets, it means that the attribute is an array of that many elements.
     * - the format string can begin or end with attributes between parenthesis, meaning that they are part of the overall buffer but not part of the attribute (see below)
     * Examples:
     * <pre>
     * "f[3]" // a float attribute of 3 elements => offset=0, size=12, count=3, stride=12
     * "f[3] v3" // => offset=0, size=24, count=2, stride=24
     * "(f[3] v3) m4 (f[12])" // => offset=24 , size=64, count=1, stride=136
     * <pre>
     *
     * For a buffer containing a 3d position, a normal, and a texture coordinate for each vertex bound respectively to attributes 0, 1 and 2, the format strings would be:
     * <pre>
     *   0: "v3 (v3 v2)"
     *   1: "(v3) v3 (v2)"
     *   2: "(v3 v3) v2" or even "(b[24]) v2" which leads to the same result
     * <pre>
     *
     * In this context a glsl declaration of the vertex shader would be:
     * <pre>
     *  layout(location = 0) in vec3 position;
     *  layout(location = 1) in vec3 normal;
     *  layout(location = 2) in vec2 texcoord;
     * <pre>
     */
    static VertexStructure parse_format(const std::string &format);

    bool operator==(const VertexStructure &other) const;
    bool operator!=(const VertexStructure &other) const;
};
std::ostream &operator<<(std::ostream &os, const VertexStructure &vs);
namespace std
{
    string to_string(const VertexStructure &vs);
}

class VertexBuffer
{
private:
    unsigned int id;
    unsigned int size; // in bytes
    std::string format;
    friend class VertexArray;
public:
    VertexBuffer();
    VertexBuffer(const void *data, unsigned int size, const std::string &format = "f");
    VertexBuffer(const std::vector<Eigen::Vector3f> &data);
    VertexBuffer(const std::vector<uint32_t> &data);
    VertexBuffer(const VertexBuffer &buffer) = delete;
    VertexBuffer(VertexBuffer &&buffer);
    ~VertexBuffer();

    void set_data(const void *data, unsigned int size, const std::string &format = "f");

    /**
     * the size of the buffer in bytes
    */
    unsigned int get_size() const;

    /**
     * the format of the buffer, in the same format as VertexStructure::parse_format
    */
    std::string get_format() const;

    /*
     * the size of a single vertex in bytes 
    */
    size_t component_size() const;

    /*
     * the number of vertices in the buffer, (computed as size / component_size())
    */
    size_t count() const;
};
