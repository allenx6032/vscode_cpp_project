#include <GLFW/glfw3.h>
#include "leif.h"

//lf_init_glfw(displayWidth, displayHeight, glfwWindow);

int main() {
  glfwInit();
  GLFWwindow* window = glfwCreateWindow(800, 600, "Hello", NULL, NULL);

  glfwMakeContextCurrent(window);

  lf_init_glfw(800, 600, window);

  while(!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    lf_begin();

    lf_text("Hello, Leif!");

    lf_end();
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  lf_terminate();
  glfwDestroyWindow(window);
  glfwTerminate();
}