﻿#include "maze_PathFinder.h"
#include "maze_Node.h"
#include "maze_Maze.h"

#include <vector>
#include <algorithm>
#include <cmath>

static constexpr int directionsX[4] = {0,  1,  0, -1};
static constexpr int directionsY[4] = {-1, 0,  1,  0};

std::shared_ptr<Node> PathFinder::FindPath(const std::shared_ptr<Maze>& maze,
    const int sx, const int sy,
    const int tx, const int ty)
{
    std::vector<std::shared_ptr<Node>> open;
    std::vector<std::shared_ptr<Node>> closed;

    auto startNode = std::make_shared<Node>(Node{
        sx,
        sy,
        0,
        Heuristic(sx, sy, tx, ty),
        0,
        nullptr
    });
    startNode->f = startNode->g + startNode->h;
    open.push_back(startNode);

    while (!open.empty())
    {
        auto currentIt = std::ranges::min_element(
            open,
            [](const std::shared_ptr<Node>& lhs, const std::shared_ptr<Node>& rhs)
            {
                return lhs->f < rhs->f;
            }
        );
        std::shared_ptr<Node> currentNode = *currentIt;

        std::erase(open, currentNode);

        if (currentNode->x == tx && currentNode->y == ty)
        {
            return currentNode;
        }

        closed.push_back(currentNode);

        for (int i = 0; i < 4; ++i)
        {
            int ny = currentNode->y + directionsY[i];
            int nx = currentNode->x + directionsX[i];

            if (!maze->IsValid(ny, nx))
                continue;

            if (maze->GetAt(nx, ny))
                continue;

            if (std::ranges::any_of(
                closed,
                [ny, nx](const std::shared_ptr<Node>& node)
                {
                    return (node->x == nx && node->y == ny);
                }
            ))
            {
                continue;
            }

            const int newG = currentNode->g + 1;
            const int newH = Heuristic(nx, ny, tx, ty);
            const int newF = newG + newH;

            auto itOpen = std::ranges::find_if(
                open,
                [ny, nx](const std::shared_ptr<Node>& node)
                {
                    return (node->x == nx && node->y == ny);
                }
            );

            if (itOpen == open.end() || newG < (*itOpen)->g)
            {
                if (itOpen != open.end()) {
                    open.erase(itOpen);
                }

                auto neighbor = std::make_shared<Node>(Node{
                    nx,
                    ny,
                    newG,
                    newH,
                    newF,
                    currentNode
                });
                open.push_back(neighbor);
            }
        }
    }

    return {};
}

std::shared_ptr<Node> PathFinder::SimplifyPath(const std::shared_ptr<Node>& node)
{
    std::vector<std::shared_ptr<Node>> fullPath;
    for (auto current = node; current; current = current->parent)
    {
        fullPath.push_back(current);
    }

    std::ranges::reverse(fullPath);

    std::vector<std::shared_ptr<Node>> simplified;
    simplified.reserve(fullPath.size());

    simplified.push_back(fullPath[0]);

    for (size_t i = 1; i + 1 < fullPath.size(); i++)
    {
        const int xPrev = fullPath[i - 1]->x;
        const int yPrev = fullPath[i - 1]->y;
        const int xCurr = fullPath[i    ]->x;
        const int yCurr = fullPath[i    ]->y;
        const int xNext = fullPath[i + 1]->x;
        const int yNext = fullPath[i + 1]->y;

        const int dx1 = xCurr - xPrev;
        const int dy1 = yCurr - yPrev;
        const int dx2 = xNext - xCurr;
        const int dy2 = yNext - yCurr;

        if (dx1 != dx2 || dy1 != dy2)
        {
            simplified.push_back(fullPath[i]);
        }
    }

    simplified.push_back(fullPath.back());

    for (size_t i = 0; i + 1 < simplified.size(); i++)
    {
        simplified[i]->parent = simplified[i + 1];
    }
    
    simplified.back()->parent = nullptr;

    return simplified[0];
}

int PathFinder::Heuristic(const int sx, const int sy, const int tx, const int ty)
{
    return std::abs(sx - tx) + std::abs(sy - ty);
}
