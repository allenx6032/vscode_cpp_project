#include "2dge_game.hpp"
static const float REPULSION_RATE = 50;
AssetManager::AssetManager()
{
    __DEBUG_EXEC(std::cout << "AssetManager()\n");
}
AssetManager::~AssetManager()
{
    __DEBUG_EXEC(std::cout << "~AssetManager()\n");
}
StateManager::StateManager(sf::RenderWindow& window, AssetManager& asManager)
{
    __DEBUG_EXEC(std::cout << "StateManager(sf::RenderWindow&, AssetManager&)\n");
    statVec.push_back(std::move(std::unique_ptr<MenuState>(new MenuState(window, asManager))));
    statVec.push_back(std::move(std::unique_ptr<GameState>(new GameState(window, asManager))));
    currentStateIndex = STATE::MENU;
}
StateManager::~StateManager()
{
    __DEBUG_EXEC(std::cout << "~StateManager()\n");
}
void StateManager::addState(State* state_ptr)
{
    if (state_ptr)
        statVec.push_back(std::move(std::unique_ptr<State>(state_ptr)));
}
int StateManager::setState(std::size_t index)
{
    State* state_ptr = statVec[index].get();
    if (state_ptr) {
        currentStateIndex = index;
        state_ptr->resume();
        return state_ptr->gameLoop();
    }
    currentStateIndex = STATE::CLOSED;
    fprintf(stderr, "State do not exist\n");
    return STATE::CLOSED;
}
ObjectManager::ObjectManager()
{
    __DEBUG_EXEC(std::cout << "ObjectManager()\n");
    init();
}
void ObjectManager::addScript(const std::string& name, std::size_t index1, std::size_t index2)
{
    assert(index2 < scriptVec.size());
    assert(index1 < scriptVec[index2].size());
    scriptVec[index1][index2] = name;
}
void ObjectManager::init()
{
    lua.open_libraries(sol::lib::base, sol::lib::package);
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
    lua["oManager"] = std::ref(*this);
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
    lua.new_usertype<std::pair<int, int>>("PairII", sol::constructors<std::pair<int, int>(int, int)>(),
        "first", &std::pair<int, int>::first,
        "second", &std::pair<int, int>::second);
    lua.new_usertype<sf::Vector2f>("Vector2f", sol::constructors<sf::Vector2f(), sf::Vector2f(float, float)>(),
        "x", &sf::Vector2f::x,
        "y", &sf::Vector2f::y);
    lua["Vector2f"][sol::meta_function::multiplication] = [](sf::Vector2f& vec, float num) -> sf::Vector2f {
        return sf::Vector2f(vec.x * num, vec.y * num);
    };
    lua["Vector2f"][sol::meta_function::addition] = [](sf::Vector2f& vec1, sf::Vector2f vec2) -> sf::Vector2f {
        return sf::Vector2f(vec1.x + vec2.x, vec1.y + vec2.y);
    };
    lua["Vector2f"][sol::meta_function::subtraction] = [](sf::Vector2f& vec1, sf::Vector2f vec2) -> sf::Vector2f {
        return sf::Vector2f(vec1.x - vec2.x, vec1.y - vec2.y);
    };
    lua["Vector2f"][sol::meta_function::division] = [](sf::Vector2f& vec, float num) -> sf::Vector2f {
        return sf::Vector2f(vec.x / num, vec.y / num);
    };
    lua.new_usertype<sf::Time>("Time", sol::constructors<sf::Time()>(),
    "asSeconds", &sf::Time::asSeconds,
    "asMilliseconds", &sf::Time::asMilliseconds,
    "asMicroseconds", &sf::Time::asMicroseconds);
    lua["seconds"] = &sf::seconds;
    lua["milliseconds"] = &sf::milliseconds;
    lua["microseconds"] = &sf::microseconds;
    lua["getNormalized"] = &getNormalized;
    lua["atan"] = &atan;
    sol::protected_function_result script_result = lua.script_file("scripts/objects.lua");
    std::size_t type_max = lua["setting"]["type_max"];
    std::size_t action_type_max = lua["setting"]["action_type_max"];
    scriptVec = std::vector<std::vector<std::string>>(type_max, std::vector<std::string>(action_type_max, "nope"));
    script_result = lua.script_file("scripts/action_init.lua");
    assert(script_result.valid());
    sol::load_result script = lua.load_file("scripts/actions.lua");
    assert(script.valid());
    sol::protected_function target = script;
    bcode = target.dump();
}
ObjectManager::~ObjectManager() 
{
    __DEBUG_EXEC(std::cout << "~ObjectManager()\n");
}
std::shared_ptr<StaticGameObject> ObjectManager::newStaticObject(std::size_t type)
{
    for (auto& obj: stObjVec) {
        if (obj->getType() == type && !obj->isActive()) {
            obj->reset();
            obj->activate();
            return obj;
        }
    }
    for (auto& obj: typeObjVec) {
        if (obj->getType() == type) {
            auto obj_ptr =  std::make_shared<StaticGameObject>(static_cast<StaticGameObject&>(*obj));
            stObjVec.push_back(obj_ptr);
            return obj_ptr;
        }
    }
    std::cerr << "_____ERROR: Static object type " << type << " is uninitialized\n";
    return nullptr;
}
std::shared_ptr<SpriteObject> ObjectManager::newSpriteObject(std::size_t type)
{
    for (auto& obj: spriteObjVecType) {
        if (obj->getType() == type) {
            auto obj_ptr =  std::make_shared<SpriteObject>(*obj);
            spriteObjVec.push_back(obj_ptr);
            return obj_ptr;
        }
    }
    std::cerr << "_____ERROR: Static object type " << type << " is uninitialized\n";
    return nullptr;
}
std::shared_ptr<DynamicGameObject> ObjectManager::newDynamicObject(std::size_t type) 
{
		for (auto& obj: dynObjVec) {
			if (obj->getType() == type && !obj->isActive()) {
				obj->reset();
				obj->activate();
				return obj;
			}
		}
		for (auto& obj: typeObjVec) {
			if (obj->getType() == type) {
				auto obj_ptr =  std::make_shared<DynamicGameObject>(
					static_cast<DynamicGameObject&>(obj->getRef()));
				dynObjVec.push_back(obj_ptr);
				return obj_ptr;
			}
		}
		std::cerr << "_____ERROR: Dynamic object type " << type << " is uninitialized\n";
		return nullptr;
	}
