#include "e2d_program.h"

//using name space
using namespace Easy2D;

Program::Program()
{
}

Program::~Program()
{
    glDeleteProgram(mProgramID);
}

bool Program::load(const Path& vFile, const Path& fFile)
{
    mProgramID = glCreateProgram();
    if (mProgramID == 0)
    {
        return false;
    }
    Shader vShader(vFile, GL_VERTEX_SHADER);
    if (!vShader.load())
    {
        LOG_ERROR << ("Program load shader failed: ") << vFile;
        return false;
    }
    glAttachShader(mProgramID, vShader.getShader());
    Shader fShader(fFile, GL_FRAGMENT_SHADER);
    if (!fShader.load())
    {
        LOG_ERROR << ("Program load shader failed: ") << fFile;
        return false;
    }
    glAttachShader(mProgramID, fShader.getShader());

    glLinkProgram(mProgramID);
    GLint status;
    glGetProgramiv(mProgramID, GL_LINK_STATUS, &status);
    if (status == 0)
    {
        GLchar infoLog[1024];
        glGetProgramInfoLog(mProgramID, 1024, NULL, infoLog);
        LOG_ERROR << ("Program::load: Failed to link program: ") << infoLog;
        return false;
    }
    return true;
}

void Program::bind()
{
    glUseProgram(mProgramID);
}

void Program::unbind()
{
    glUseProgram(0);
}

const GLuint Program::getProgramID() const
{
    return mProgramID;
}

GLuint Program::getUniformLocation(const GLchar* nameInShader) const
{
    return glGetUniformLocation(mProgramID, nameInShader);
}

GLuint Program::getAttribLocation(const GLchar* nameInShader) const
{
    return glGetAttribLocation(mProgramID, nameInShader);
}
