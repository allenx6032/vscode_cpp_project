#include "war_content.h"
#include <iostream>

std::string current_mod = "";

sf::Image atlas;
unsigned int tiles;
std::unordered_map <std::string, unsigned int> texlib;
std::unordered_map <std::string, TileContent> tilelib;
std::unordered_map <std::string, ObjectContent> objlib;
std::unordered_map <std::string, UnitContent> unitlib;

Content::Content()
{

}

Content::~Content()
{
    //dtor
}

int Content::register_tile(lua_State * L)
{
    std::string name = luaL_checkstring(L, 1);
    lua_getfield(L, 2, "texture");
    std::string texture = luaL_checkstring(L, -1);
    std::cout << name << " tile registered with ";
    //textures.push_back(sf::Texture());
    //tilelib[name].texture = new sf::Texture();
    //tilelib[name].texture->loadFromFile("mods/" + current_mod + "/textures/" + texture);
    //textures[textures.size() - 1].loadFromFile("mods/" + current_mod + "/textures/" + texture);
    sf::Image image;
    image.loadFromFile("mods/" + current_mod + "/textures/" + texture);

    atlas.copy(image, 32 * tiles, 0);
    tilelib[name].texture = tiles;
    tiles++;
    std::cout << texture << " texture" << std::endl;
    return 1;
}

int Content::register_object(lua_State * L)
{
    std::cout << "Registering object \"";
    std::string name = luaL_checkstring(L, 1);
    std::cout << name << "\"...\n";
    lua_getfield(L, 2, "texture");
    lua_getfield(L, -1, "up");
    lua_getfield(L, -2, "down");
    lua_getfield(L, -3, "left");
    lua_getfield(L, -4, "right");
    std::string texture_up = luaL_checkstring(L, -4);
    std::string texture_down = luaL_checkstring(L, -3);
    std::string texture_left = luaL_checkstring(L, -2);
    std::string texture_right = luaL_checkstring(L, -1);

    objlib[name].texture_up = new sf::Texture();
    objlib[name].texture_up->loadFromFile("mods/" + current_mod + "/textures/" + texture_up);
    objlib[name].texture_down = new sf::Texture();
    objlib[name].texture_down->loadFromFile("mods/" + current_mod + "/textures/" + texture_down);
    objlib[name].texture_left = new sf::Texture();
    objlib[name].texture_left->loadFromFile("mods/" + current_mod + "/textures/" + texture_left);
    objlib[name].texture_right = new sf::Texture();
    objlib[name].texture_right->loadFromFile("mods/" + current_mod + "/textures/" + texture_right);

    std::cout << "size...\n";
    lua_getfield(L, 2, "size");
    lua_getfield(L, -1, "x");
    lua_getfield(L, -2, "y");
    sf::Vector2f spriteSize(lua_tonumber(L, -2), lua_tonumber(L, -1));
    objlib[name].spriteSize = spriteSize;

    std::cout << "origin...\n";
    lua_getfield(L, 2, "origin");
    lua_getfield(L, -1, "x");
    lua_getfield(L, -2, "y");
    sf::Vector2f spriteOrigin(lua_tonumber(L, -2), lua_tonumber(L, -1));
    objlib[name].spriteOrigin = spriteOrigin;

    std::cout << "textures: ";
    std::cout << texture_up << ", " << texture_down << ", " << texture_left << ", " << texture_right << std::endl;
}

void Content::initContent(std::string save)
{
    lua_State * L = luaL_newstate();

    lua_newtable(L);
    lua_setglobal(L, "game");
    lua_getglobal(L, "game");

    lua_pushstring(L, "register_tile");
    lua_pushcfunction(L, Content::register_tile);
    lua_settable(L, -3);

    lua_pushstring(L, "register_object");
    lua_pushcfunction(L, Content::register_object);
    lua_settable(L, -3);

    atlas.create(1024, 1024);
    tiles = 0;

    std::ifstream ifmods("save/" + save + "/mods.txt");
    if (ifmods.is_open())
    {
        while(ifmods >> current_mod)
        {
            if (current_mod == "")
            {
                std::cout << "Warning: saving with zero mods\n";
                break;
            }
            std::string lua_file = "mods/" + current_mod + "/init.lua";
            int ok = luaL_dofile(L, lua_file.c_str());
            if (ok != LUA_OK)
                std::cout << "Error at loading " << current_mod << " mod!\n";
            else
                std::cout << current_mod << " mod loaded.\n";
            lua_close(L);

        }
    }
    else
    {
    std::cout << "No so file for content lol";
    }
    ifmods.close();

    /*
    std::ifstream ifobj("resources/objects.list");
    if (ifobj.is_open())
    {
        while (!ifobj.eof())
        {
            std::string object_name, object_texture_name;
            ifobj >> object_name >> object_texture_name;
            if (object_name == "") std::cout << "WARNING EMPTY FILE!\n";
            objlib[object_name].texture_up = &texlib[object_texture_name];
            ifobj >> objlib[object_name].spriteSize.x >> objlib[object_name].spriteSize.y;
            ifobj >> objlib[object_name].spriteOrigin.x >> objlib[object_name].spriteOrigin.y;
        }
    }


    ifobj.close();
    */

}

