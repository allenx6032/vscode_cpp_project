#include "gl_render.h"
#include "sharder.h"
#include "vertex_array.h"
#include "index_buffer.h" 

#include <string>

/**
 * @brief  设置顶点属性的格式，告诉 OpenGL 如何解析顶点缓冲区中的数据。
 * 
 * @param index  指定要设置的顶点属性的索引。顶点属性的索引是通过 `glGetAttribLocation` 或预定义常量来获取的。
 *               例如：位置属性的索引为 0，法线属性的索引通常为 1。
 * 
 * @param size   指定每个顶点属性的组件数目。常见的值包括：
 *               - 1：例如一个 `float` 类型的颜色分量或标量。
 *               - 2：例如一个 `vec2`，包含两个浮点数。
 *               - 3：例如一个 `vec3`，包含三个浮点数。
 *               - 4：例如一个 `vec4`，包含四个浮点数。
 * 
 * @param type   指定每个组件的数据类型。常见的值包括：
 *               - `GL_FLOAT`：浮点型数据。
 *               - `GL_INT`：整数型数据。
 *               - `GL_UNSIGNED_INT`：无符号整数型数据。
 *               - `GL_BYTE`：字节型数据。
 *               - `GL_UNSIGNED_BYTE`：无符号字节型数据。
 *               - `GL_SHORT`：短整型数据。
 *               - `GL_UNSIGNED_SHORT`：无符号短整型数据。
 * 
 * @param normalized  是否将数据归一化。若为 `GL_TRUE`，则 OpenGL 会将整数类型的数据映射到 [-1, 1]（对于有符号类型）或 [0, 1]（对于无符号类型）的范围。
 *                    若为 `GL_FALSE`，则 OpenGL 会直接使用原始数据，不进行归一化。
 * 
 * @param stride   指定相邻顶点属性之间的间隔（以字节为单位）。例如，假设每个顶点有位置、法线和纹理坐标三个属性，
 *                 如果 `position` 占用 3 * sizeof(float) 字节，`normal` 占用 3 * sizeof(float) 字节，`texCoord` 占用 2 * sizeof(float) 字节，
 *                 那么 `stride` 的值就是每个顶点所占的字节数。通常它是相邻属性的大小之和。
 * 
 * @param pointer  指定属性数据在缓冲区中的偏移量（以字节为单位）。例如，如果顶点位置在缓冲区的开头，`pointer` 就是 0，
 *                 如果法线数据在缓冲区中紧随其后，`pointer` 就是顶点位置数据的大小，依此类推。
 * 
 * void glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer);
 */

int getVAO() {
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f,  0.5f, 0.0f
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    return VAO;
}

int getEBO() {
    float vertices[] = {
        0.5f, 0.5f, 0.0f,   // 右上角
        0.5f, -0.5f, 0.0f,  // 右下角
        -0.5f, -0.5f, 0.0f, // 左下角
        -0.5f, 0.5f, 0.0f   // 左上角
    };

    unsigned int indices[] = {
        // 注意索引从0开始! 
        // 此例的索引(0,1,2,3)就是顶点数组vertices的下标，
        // 这样可以由下标代表顶点组合成矩形

        0, 1, 3, // 第一个三角形
        1, 2, 3  // 第二个三角形
    };

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    return VAO;
}

int main() {
    std::string vsPath = "..\\resource\\shader\\triangle\\texture.vs";
    std::string fsPath = "..\\resource\\shader\\triangle\\texture.fs";
    
    GLRender render;
    render.init();

    // shader 初始化需要 opengl 环境，所以需要提前初始化 render
    Shader sharder(vsPath.c_str(), fsPath.c_str());
    int VAO = getEBO();

    float vertices[] = {
        0.5f, 0.5f, 0.0f,   // 右上角
        0.5f, -0.5f, 0.0f,  // 右下角
        -0.5f, -0.5f, 0.0f, // 左下角
        -0.5f, 0.5f, 0.0f   // 左上角
    };

    float indices[] = {
        // 注意索引从0开始! 
        // 此例的索引(0,1,2,3)就是顶点数组vertices的下标，
        // 这样可以由下标代表顶点组合成矩形

        0, 1, 3, // 第一个三角形
        1, 2, 3  // 第二个三角形
    };


    // VertexArray  vao{}; // 创建并绑定 vao
    // VertexBuffer vbo{vertices, 12}; // 创建并绑定 vbo
    // IndexBuffer  ebo{indices, 6}; // 创建并绑定 ebo
    
    // VertexData data;
    // data.pushAttribFloat(3);  // 添加位置属性
    // vao.addAttribs(vbo, data);

    render.setRenderCallback([=](){
        sharder.use();
        // glBindVertexArray(vao);
        // vao.bind();
        glBindVertexArray(VAO);
        // 从 VBO 中获取，因为只有三个，所以是三角形
        // glDrawArrays(GL_TRIANGLES, 0, 6); 
        // 从 EBO 中获取，两个三角形，六个点，所以是正方形
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); 
    });

    render.loop();

    return 0;
}