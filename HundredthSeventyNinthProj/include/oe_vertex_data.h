/**
 * 这里的基本拷贝参考此博主的实现 
 * https://www.bilibili.com/opus/826331293780279318/?from=readlist
*/
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <iostream>

/**
 * @brief Attrib 用于记录顶点属性，包括此属性的数据类型，此属性需要多个元素表示
 * @param type 类型
 * @param count 数量
*/
struct Attrib
{
    unsigned int type;
    unsigned int count;

    static unsigned int getTypeSize(unsigned int type) {
        switch (type) {
            case GL_FLOAT: return 4;
            case GL_UNSIGNED_INT: return 4;   
            case GL_UNSIGNED_BYTE: return 1;
        }
        return 0;
    }
};

class VertexData {
private:
    std::vector<Attrib> vertex; // 由一组顶点属性构成的顶点
    unsigned int stride;        // 每个顶点的步长

public:
    VertexData() : stride(0) {}

    inline const unsigned int getStride() const { return stride; }
    inline const std::vector<Attrib> getVertex() const { return vertex; }

    void pushAttrib(unsigned int count) {
        std::cout << "Invalid type." << std::endl;
    }

    void pushAttribFloat(unsigned int count);

    void pushAttribInt(unsigned int count);

    void pushAttribByte(unsigned int count);
};

void VertexData::pushAttribFloat(unsigned int count) {
    Attrib a{ GL_FLOAT, count};
    vertex.push_back(a);
    stride += count * Attrib::getTypeSize(GL_FLOAT);
}

void VertexData::pushAttribInt(unsigned int count) {
    Attrib a{ GL_UNSIGNED_INT, count};
    vertex.push_back(a);
    stride += count * Attrib::getTypeSize(GL_UNSIGNED_INT);
}

void VertexData::pushAttribByte(unsigned int count) {
    Attrib a{ GL_UNSIGNED_BYTE, count};
    vertex.push_back(a);
    stride += count * Attrib::getTypeSize(GL_UNSIGNED_BYTE);
}   

/**
 * 好像并不需要模板
*/
// template <typename T>
// class VertexData {
// private:
//     std::vector<Attrib> vertex; // 由一组顶点属性构成的顶点
//     unsigned int stride;        // 每个顶点的步长

// public:
//     VertexData() : stride(0) {}

//     inline const unsigned int getStride() const { return stride; }
//     inline const std::vector<Attrib> getVertex() const { return vertex; }

//     void pushAttrib(unsigned int count) {
//         std::cout << "Invalid type." << std::endl;
//     }

//     template<>
//     void VertexData<float>::pushAttrib(unsigned int count) {
//         Attrib a{ GL_FLOAT, count};
//         vertex.push_back(a);
//         stride += count * Attrib::getTypeSize(GL_FLOAT);
//     }

//     template<>
//     void VertexData<unsigned int>::pushAttrib(unsigned int count) {
//         Attrib a{ GL_UNSIGNED_INT, count};
//         vertex.push_back(a);
//         stride += count * Attrib::getTypeSize(GL_UNSIGNED_INT);
//     }

//     template<>
//     void VertexData<unsigned char>::pushAttrib(unsigned int count) {
//         Attrib a{ GL_UNSIGNED_BYTE, count};
//         vertex.push_back(a);
//         stride += count * Attrib::getTypeSize(GL_UNSIGNED_BYTE);
//     }   
// };


