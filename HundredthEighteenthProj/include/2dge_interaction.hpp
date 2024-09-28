#ifndef INTERACTION_HPP_
#define INTERACTION_HPP_
#include <iostream>
#include <vector>
#include <limits>
#include "sol/sol.hpp"
#include "2dge_gameobjects.hpp"
#define NDEBUG
#ifndef NDEBUG
#define __DEBUG_EXEC(code) code
#else
#define __DEBUG_EXEC(code) ;
#endif
class InteractionManager
{
    sol::state lua;
    sol::bytecode bcode;
    std::vector<std::vector<std::string>> scriptVec;
public:
   InteractionManager();
   void addScript(const std::string& name, std::size_t index1, std::size_t index2);
   void initScripts();
   template<typename T1, typename T2, typename... Args>
   void interact(T1& obj1, T2& obj2, Args... args) 
   {
        auto obj1_type = obj1.getType();
        auto obj2_type = obj2.getType();
        assert(obj1_type < scriptVec.size());
        assert(obj1_type < scriptVec[obj2_type].size());
        assert(obj2_type < scriptVec.size());
        assert(obj2_type < scriptVec[obj1_type].size());
        auto script = lua.safe_script(bcode.as_string_view(), sol::script_pass_on_error);
        assert(script.valid());
        lua[scriptVec[obj1_type][obj2_type]](obj1, obj2, args...);
        //lua[scriptVec[obj2_type][obj1_type]](obj2.getRef(), args...);
   }
   ~InteractionManager();
};
#endif //INTERACTION_HPP_