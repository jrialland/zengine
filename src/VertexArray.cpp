#include "Logging.hpp"
#include "VertexArray.hpp"

#include <GL/glew.h>
#include <GL/gl.h>

VertexArray::VertexArray()
{
    glGenVertexArrays(1, &id);
}

VertexArray::VertexArray(std::shared_ptr<ShaderProgram> shader_program)
{
    this->shader_program = shader_program;
}

VertexArray::~VertexArray()
{
    glDeleteVertexArrays(1, &id);
    if(ebo_id != 0)
    {
        glDeleteBuffers(1, &ebo_id);
    }
}

void VertexArray::set_program(std::shared_ptr<ShaderProgram> shader_program)
{
    this->shader_program = shader_program;
}

void VertexArray::set_program(const std::string &vertex_shader, const std::string &fragment_shader) {
    shader_program = std::make_shared<ShaderProgram>(vertex_shader, fragment_shader);
}

std::shared_ptr<ShaderProgram> VertexArray::get_shader_program()
{
    shader_program->use();
    return shader_program;
}

void VertexArray::bind_buffer(int location, const std::string &format, const std::vector<float> &buffer) {
    auto vertex_buffer = std::make_shared<VertexBuffer>(buffer);
    bind_buffer(location, format, vertex_buffer);
}

void VertexArray::bind_buffer(int location, const std::vector<Eigen::Vector3f> &buffer) {
    auto vertex_buffer = std::make_shared<VertexBuffer>(buffer);
    bind_buffer(location, "f3", vertex_buffer);
}

void VertexArray::bind_buffer(int location, const std::string &format, std::shared_ptr<VertexBuffer> buffer, int divisor)
{

    if (location == 0)
    {
        assert(divisor == 0 && "divisor is forbidden for location 0 !");
    }

    auto vs = VertexStructure::parse_format(format);

    // bind the vao
    LOG(INFO) << "glBindVertexArray(" << id << ")";
    glBindVertexArray(id);

    // bind the buffer
    LOG(INFO) << "glBindBuffer(GL_ARRAY_BUFFER, " << buffer->id << ")";
    glBindBuffer(GL_ARRAY_BUFFER, buffer->id);

    // set the attribute
    // note : as we have sizes in bytes, we use GL_BYTE as the type even for floats
    // note : the pointer parameter is the offset in bytes from the beginning of the buffer, and not a real pointer

    void *pOffset = reinterpret_cast<void *>((uint64_t)vs.offset); // opengl is an old api made with 32 bits in mind, so we have to cast to uint64_t to avoid warnings

    LOG(INFO) << "glVertexAttribPointer(" << location << ", " << vs.components_count << ", " << vs.gl_type << ", "
              << "GL_FALSE"
              << ", " << vs.stride << ", " << pOffset << ")";
    glVertexAttribPointer(location, vs.components_count, vs.gl_type, GL_FALSE, vs.stride, pOffset);

    // a value of 0 means that the attribute is not instanced
    instanced = divisor != 0;
    glVertexAttribDivisor(location, divisor);

    // enable the attribute
    glEnableVertexAttribArray(location);

    if (count == -1)
    {
        count = buffer->get_size() / vs.stride; // just a guess, we will override it later if we have an ebo
        LOG(INFO << "vertices = " << count);
    }
    else
    {
        assert(count == (buffer->get_size() / vs.stride) * (divisor == 0 ? 1 : divisor) && "all buffers must have the same count");
    }

    buffers[location] = buffer;
}

void VertexArray::unbind_buffer(int location)
{
    glDisableVertexArrayAttrib(id, location);
}

void VertexArray::set_ebo(const std::vector<uint32_t> &indices)
{
    // bind the vao
    glBindVertexArray(id);
    
    // create the ebo if it does not exist
    if(ebo_id == 0) {
        glGenBuffers(1, &ebo_id);
    }

    // bind the ebo
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);
    
    // set the count
    count = indices.size();
    LOG(INFO) << "vertices = " << count;
}

size_t VertexArray::get_count() const
{
    return count;
}

void VertexArray::render()
{
    if (shader_program)
    {
        shader_program->use();
    }
    else
    {
        throw std::runtime_error("no shader program set !");
    }

    glBindVertexArray(id);

    if (!instanced)
    {

        if (ebo_id != 0)
        {
            glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
        }
        else
        {
            glDrawArrays(GL_TRIANGLES, 0, count);
        }
    }
    else
    {
        if (ebo_id != 0)
        {
            glDrawElementsInstanced(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr, count);
        }
        else
        {
            glDrawArraysInstanced(GL_TRIANGLES, 0, count, count);
        }
    }
}