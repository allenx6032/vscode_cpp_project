#ifndef OPENGL_ENGINE_VERTEX_ARRAY_H
#define OPENGL_ENGINE_VERTEX_ARRAY_H

#include "oe_vertex_buffer.h"
#include "oe_vertex_data.h"

/**
 * 对 VAO 的封装
*/
class VertexArray {
public:
    VertexArray();
    virtual ~VertexArray();

    void bind() const;
    void unBind() const;

    void addAttribs(const VertexBuffer& VBO, const VertexData& data);

private:
    unsigned int id;
};

VertexArray::VertexArray() {
    glGenVertexArrays(1, &id);
    glBindVertexArray(id);
}

void VertexArray::bind() const {
    glBindVertexArray(id);
}

void VertexArray::unBind() const {
    glBindVertexArray(0);
}

VertexArray::~VertexArray() {
    glBindVertexArray(0);
}

void VertexArray::addAttribs(const VertexBuffer& VBO, const VertexData& data) {
    VBO.bind();
    const auto& vertices{ data.getVertex() };

    int offset{};
    for (int i = 0; i < vertices.size(); ++i) {
        const auto& attrib{ vertices[i] };

        glVertexAttribPointer(i, attrib.count, attrib.type, GL_FALSE, data.getStride(), (void*)offset);
        glEnableVertexAttribArray(i);

        offset += attrib.count * Attrib::getTypeSize(attrib.type);
    }
}

#endif // OPENGL_ENGINE_VERTEX_ARRAY_H