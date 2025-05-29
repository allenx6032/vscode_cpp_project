#pragma once

#include <array>
#include <vector>

#include "Point.hpp"

struct GLFWwindow;

// ApEk,
// NoAvailableAlias
// this code is public domain

using Color = std::array<float, 3>;

class DemoState
{
    template <typename T> struct VboWrapper
    {
        std::uint32_t vboID = 0;
        std::vector<T> buffer;

        std::size_t size() const
        {
            return buffer.size() * sizeof(T);
        }
    };

    void updateBackground();
    void updateHatchArea();
    void updateSearched();

    public:

    Point pressPos = {{}};
    Point cursorPos = {{}};
    Point releasePos = {{}};
    Point windowSize = {{ 512, 512 }};

    bool keyPressed = false;
    bool keyChanged = false;

    bool bigminFlag = false;
    bool useShaders = false;

    GLFWwindow* window = nullptr;

    VboWrapper<Color> backgroundColors;
    VboWrapper<Point> backgroundPoints;

    VboWrapper<Color> hatchAreaColors;
    VboWrapper<Point> hatchAreaPoints;

    VboWrapper<Color> searchedColors;
    VboWrapper<Point> searchedPoints;

    void resetState();
    void updateTick();
};
