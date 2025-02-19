#ifndef OPENGL_ENGINE_VERTEX_BUFFER_H
#define OPENGL_ENGINE_VERTEX_BUFFER_H

#include <glad/glad.h>

/**
 * 对于 VBO 的封装
*/
class VertexBuffer {
public:
    VertexBuffer() = default;
    VertexBuffer(const float* vertices, unsigned int count);
    virtual ~VertexBuffer();

    void bind() const;
    void unBind() const;

private:
    unsigned int id;
};

VertexBuffer::VertexBuffer(const float* vertices, unsigned int count) {
    glGenBuffers(1, &id);
    glBindBuffer(GL_ARRAY_BUFFER, id);
    glBufferData(GL_ARRAY_BUFFER, count * sizeof(float), vertices, GL_STATIC_DRAW);
}

void VertexBuffer::bind() const {
    glBindBuffer(GL_ARRAY_BUFFER, id);
}

void VertexBuffer::unBind() const {
    glDeleteBuffers(1, &id);
}

VertexBuffer::~VertexBuffer() {
    // 如果你调用了 glDeleteBuffers 删除同一个 ID 多次，OpenGL 通常会忽略后续的删除请求，因为已删除的缓冲区 ID 不再有效。
    glDeleteBuffers(1, &id);
}

#endif // OPENGL_ENGINE_VERTEX_BUFFER_H
