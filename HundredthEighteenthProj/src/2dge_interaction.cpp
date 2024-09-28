#include "2dge_interaction.hpp"
InteractionManager::InteractionManager()
{
    __DEBUG_EXEC(std::cout << "InteractionManager()\n");
    initScripts();
}
InteractionManager::~InteractionManager()
{
    __DEBUG_EXEC(std::cout << "~InteractionManager()\n");
}
void InteractionManager::addScript(const std::string& name, std::size_t index1, std::size_t index2)
{
    assert(index2 < scriptVec.size());
    assert(index1 < scriptVec[index2].size());
    scriptVec[index1][index2] = name;
}
void InteractionManager::InteractionManager::initScripts()
{
    lua.open_libraries(sol::lib::base, sol::lib::package);
    lua.new_usertype<InteractionManager>("InteractionManager", sol::constructors<InteractionManager()>(),
        "addScript", &InteractionManager::addScript);
    lua["intManager"] = std::ref(*this);
    lua.new_usertype<GameObject>("GameObject", sol::constructors<GameObject()>(),
        "addAnimation", &GameObject::addAnimation,
        "setAnimation", &GameObject::setAnimation,
        "playAnimation", &GameObject::playAnimation,
        "getAnimation", &GameObject::getAnimation,
        "getGlobalCenter", &GameObject::getGlobalCenter,
        "setInteractRadius", &GameObject::setInteractRadius, 
        "getCurrentAnimationNum", &GameObject::getCurrentAnimationNum,
        "setPosition", sol::overload(
            sol::resolve<void(float, float)>(&GameObject::setPosition),
            sol::resolve<void(const sf::Vector2f&)>(&GameObject::setPosition)),
        "move", &GameObject::move,
        "setLooped", &GameObject::setLooped);
    lua.new_usertype<ParameterObject>("ParameterGameObject", sol::constructors<ParameterObject(int)>(),
        "setHp", &ParameterObject::setHp,
        "getHp", &ParameterObject::getHp,
        "setMaxHp", &ParameterObject::setMaxHp,
        "getMaxHp", &ParameterObject::getMaxHp,
        "setDamage", &ParameterObject::setDamage,
        "getDamage", &ParameterObject::getDamage,
        "setState", &ParameterObject::setState,
        "getState", &ParameterObject::getState,
        "setTarget", &ParameterObject::setTarget);
    lua.new_usertype<DynamicGameObject>("DynamicGameObject", sol::constructors<DynamicGameObject()>(),
        sol::base_classes, sol::bases<GameObject, ParameterObject>(),
        "setVelocity", sol::overload(
            sol::resolve<void(float, float)>(&DynamicGameObject::setVelocity),
            sol::resolve<void(const sf::Vector2f&)>(&DynamicGameObject::setVelocity)),
        "getVelocity", &DynamicGameObject::getVelocity,
        "deactivate", &DynamicGameObject::deactivate,
        "setDeactivateTime", &DynamicGameObject::setDeactivateTime,
        "clockRestart", &DynamicGameObject::clockRestart,
        "setGhostMode", &DynamicGameObject::setGhostMode);
    lua.new_usertype<sf::Vector2f>("Vector2f", sol::constructors<sf::Vector2f(), sf::Vector2f(float, float)>());
    lua["Vector2f"][sol::meta_function::multiplication] = [](sf::Vector2f& vec, float num) -> sf::Vector2f {
        return sf::Vector2f(vec.x * num, vec.y * num);
    };
    lua.new_usertype<StaticGameObject>("StaticGameObject", sol::constructors<StaticGameObject()>(),
        sol::base_classes, sol::bases<GameObject>());
    lua["seconds"] = &sf::seconds;
    lua["milliseconds"] = &sf::milliseconds;
    lua["microseconds"] = &sf::microseconds;
    sol::protected_function_result script_result = lua.script_file("scripts/objects.lua");
    //sol::load_result script = lua.load_file("scripts/objects.lua");
    assert(script_result.valid());
    std::size_t type_max = lua["setting"]["type_max"];
    scriptVec = std::vector<std::vector<std::string>>(type_max, std::vector<std::string>(type_max, "nope"));
    script_result = lua.script_file("scripts/interaction_init.lua");
    assert(script_result.valid());
    sol::load_result script = lua.load_file("scripts/interactions.lua");
    assert(script.valid());
    sol::protected_function target = script;
    bcode = target.dump();
}