#pragma once

#include <string>
#include <Eigen/Dense>

#include <memory>
#include "ShaderProgram.hpp"
#include "VertexBuffer.hpp"

class VertexArray
{
    private:
        uint32_t id;
        std::shared_ptr<ShaderProgram> shader_program;
        std::map<int, std::shared_ptr<VertexBuffer>> buffers;
        size_t count = -1; // the number of vertices, coming from the first buffer or the ebo if present
        bool use_ebo = false;
        bool instanced = false;
    public:

    VertexArray();
    VertexArray(std::shared_ptr<ShaderProgram> shader_program);
    ~VertexArray();

    void set_shader_program(std::shared_ptr<ShaderProgram> shader_program);

    /**
     * Note : this class maintains a map to the buffers it uses, so it will not be destroyed until the VertexArray is destroyed if necessary.
     * 
     * @brief set the buffer at the given location
     *  
     * @param location the location of the attribute in the shader
     * @param format the format of the attribute, as described in VertexStructure::parse_format
     * @param buffer the buffer to bind
     * @param divisor the divisor to use for this attribute (see glVertexAttribDivisor)
     * 
     *  the divisor is used to specify that the current element if this buffer advances slower than the normal rate.
     *  For example, if the buffer @0 has no divisor and the buffer@1 has a divisor of 2, then the first element of buffer@1 will be used for the first 2 elements of buffer@0, then the second element of buffer@1 will be used for the next 2 elements of buffer@0, and so on.
     * 
     *  in order to preserve my sanity, I forbid the use of a divisor for the first buffer.
     */
    void set_buffer(int location, const std::string &format, std::shared_ptr<VertexBuffer> buffer, int divisor=0);

    void set_ebo(std::shared_ptr<VertexBuffer> buffer);

    std::shared_ptr<ShaderProgram> get_shader_program();

    size_t get_count() const;

    void render(); // only triangles for now
};
