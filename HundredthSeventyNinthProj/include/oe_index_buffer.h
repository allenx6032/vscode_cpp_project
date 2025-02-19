#ifndef OPENGL_ENGINE_INDEX_BUFFER_H
#define OPENGL_ENGINE_INDEX_BUFFER_H

#include <glad/glad.h>

/**
 * 对于 EBO 的封装
*/
class IndexBuffer {
public:
    IndexBuffer() = default;
    IndexBuffer(const float* indices, unsigned int count);
    virtual ~IndexBuffer();

    void bind() const;
    void unBind() const;

private:
    unsigned int id;
};

IndexBuffer::IndexBuffer(const float* indices, unsigned int count) {
    glGenBuffers(1, &id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(float), indices, GL_STATIC_DRAW);
}

void IndexBuffer::bind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
}

void IndexBuffer::unBind() const {
    glDeleteBuffers(1, &id);
}

IndexBuffer::~IndexBuffer() {
    // 如果你调用了 glDeleteBuffers 删除同一个 ID 多次，OpenGL 通常会忽略后续的删除请求，因为已删除的缓冲区 ID 不再有效。
    glDeleteBuffers(1, &id);
}


#endif // OPENGL_ENGINE_INDEX_BUFFER_H