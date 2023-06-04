#include "VertexBuffer.hpp"
#include "strutil.hpp"
#include <GL/glew.h>
#include <GL/gl.h>
#include <cassert>
#include <regex>
#include <utility>

static std::pair<std::string, int> parse_attr(const std::string &attr)
{
    static std::regex re("^\\[(\\d+)\\]$");
    std::smatch match;
    if (std::regex_search(attr, match, re))
    {
        int multiplier = std::stoi(match[1]);
        return std::make_pair(attr.substr(0, match.position(0)), multiplier);
    }
    else
    {
        return std::make_pair(attr, 1);
    }
}

VertexStructure VertexStructure::parse_format(const std::string &format_)
{
    std::string format(format_); // copy because we will modify it
    assert(format.size() > 0);
    VertexStructure vs{0, 0, 0, 0};

    // offset
    if (format[0] == '(')
    {
        auto end = format.find(')');
        assert(end != std::string::npos);
        auto offset_vs = parse_format(format.substr(1, end - 1));
        vs.offset = offset_vs.stride;
        vs.stride = offset_vs.stride;
        format = strutil::trim(format.substr(end + 1));
    }

    // check if trailing parenthesed attributes
    std::string end;
    auto paren = format.find('(');
    if (paren != std::string::npos)
    {
        format = format.substr(0, paren);
        end = format.substr(paren + 1);
        auto close_paren = end.find(')');
        assert(close_paren != std::string::npos);
        end = strutil::trim(end.substr(0, close_paren));
    }

    for (auto &attr : strutil::split(format, " "))
    {
        auto [attr_name, attr_multiplier] = parse_attr(attr);
        // (f for float, i for int, u for unsigned int, b for byte, v2 for 2 floats, v3 for 3 floats, v4 for 4 floats, m2 for 2x2 matrix, m3 for 3x3 matrix, m4 for 4x4 matrix)
        switch (attr_name[0])
        {
        case 'f':
            vs.size += 4 * attr_multiplier;
            vs.components_count += attr_multiplier;
            vs.gl_type = GL_FLOAT;
            break;
        case 'i':
            vs.size += 4 * attr_multiplier;
            vs.components_count += attr_multiplier;
            vs.gl_type = GL_INT;
            break;
        case 'u':
            vs.size += 4 * attr_multiplier;
            vs.components_count += attr_multiplier;
            vs.gl_type = GL_UNSIGNED_INT;
            break;
        case 'b':
            vs.size += 1 * attr_multiplier;
            vs.components_count += attr_multiplier;
            vs.gl_type = GL_UNSIGNED_BYTE;
            break;
        case 'v':
            vs.gl_type = GL_FLOAT;
            switch (attr_name[1])
            {
            case '2':
                vs.size += 4 * 2 * attr_multiplier;
                vs.components_count += attr_multiplier * 2;
                break;
            case '3':
                vs.size += 4 * 3 * attr_multiplier;
                vs.components_count += attr_multiplier * 3;
                break;
            case '4':
                vs.size += 4 * 4 * attr_multiplier;
                vs.components_count += attr_multiplier * 4;
                break;
            default:
                assert(false && "Invalid vector size (tip: use f[xx] for vectors > 4)");
            }
            break;
        case 'm':
            vs.gl_type = GL_FLOAT;
            switch (attr_name[1])
            {
            case '2':
                vs.size += 4 * 2 * 2 * attr_multiplier;
                vs.components_count += attr_multiplier * 4;
                break;
            case '3':
                vs.size += 4 * 3 * 3 * attr_multiplier;
                vs.components_count += attr_multiplier * 9;
                break;
            case '4':
                vs.size += 4 * 4 * 4 * attr_multiplier;
                vs.components_count += attr_multiplier * 16;
                break;
            default:
                assert(false && "Invalid matrix size (tip: use f[xx] for matrices > 4x4)");
            }
            break;
        default:
            assert(false && "Invalid attribute type");
        }
    }

    vs.stride += vs.size;

    // end
    if (end.size() > 0)
    {
        auto end_vs = parse_format(end);
        vs.stride += end_vs.stride;
    }
    return vs;
}

bool VertexStructure::operator==(const VertexStructure &other) const
{
    return offset == other.offset && size == other.size && components_count == other.components_count && stride == other.stride && gl_type == other.gl_type;
}

bool VertexStructure::operator!=(const VertexStructure &other) const
{
    return !(*this == other);
}

std::ostream &operator<<(std::ostream &os, const VertexStructure &vs)
{
    os << std::to_string(vs);
    return os;
}

namespace std
{
    string to_string(const VertexStructure &vs)
    {
        return "VertexStructure(offset=" + std::to_string(vs.offset) + ", size=" + std::to_string(vs.size) + ", components_count=" + std::to_string(vs.components_count) + ", stride=" + std::to_string(vs.stride) + ", gl_type=" + std::to_string(vs.gl_type) + ")";
    }
}

VertexBuffer::VertexBuffer()
{
    glGenBuffers(1, &id);
}

VertexBuffer::VertexBuffer(const void *data, unsigned int size, const std::string &format) : VertexBuffer()
{
    set_data(data, size, format);
}

VertexBuffer::VertexBuffer(const std::vector<Eigen::Vector3f> &data) : VertexBuffer()
{
    set_data(data.data(), data.size() * sizeof(Eigen::Vector3f), "3f");
}

VertexBuffer::VertexBuffer(const std::vector<uint32_t> &data) : VertexBuffer()
{
    set_data(data.data(), data.size() * sizeof(uint32_t), "u");
}

VertexBuffer::VertexBuffer(VertexBuffer &&buffer)
{
}
VertexBuffer::~VertexBuffer()
{
    glDeleteBuffers(1, &id);
}

void VertexBuffer::set_data(const void *data, unsigned int size, const std::string &format)
{
    glBindBuffer(GL_ARRAY_BUFFER, id);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    this->format = format;
    this->size = size;
}

unsigned int VertexBuffer::get_size() const
{
    return size;
}

std::string VertexBuffer::get_format() const
{
    return format;
}

/*
 * the size of a single vertex in bytes
 */
size_t VertexBuffer::component_size() const
{
    return VertexStructure::parse_format(format).stride;
}

/*
 * the number of vertices in the buffer, (computed as size / component_size())
 */
size_t VertexBuffer::count() const
{
    return size / component_size();
}