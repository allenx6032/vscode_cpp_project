#pragma once

#include <string>
#include <vector>

#include <GL/glew.h>

class ShaderProgram
{
    class Shader
    {
        GLuint shader;
        std::string shaderPath;
        std::vector<GLchar> shaderSource;

        void compileShader() const;

        public:

        Shader(std::string, GLuint);
       ~Shader();

        GLuint getShader() const;
    };
    
    GLuint program = 0;
    std::vector<Shader> shaders;

    void linkValidateProgram();

    public:

    ShaderProgram() = default;
   ~ShaderProgram();

    void createProgram();
    void shaderFromFile(std::string, GLuint);

    void programBegin() const;
    void programEnd() const;
};
