#ifndef CONTENT_H
#define CONTENT_H

#include <unordered_map>
#include <fstream>
#include <functional>
#include <SFML_Graphics.hpp>

#include "war_tile_content.h"
#include "war_object_content.h"
#include "war_unit_content.h"
#include "lua_includes.h"

extern std::string current_mod;

extern sf::Image atlas;
extern unsigned int tiles;
extern std::unordered_map <std::string, unsigned int> texlib;
extern std::unordered_map <std::string, TileContent> tilelib;
extern std::unordered_map <std::string, ObjectContent> objlib;
extern std::unordered_map <std::string, UnitContent> unitlib;

class Content
{
    private:
        static int register_tile(lua_State * L);
        static int register_object(lua_State * L);

    public:

        Content();
        virtual ~Content();

        void initContent(std::string save);
};

#endif // CONTENT_H

