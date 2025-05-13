/**
 * @Author: Amélie Heinrich
 * @Create Time: 2024-03-23 17:33:57
 */

#pragma once

#include <vector>
#include <cmath>

#include <rng.hpp>
#include <generator.hpp>

class TunnelingGenerator : public DungeonGenerator
{
public:
    virtual void GenerateMap(Map& map) override;

    virtual const std::string& GetName() const override {
        return "Tunneling";
    }
    virtual GeneratorType GetType() const override {
        return GeneratorType::Tunneling;
    }

private:
    void CreateRoom(Map& map, Room& room);
    void CreateHorTunnel(Map& map, int x1, int x2, int y);
    void CreateVirTunnel(Map& map, int y1, int y2, int x);

    int _roomMinSize = 6;
    int _roomMaxSize = 15;
    int _maxRooms = 30;
};
