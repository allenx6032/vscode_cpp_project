#include "war_vao.h"
#include <iostream>
#include <cassert>

VAO::VAO()
{
    glGenVertexArrays(1, &vao);
}

VAO::~VAO()
{
    glDeleteBuffers(vbos.size(), vbos.data());
    glDeleteBuffers(1, &ebo);
    glDeleteVertexArrays(1, &vao);
}

void VAO::Clear()
{
    vbos.clear();
    glDeleteBuffers(vbos.size(), vbos.data());
    glDeleteBuffers(1, &ebo);
}

void VAO::Bind()
{
    glBindVertexArray(vao);
}

void VAO::AddVBO(const std::vector<glm::vec3>& data)
{
    Bind();
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(glm::vec3), data.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(vbos.size(), 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    vbos.push_back(vbo);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void VAO::AddEBO(const std::vector<unsigned>& data)
{
    ebo_len = data.size();

    Bind();
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.size() * sizeof(unsigned int), data.data(), GL_STATIC_DRAW);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void VAO::Draw()
{
    assert(ebo != 0);
    Bind();
    for (size_t i = 0; i < vbos.size(); ++i)
    {
        glEnableVertexAttribArray(i);
    }
    glDrawElements(GL_TRIANGLES, ebo_len, GL_UNSIGNED_INT, 0);
    for (size_t i = 0; i < vbos.size(); ++i)
    {
        glDisableVertexAttribArray(i);
    }
    glBindVertexArray(0);
}
