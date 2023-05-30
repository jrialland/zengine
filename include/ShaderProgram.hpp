#pragma once

#include <map>
#include <string>
#include <Eigen/Dense>

enum class ShaderType
{
    Vertex,
    Fragment
};

class ShaderProgram
{

    uint32_t id;

    std::map<std::string, int32_t> uniformLocations;

    int32_t get_uniform_location(const std::string &name);

public:
    struct ShaderDefinition
    {
        ShaderType type;
        std::string code;
    };

    ShaderProgram(const std::vector<ShaderDefinition> &shaderDefinitions);
    ~ShaderProgram();

    void use();

    /**
     * @brief in the case of this shader program does not need any vertex buffer, it can be run directly (in triangle mode)
     * @param vertices
     */
    void run(int vertices);

    void set_uniform(const std::string &name, int32_t value);
    void set_uniform(const std::string &name, uint32_t value);
    void set_uniform(const std::string &name, float value);
    void set_uniform(const std::string &name, const Eigen::Vector2f &value);
    void set_uniform(const std::string &name, const Eigen::Vector3f &value);
    void set_uniform(const std::string &name, const Eigen::Vector4f &value);
    void set_uniform(const std::string &name, const Eigen::Matrix2f &value);
    void set_uniform(const std::string &name, const Eigen::Matrix3f &value);
    void set_uniform(const std::string &name, const Eigen::Matrix4f &value);
};
