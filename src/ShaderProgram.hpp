#pragma once

#include <string>

enum class ShaderType
{
    Vertex,
    Fragment
};

class ShaderProgram
{

    uint32_t id;

public:
    struct ShaderDefinition
    {
        ShaderType type;
        std::string code;
    };

    ShaderProgram(std::initializer_list<ShaderDefinition> shaderDefinitions);
    ~ShaderProgram();

    /**
     * @brief in the case of this shader program does not need any vertex buffer, it can be run directly (in triangle mode)
     * @param vertices
     */
    void run(int vertices);
};
