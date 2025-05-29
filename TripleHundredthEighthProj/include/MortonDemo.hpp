#pragma once

#include <memory>
#include <sstream>

#include "ShaderProgram.hpp"
#include "DemoState.hpp"

struct GLFWwindow;

// ApEk,
// NoAvailableAlias
// this code is public domain

class MortonDemo
{
    class SharedExit
    {
        std::shared_ptr<SharedExit> finally;
 
        public:
 
        SharedExit() = default;
 
        template <typename T> SharedExit(T&& lambda):
            finally(this, std::forward<T>(lambda)) {}
    };

    std::stringstream log;
    SharedExit finally;
    DemoState state;

    ShaderProgram hatchAreaShader;
    ShaderProgram searchedShader;

    void drawHatchArea(ShaderProgram const&) const;
    void drawSearched(ShaderProgram const&) const;

    void drawBackground() const;
    void drawHatchArea() const;
    void drawSearched() const;
    void draw2dRange() const;
    void demoDoDraw() const;

    void displayFps() const;

    void windowSizeIndirect(GLFWwindow*, int, int);
    void cursorButtonIndirect(GLFWwindow*, int, int, int);
	void cursorScrollIndirect(GLFWwindow*, double, double);
    void cursorPositionIndirect(GLFWwindow*, double, double);

    static void windowSizeCallback(GLFWwindow*, int, int);
    static void cursorButtonCallback(GLFWwindow*, int, int, int);
	static void cursorScrollCallback(GLFWwindow*, double, double);
    static void cursorPositionCallback(GLFWwindow*, double, double);

    void writeLogFile();
    void createShaders();
    void initializeDemo();

    public:

    void mainLoop();
};
