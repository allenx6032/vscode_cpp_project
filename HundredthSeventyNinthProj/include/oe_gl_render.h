#ifndef OPENGL_ENGINE_GLRENDER_H
#define OPENGL_ENGINE_GLRENDER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <functional>

class GLRender {
public:
    using RenderCallback = std::function<void()>;
    GLRender() = default;
    void init();
    void release();
    void loop();
    void setRenderCallback(const RenderCallback& callback) { 
        renderCallback_ = callback; 
    }

private:
    
    void processInput(GLFWwindow* window);
    void frameBufferNotify();

    const unsigned int SCR_WIDTH = 800;
    const unsigned int SCR_HEIGHT = 600;

    RenderCallback renderCallback_;
    GLFWwindow* window_;
    bool isLoop_ = false;
};


/**
 * 初始化 GLFW 库
 * 设置 OpenGL 版本号
 * 设置 OpenGL 使用核心模式
 * 创建窗口，设置回调函数
 * 初始化 GLAD 库，用于找到对应函数
*/
void GLRender::init() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    window_ = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "GLRender-windows", NULL, NULL);
    if (window_ == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
    }
    glfwMakeContextCurrent(window_);
    glfwSetFramebufferSizeCallback(window_, [](GLFWwindow* window, int width, int height) {
         glViewport(0, 0, width, height); 
    });

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
    }

}

void GLRender::release() {
    isLoop_ = false;
}

/**
 * glfwWindowShouldClose 检查窗口是否应该关闭
*/
void GLRender::loop() {
    if (isLoop_) {
        return;
    }

    isLoop_ = true;
    while (!glfwWindowShouldClose(window_) && isLoop_)
    {
        // 处理用户输入
        processInput(window_);

        // 使用自定义颜色清空屏幕的颜色缓冲
        // 当调用glClear函数，清除颜色缓冲之后，整个颜色缓冲都会被填充为glClearColor里所设置的颜色。
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        if (renderCallback_) {
            renderCallback_();
        }
        
        // 交换前后缓冲区
        // OpenGL 使用 双缓冲机制，一个缓冲区用于显示当前画面（前缓冲区），另一个缓冲区用于渲染下一帧（后缓冲区）。
        // 调用 glfwSwapBuffers 将后缓冲区与前缓冲区交换，更新显示内容。
        glfwSwapBuffers(window_);
        glfwPollEvents();
    }
}

// 处理用户的输入
void GLRender::processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
}



#endif // OPENGL_ENGINE__RENDER_H