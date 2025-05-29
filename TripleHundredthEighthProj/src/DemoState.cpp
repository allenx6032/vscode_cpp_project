#include <algorithm>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "DemoState.hpp"
#include "Point.hpp"

// ApEk,
// NoAvailableAlias
// this code is public domain

template <typename Iterator> struct RangeHelper
{
    Iterator lhs;
    Iterator rhs;

    Iterator cbegin() const { return lhs; }
    Iterator begin() const { return lhs; }
    Iterator cend() const { return rhs; }
    Iterator end() const { return rhs; }
};
template <typename Iterator>
static RangeHelper<Iterator> Range(Iterator lhs,
                                   Iterator rhs)
{
    return { lhs, rhs };
}

//------------------------------------------------------------------------------

template <typename PT>
static void bigalg(PT const& Min,
                   PT const& Max,
                   std::vector<PT>& points,
                   std::vector<PT>& missed,
                   std::vector<PT>& retval)
{
    PT min = { std::min(Min[0], Max[0]), std::min(Min[1], Max[1]) };
    PT max = { std::max(Min[0], Max[0]), std::max(Min[1], Max[1]) };

    auto begin = points.cbegin();
    auto end   = points.cend();

    auto start = std::lower_bound(begin, end, min, morton<PT>());
    auto stop  = std::upper_bound(start, end, max, morton<PT>());

    while (start != stop)
    {
        PT const& point = *start;

        if (point[0] >= min[0] && point[1] >= min[1] &&
            point[0] <= max[0] && point[1] <= max[1])
        {
            retval.emplace_back(point);
            ++start;
        }
        else
        {
            missed.emplace_back(point);
            start = std::lower_bound(start, stop,
                bigmin<PT>(min, max, point), morton<PT>());
        }
    }
}

//------------------------------------------------------------------------------

template <typename PT>
static void search(PT const& Min,
                   PT const& Max,
                   std::vector<PT>& points,
                   std::vector<PT>& missed,
                   std::vector<PT>& retval)
{
    PT min = { std::min(Min[0], Max[0]), std::min(Min[1], Max[1]) };
    PT max = { std::max(Min[0], Max[0]), std::max(Min[1], Max[1]) };

    auto begin = points.cbegin();
    auto end   = points.cend();

    auto start = std::lower_bound(begin, end, min, morton<PT>());
    auto stop  = std::upper_bound(start, end, max, morton<PT>());

    for (PT point : Range(start, stop))
    {
        if (point[0] >= min[0] && point[1] >= min[1] &&
            point[0] <= max[0] && point[1] <= max[1])
        {
            retval.emplace_back(point); // hotspot #1 b
        }
        else
        {
            missed.emplace_back(point); // hotspot #1 a
        }
    }
}

//------------------------------------------------------------------------------

void DemoState::updateBackground()
{
    auto xMax = windowSize[0];
    auto yMax = windowSize[1];
    auto sMax = xMax * yMax;

    backgroundColors.buffer.resize(sMax);
    backgroundPoints.buffer.resize(sMax);

    for (std::size_t x = 0; x < xMax; ++x)
    {
        for (std::size_t y = 0; y < yMax; ++y)
        {
            auto i = x * yMax + y;

            backgroundPoints.buffer[i][0] = x;
            backgroundPoints.buffer[i][1] = y;

            backgroundColors.buffer[i][2] =
                (i / float(sMax) * 0.8f + 0.2f);
        }
    }
    std::sort(backgroundPoints.buffer.begin(),
        backgroundPoints.buffer.end(), morton<Point>());

    glBindBuffer(GL_ARRAY_BUFFER, backgroundPoints.vboID);
    glBufferData(GL_ARRAY_BUFFER, backgroundPoints.size(),
        backgroundPoints.buffer.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, backgroundColors.vboID);
    glBufferData(GL_ARRAY_BUFFER, backgroundColors.size(),
        backgroundColors.buffer.data(), GL_STATIC_DRAW);
}
void DemoState::updateHatchArea()
{
    glBindBuffer(GL_ARRAY_BUFFER, hatchAreaPoints.vboID);
    glBufferData(GL_ARRAY_BUFFER, hatchAreaPoints.size(),
        hatchAreaPoints.buffer.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, hatchAreaColors.vboID);
    glBufferData(GL_ARRAY_BUFFER, hatchAreaColors.size(),
        hatchAreaColors.buffer.data(), GL_DYNAMIC_DRAW);
}
void DemoState::updateSearched()
{
    glBindBuffer(GL_ARRAY_BUFFER, searchedPoints.vboID);
    glBufferData(GL_ARRAY_BUFFER, searchedPoints.size(),
        searchedPoints.buffer.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, searchedColors.vboID);
    glBufferData(GL_ARRAY_BUFFER, searchedColors.size(),
        searchedColors.buffer.data(), GL_DYNAMIC_DRAW);
}

//------------------------------------------------------------------------------

void DemoState::resetState()
{
    pressPos = {};
    releasePos = {};
    keyChanged = true;
    updateBackground();

    hatchAreaColors.buffer.clear();
    hatchAreaPoints.buffer.clear();
    searchedColors.buffer.clear();
    searchedPoints.buffer.clear();
}

//------------------------------------------------------------------------------

void DemoState::updateTick()
{
    Point const& min = keyPressed ? cursorPos : pressPos;
    Point const& max = releasePos;

    hatchAreaColors.buffer.clear();
    hatchAreaPoints.buffer.clear();
    searchedColors.buffer.clear();
    searchedPoints.buffer.clear();

    if (bigminFlag == false)
    {
        search<Point>(min, max, backgroundPoints.buffer,
                                hatchAreaPoints.buffer,
                                searchedPoints.buffer);
    }
    else
    {
        bigalg<Point>(min, max, backgroundPoints.buffer,
                                hatchAreaPoints.buffer,
                                searchedPoints.buffer);
    }
    if (useShaders == false)
    {
        std::size_t size = hatchAreaPoints.buffer.size();
        std::size_t i = 0;

        hatchAreaColors.buffer.resize(size);

        for (Color& color : hatchAreaColors.buffer) // hotspot #2
        {
            color[0] = (i++ / float(size) * 0.9f + 0.1f);
        }
    }
    if (useShaders == false)
    {
        std::size_t size = searchedPoints.buffer.size();
        std::size_t i = 0;

        searchedColors.buffer.resize(size);

        for (Color& color : searchedColors.buffer) // hotspot #3
        {
            color[1] = (i++ / float(size) * 0.9f + 0.1f);
        }
    }
    updateHatchArea();
    updateSearched();

    keyChanged = false;
}
