#include "ShaderProgram.hpp"

#include <GL/glew.h>
#include <GL/gl.h>

#include <cassert>
#include <vector>
#include <stdexcept>

static uint32_t get_gl_shader_type(ShaderType shaderType)
{
    switch (shaderType)
    {
    case ShaderType::Vertex:
        return GL_VERTEX_SHADER;
    case ShaderType::Fragment:
        return GL_FRAGMENT_SHADER;
    }
    throw std::runtime_error("unknown shader type");
}

static std::string get_shader_type_str(ShaderType shaderType)
{
    switch (shaderType)
    {
    case ShaderType::Vertex:
        return "Vertex";
    case ShaderType::Fragment:
        return "Fragment";
    }
    throw std::runtime_error("unknown shader type");
}

ShaderProgram::ShaderProgram(const std::vector<ShaderDefinition> &shaderDefinitions)
{

    std::vector<uint32_t> shaderIds;

    for (auto &shaderDef : shaderDefinitions)
    {

        auto shaderId = glCreateShader(get_gl_shader_type(shaderDef.type));
        const GLchar *source = (const GLchar *)shaderDef.code.c_str();
        glShaderSource(shaderId, 1, &source, nullptr);
        glCompileShader(shaderId);

        int32_t compiled = 0;
        glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compiled);
        if (!compiled)
        {
            int32_t len;
            glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &len);
            std::string message;
            message.reserve(len);
            glGetShaderInfoLog(shaderId, len, &len, (GLchar *)message.data());
            glDeleteShader(shaderId);
            throw std::runtime_error(get_shader_type_str(shaderDef.type) + " shader compilation failed : " + message);
        }
        else
        {
            shaderIds.push_back(shaderId);
        }
    }

    id = glCreateProgram();

    for (auto shaderId : shaderIds)
    {
        glAttachShader(id, shaderId);
    }

    int32_t linked = 0;
    glGetProgramiv(id, GL_LINK_STATUS, &linked);
    if (!linked)
    {

        int32_t len;
        glGetProgramiv(id, GL_INFO_LOG_LENGTH, &len);
        std::string message;
        message.reserve(len);
        glGetProgramInfoLog(id, len, &len, (GLchar *)message.data());
        glDeleteProgram(id);
        for (auto shaderId : shaderIds)
        {
            glDeleteShader(shaderId);
        }
        throw std::runtime_error("Could not link shader program : " + message);
    }

    for (auto shaderId : shaderIds)
    {
        glDetachShader(id, shaderId);
        glDeleteShader(shaderId);
    }
}

int32_t ShaderProgram::get_uniform_location(const std::string &name)
{
    auto it = uniformLocations.find(name);
    if (it != uniformLocations.end())
    {
        return it->second;
    }
    else
    {
        auto location = glGetUniformLocation(id, name.c_str());
        if (location < 0)
        {
            throw std::runtime_error("uniform " + name + " not found");
        }
        uniformLocations[name] = location;
        return location;
    }
}

ShaderProgram::~ShaderProgram()
{
    glDeleteProgram(id);
}

void ShaderProgram::use()
{
    glUseProgram(id);
}

void ShaderProgram::run(int vertices)
{
    assert(vertices % 3 == 0);
    glUseProgram(id);
    glDrawElements(GL_TRIANGLES, vertices, GL_UNSIGNED_INT, nullptr);
}

void ShaderProgram::set_uniform(const std::string &name, int32_t value)
{
    glUniform1i(get_uniform_location(name), value);
}

void ShaderProgram::set_uniform(const std::string &name, uint32_t value)
{
    glUniform1ui(get_uniform_location(name), value);
}
void ShaderProgram::set_uniform(const std::string &name, float value)
{
    glUniform1f(get_uniform_location(name), value);
}
void ShaderProgram::set_uniform(const std::string &name, const Eigen::Vector2f &value)
{
    glUniform2fv(get_uniform_location(name), 1, value.data());
}
void ShaderProgram::set_uniform(const std::string &name, const Eigen::Vector3f &value)
{
    glUniform3fv(get_uniform_location(name), 1, value.data());
}
void ShaderProgram::set_uniform(const std::string &name, const Eigen::Vector4f &value)
{
    glUniform4fv(get_uniform_location(name), 1, value.data());
}
void ShaderProgram::set_uniform(const std::string &name, const Eigen::Matrix2f &value)
{
    glUniformMatrix2fv(get_uniform_location(name), 1, GL_FALSE, value.data());
}
void ShaderProgram::set_uniform(const std::string &name, const Eigen::Matrix3f &value)
{
    glUniformMatrix3fv(get_uniform_location(name), 1, GL_FALSE, value.data());
}
void ShaderProgram::set_uniform(const std::string &name, const Eigen::Matrix4f &value)
{
    glUniformMatrix4fv(get_uniform_location(name), 1, GL_FALSE, value.data());
}