std::shared_ptr<DynamicGameObject> ObjectManager::findDynamicObject(std::size_t type) 
{
    for (auto& obj: dynObjVec) {
        if (obj->getType() == type && obj->isActive())
            return obj;
    }
    return nullptr;
}
std::shared_ptr<StaticGameObject> ObjectManager::findStaticObject(std::size_t type) 
{
    for (auto& obj: stObjVec) {
        if (obj->getType() == type && obj->isActive())
            return obj;
    }
    return nullptr;
}
void ObjectManager::addDynamicObject(std::shared_ptr<DynamicGameObject> obj_ptr)
{
    assert(obj_ptr);
    dynObjVec.push_back(obj_ptr);
}
void ObjectManager::addStaticObject(std::shared_ptr<StaticGameObject> obj_ptr)
{
    assert(obj_ptr);
    stObjVec.push_back(obj_ptr);
}
std::shared_ptr<DynamicGameObject> ObjectManager::getDynObject(std::size_t num) const
{
    if (num <= dynObjVec.size())
        return dynObjVec[num];
    return nullptr;
}
std::shared_ptr<StaticGameObject> ObjectManager::getStObject(std::size_t num) const
{
    if (num <= stObjVec.size())
        return stObjVec[num];
    return nullptr;
}
const std::size_t ObjectManager::getDynSize() const
{
    return dynObjVec.size();
}
const std::size_t ObjectManager::getStSize() const
{
    return stObjVec.size();
}
void ObjectManager::render(sf::RenderTarget& target, sf::Time frameTime, sf::View view)
{
    auto viewPos = view.getCenter();
    auto viewSize = view.getSize();
    for (auto& obj: spriteObjVec) {
        auto spritePos = obj->sprite.getPosition();
        bool inRange = abs(spritePos.x - viewPos.x) < viewSize.x &&
            abs(spritePos.y - viewPos.y) < viewSize.y;
        if (obj && inRange)
            target.draw(obj->sprite);
    }
    for (auto& obj: dynObjVec) {
        auto objPos = obj->getPosition();
        bool inRange = abs(objPos.x - viewPos.x) < viewSize.x &&
            abs(objPos.y - viewPos.y) < viewSize.y;
        if (obj && obj->isActive() && inRange)
            obj->render(target, frameTime);
    }
    for (auto& obj: stObjVec) {
        auto objPos = obj->getPosition();
        bool inRange = abs(objPos.x - viewPos.x) < viewSize.x &&
            abs(objPos.y - viewPos.y) < viewSize.y;
        if (obj && obj->isActive() && inRange)
            obj->render(target, frameTime);
    }
}
void ObjectManager::interact()
{
    auto dynsize = dynObjVec.size();
    sf::Vector2f mtv;
    sf::Vector2f* mtv_ptr = &mtv;
    for (std::size_t i = 0; i < dynsize - 1; i++) {
        for (std::size_t j = i + 1; j < dynsize; j++) {
            bool isInteract = dynObjVec[i]->isActive() 
                              && !dynObjVec[i]->getGhostMode()
                              && dynObjVec[j]->isActive()
                              && !dynObjVec[j]->getGhostMode();
            if (isInteract) {
                if (collision.isCollide(dynObjVec[i]->getCollider(), dynObjVec[j]->getCollider(), CONVEX_MODE, mtv_ptr)) {
                    intManager.interact(dynObjVec[i]->getRef(), dynObjVec[j]->getRef(), mtv, CONVEX_MODE);
                    intManager.interact(dynObjVec[j]->getRef(), dynObjVec[i]->getRef(), -mtv, CONVEX_MODE);
                }
                if (collision.isCollide(dynObjVec[i]->getCollider(), dynObjVec[j]->getCollider(), CIRCLE_MODE, mtv_ptr)) {
                    intManager.interact(dynObjVec[i]->getRef(), dynObjVec[j]->getRef(), mtv, CIRCLE_MODE);
                    intManager.interact(dynObjVec[j]->getRef(), dynObjVec[i]->getRef(), -mtv, CIRCLE_MODE);
                }
            }
        }
    }
    auto stsize = stObjVec.size();
    for (std::size_t i = 0; i < stsize; i++) {
        auto pos1 = stObjVec[i]->getPosition();
        for (std::size_t j = 0; j < dynsize; j++) {
            auto pos2 = dynObjVec[j]->getPosition();
            bool inRange = abs(pos1.x - pos2.x) < 1000 && abs(pos1.y - pos2.y) < 1000;
            if (!dynObjVec[j]->getGhostMode() && dynObjVec[j]->isActive() && stObjVec[i]->isActive() && inRange)
                if (collision.isCollide(dynObjVec[j]->getCollider(), stObjVec[i]->getCollider(), CONVEX_MODE, mtv_ptr)) {
                    intManager.interact(dynObjVec[j]->getRef(), stObjVec[i]->getRef(), mtv, CONVEX_MODE);
                    intManager.interact(stObjVec[i]->getRef(), dynObjVec[j]->getRef(), mtv, CONVEX_MODE);
                }
        }
    }
}
void ObjectManager::update()
{
    for (auto& obj: dynObjVec) {
        auto activateTime = obj->getActivateTime();
        if (!obj->isActive() && activateTime != sf::Time::Zero
        && obj->getElapsedTime() > activateTime) {
            obj->activate();
            obj->setActivateTime(sf::Time::Zero);
        }
        auto deactivateTime = obj->getDeactivateTime();
        if (obj->isActive() && deactivateTime != sf::Time::Zero
        && obj->getElapsedTime() > deactivateTime) {
            obj->deactivate();
            obj->setDeactivateTime(sf::Time::Zero);
        }
        if (obj && obj->isActive())
            obj->update();
    }
}
const std::vector<std::shared_ptr<DynamicGameObject>>& ObjectManager::getDynamicObjects() const
{
    return dynObjVec;
}
App::App() 
:asManager(),
sManager(window, asManager)
{
    __DEBUG_EXEC(std::cout << "App()\n");
    LuaScript script("data/setting.lua");
    int width = script.get<int>("Window.Screen_resolution.width");
    int height = script.get<int>("Window.Screen_resolution.height");
    bool fullScreen = script.get<bool>("Window.FullScreen");
    sf::Vector2i screenDimensions(width, height);
    if (fullScreen)
        window.create(sf::VideoMode(screenDimensions.x, screenDimensions.y), "view test", sf::Style::Fullscreen);
    else
        window.create(sf::VideoMode(screenDimensions.x, screenDimensions.y), "view test");
    window.setFramerateLimit(144);
}
App::~App()
{
    __DEBUG_EXEC(std::cout << "~App()\n");
}
int App::run()
{   
    int nextStateIndex = STATE::GAME;
    while ((nextStateIndex = sManager.setState(nextStateIndex)) != STATE::CLOSED);
    return EXIT_SUCCESS;
}