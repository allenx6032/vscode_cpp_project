#include "2dge_ai.hpp"
AImanager::AImanager(ObjectManager& oManager, Map& map):
    oManager(oManager),
    map(map)
{
    __DEBUG_EXEC(std::cout << "AImanager(oManager, map)\n");
    init();
}
namespace sf
{
    bool operator<(const sf::Vector2i& a, const sf::Vector2i& b)
    {
        if (a.x == b.x)
            return a.y > b.y;
        return a.x < b.x;
    }
}
void AImanager::init()
{
    lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::math);
    lua.new_usertype<ObjectManager>("ObjectManager", sol::constructors<ObjectManager()>(),
        "addScript", &ObjectManager::addScript,
        "createDynamicObject", &ObjectManager::createDynamicObject<>,
        "newDynamicObjectType", &ObjectManager::newObjectType<DynamicGameObject>,
        "newDynamicObject", &ObjectManager::newDynamicObject,
        "addDynamicObject", &ObjectManager::addDynamicObject,
        "findDynamicObject", &ObjectManager::findDynamicObject,
        "newStaticObjectType", &ObjectManager::newObjectType<StaticGameObject>,
        "newStaticObject", &ObjectManager::newStaticObject,
        "addStaticObject", &ObjectManager::addStaticObject,
        "newSpriteObjectType", &ObjectManager::newSpriteObjectType<>,
        "newSpriteObject", &ObjectManager::newSpriteObject);
    lua.new_usertype<GameObject>("GameObject", sol::constructors<GameObject()>(),
        "playAnimation", &GameObject::playAnimation,
        "getAnimation", &GameObject::getAnimation,
        "getCurrentAnimationNum", &GameObject::getCurrentAnimationNum,
        "scale", &GameObject::scale,
        "getPosition", &GameObject::getPosition,
        "setRotation", &GameObject::setRotation,
        "setPosition", sol::overload(
            sol::resolve<void(float, float)>(&GameObject::setPosition),
            sol::resolve<void(const sf::Vector2f&)>(&GameObject::setPosition)),
        "setCenter", sol::overload(
            sol::resolve<void(float, float)>(&GameObject::setCenter),
            sol::resolve<void(const sf::Vector2f&)>(&GameObject::setCenter)
        ),
        "move", &GameObject::move,
        "mirrorFlip", &GameObject::mirrorFlip,
        "mirrorUnFlip", &GameObject::mirrorUnFlip,
        "getCurrentAnimationFrame", &GameObject::getCurrentAnimationFrame,
        "isMirror", &GameObject::isMirror,
        "getGlobalCenter", &GameObject::getGlobalCenter,
        "getLocalCenter", &GameObject::getLocalCenter,
        "setRotationAroundCentre", &GameObject::setRotationAroundCentre,
        "setLooped", &GameObject::setLooped);
    lua.new_usertype<ParameterObject>("ParameterObject", sol::constructors<ParameterObject(int)>(),
        "setHp", &ParameterObject::setHp,
        "setMaxHp", &ParameterObject::setMaxHp,
        "setDamage", &ParameterObject::setDamage,
        "setState", &ParameterObject::setState,
        "getDamage", &ParameterObject::getDamage,
        "getState", &ParameterObject::getState,
        "getTarget", &ParameterObject::getTarget);
    lua.new_usertype<DynamicGameObject>("DynamicGameObject", sol::constructors<DynamicGameObject()>(),
        sol::base_classes, sol::bases<GameObject, ParameterObject>(),
        "getVelocity", &DynamicGameObject::getVelocity,
        "setSlowMode", &DynamicGameObject::setSlowMode,
        "setVelocity", sol::overload(
            sol::resolve<void(const sf::Vector2f&)>(&DynamicGameObject::setVelocity),
            sol::resolve<void(float, float)>(&DynamicGameObject::setVelocity)
        ),
        "deactivate", &DynamicGameObject::deactivate,
        "setActivateTime", &DynamicGameObject::setActivateTime,
        "setDeactivateTime", &DynamicGameObject::setDeactivateTime,
        "getActivateTime", &DynamicGameObject::getActivateTime,
        "getElapsedTime", &DynamicGameObject::getElapsedTime,
        "restartTime", &DynamicGameObject::restartTime);
    lua.new_usertype<sf::Vector2i>("Vector2i", sol::constructors<sf::Vector2i(int, int)>(),
        "x", &sf::Vector2i::x,
        "y", &sf::Vector2i::y);
    lua.new_usertype<sf::Vector2f>("Vector2f", sol::constructors<sf::Vector2f(), sf::Vector2f(float, float)>(),
        "x", &sf::Vector2f::x,
        "y", &sf::Vector2f::y);
    lua["Vector2f"][sol::meta_function::multiplication] = [](sf::Vector2f& vec, float num) -> sf::Vector2f {
        return sf::Vector2f(vec.x * num, vec.y * num);
    };
    lua["Vector2f"][sol::meta_function::addition] = [](sf::Vector2f& vec1, sf::Vector2f& vec2) -> sf::Vector2f {
        return sf::Vector2f(vec1.x + vec2.x, vec1.y + vec2.y);
    };
    lua["Vector2f"][sol::meta_function::subtraction] = [](sf::Vector2f& vec1, sf::Vector2f& vec2) -> sf::Vector2f {
        return sf::Vector2f(vec1.x - vec2.x, vec1.y - vec2.y);
    };
    lua["Vector2f"][sol::meta_function::division] = [](sf::Vector2f& vec, float num) -> sf::Vector2f {
        return sf::Vector2f(vec.x / num, vec.y / num);
    };
    lua.new_usertype<sf::Time>("Time", sol::constructors<sf::Time()>(),
        "asSeconds", &sf::Time::asSeconds,
        "asMilliseconds", &sf::Time::asMilliseconds,
        "asMicroseconds", &sf::Time::asMicroseconds);
    lua.new_usertype<std::vector<sf::Vector2i>>("PathVector",
        "back", sol::resolve<std::vector<sf::Vector2i>::reference(void)>(&std::vector<sf::Vector2i>::back));
    lua.new_usertype<Map>("Map", sol::constructors<Map(ObjectManager&)>(),
        "getPath", &Map::getPath);
    lua["seconds"] = &sf::seconds;
    lua["milliseconds"] = &sf::milliseconds;
    lua["microseconds"] = &sf::microseconds;
    lua["getNormalized"] = &getNormalized;
    // lua["atan"] = &atan;
    lua["map"] = &map;
    sol::load_result script = lua.load_file("scripts/ai.lua");
    assert(script.valid());
    sol::protected_function target = script;
    bcode = target.dump();
}
void AImanager::update()
{
    auto script = lua.safe_script(bcode.as_string_view(), sol::script_pass_on_error);
    assert(script.valid());
    auto objects = oManager.getDynamicObjects();
    for (auto& obj: objects) {
        lua[obj->getAIclass()](obj->getRef());
    }
}