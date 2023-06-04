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
}

void VertexArray::set_shader_program(std::shared_ptr<ShaderProgram> shader_program)
{
    this->shader_program = shader_program;
}

std::shared_ptr<ShaderProgram> VertexArray::get_shader_program()
{
    return shader_program;
}

void VertexArray::set_buffer(int location, const std::string &format, std::shared_ptr<VertexBuffer> buffer, int divisor)
{

    if (location == 0)
    {
        assert(divisor == 0 && "divisor is forbidden for location 0 !");
    }

    auto vs = VertexStructure::parse_format(format);

    // bind the vao
    glBindVertexArray(id);

    // bind the buffer
    glBindBuffer(GL_ARRAY_BUFFER, buffer->id);

    // set the attribute
    // note : as we have sizes in bytes, we use GL_BYTE as the type even for floats
    // note : the pointer parameter is the offset in bytes from the beginning of the buffer, and not a real pointer

    


    void* pOffset = reinterpret_cast<void*>((uint64_t)vs.offset); // opengl is an old api made with 32 bits in mind, so we have to cast to uint64_t to avoid warnings

    glVertexAttribPointer(location, vs.components_count, vs.gl_type, GL_FALSE, vs.stride, pOffset);

    // a value of 0 means that the attribute is not instanced
    instanced = divisor != 0;
    glVertexAttribDivisor(location, divisor);

    // enable the attribute
    glEnableVertexAttribArray(location);

    if (count == -1)
    {
        count = buffer->count() * (divisor == 0 ? 1 : divisor); // just a guess, we will override it later if we have an ebo
    } else {
        assert(count == buffer->count() * (divisor == 0 ? 1 : divisor) && "all buffers must have the same count");
    }

    buffers[location] = buffer;
}

void VertexArray::set_ebo(std::shared_ptr<VertexBuffer> buffer)
{
    glBindVertexArray(id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->id); // bind the ebo to the vao
    use_ebo = true;
    count = buffer->count(); // always override what we may have computed when setting the buffers
    buffers[-1] = buffer; // -1 is a special location for the ebo
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

    if(!instanced) {
        if (use_ebo)
        {
            // count is the number of elements in the ebo, see set_ebo
            // the 'indices' parameter is not used because we have bound the ebo to the vao
            glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
        }
        else
        {
            glDrawArrays(GL_TRIANGLES, 0, count);
        }
    } else {
        if (use_ebo)
        {
            glDrawElementsInstanced(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr, count);
        }
        else
        {
            glDrawArraysInstanced(GL_TRIANGLES, 0, count, count);
        }
    }

}