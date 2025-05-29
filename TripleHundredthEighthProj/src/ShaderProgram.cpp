#include <algorithm>
#include <exception>
#include <fstream>
#include <iostream>
#include <iterator>
#include <vector>

#include <GL/glew.h>

#include "ShaderProgram.hpp"

void ShaderProgram::Shader::compileShader() const
{
    GLchar const* sourcePtr[1] = { shaderSource.data() };
    glShaderSource(shader, 1, sourcePtr, NULL);

    GLint success = 0;
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        GLint maxLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

        std::vector<GLchar> infoLog(maxLength);
        glGetShaderInfoLog(shader, maxLength, &maxLength, infoLog.data());

        std::string path ("\n\nIn file: ");
        path += shaderPath + "\n\n";

        std::copy(path.begin(), path.end(),
            std::back_inserter(infoLog));

        std::copy(shaderSource.begin(), shaderSource.end(),
            std::back_inserter(infoLog));

        throw std::runtime_error(infoLog.data());
    }
}

ShaderProgram::Shader::Shader(std::string path, GLuint type)
{
    using istream_iterator = std::istream_iterator<GLchar>;

    shader = glCreateShader(type);

    if (auto ist = std::ifstream(path))
    {
        ist >> std::noskipws;
        std::copy(istream_iterator(ist), istream_iterator(),
            std::back_inserter(shaderSource));
    }
    else
    {
        throw std::runtime_error(path + " Failed to open");
    }
    shaderSource.push_back('\0');
    compileShader();
}
ShaderProgram::Shader::~Shader()
{
    glDeleteShader(shader);
}

GLuint ShaderProgram::Shader::getShader() const
{
    return shader;
}

//------------------------------------------------------------------------------

void ShaderProgram::linkValidateProgram()
{
    GLint success = 0;
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if (!success)
    {
        GLint maxLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

        std::vector<GLchar> infoLog(maxLength);
        glGetProgramInfoLog(program, maxLength, &maxLength, infoLog.data());

        glDeleteProgram(program);

        throw std::runtime_error(infoLog.data());
    }
    glValidateProgram(program);
    glGetProgramiv(program, GL_VALIDATE_STATUS, &success);

    if (!success)
    {
        GLint maxLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

        std::vector<GLchar> infoLog(maxLength);
        glGetProgramInfoLog(program, maxLength, &maxLength, infoLog.data());

        glDeleteProgram(program);

        throw std::runtime_error(infoLog.data());
    }
}
ShaderProgram::~ShaderProgram()
{
    if (program) glDeleteProgram(program);
}

void ShaderProgram::createProgram()
{
    program = glCreateProgram();
}
void ShaderProgram::shaderFromFile(std::string path, GLuint type)
{
    shaders.emplace_back(path, type);
    glAttachShader(program, shaders.back().getShader());
}

void ShaderProgram::programBegin() const
{
    glUseProgram(program);
}
void ShaderProgram::programEnd() const
{
    glUseProgram(0);
}
