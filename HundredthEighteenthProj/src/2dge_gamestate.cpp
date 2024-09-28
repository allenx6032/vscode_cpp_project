#include "2dge_state.hpp"
static const double PI = 3.1415926535;
GameState::GameState(sf::RenderWindow& window_, AssetManager& asManager_):
    State(window_, asManager_),
    player(oManager),
    map(oManager),
    aiManager(oManager, map)
{
    __DEBUG_EXEC(std::cout << "GameState(sf::RenderWindow&, AssetManager&)\n");
	nextStateIndex = STATE::GAME;
	isPaused = false;
    view.setSize(1920, 1080);
    view.setViewport(sf::FloatRect(0.f, 0.f, 1.f, 1.f));
    minimap.setSize(1920 * (200 * 128 / 1920), 1080 * (200 * 128 / 1080));
    minimap.setViewport(sf::FloatRect(0.75f, 0.f, 0.25f, 0.25f));
    window.setView(view);
    window.setView(minimap);
	init();
}
GameState::~GameState()
{
	__DEBUG_EXEC(std::cout << "~GameState()\n");
}
void GameState::init()
{
    sol::state lua;
    lua.open_libraries(sol::lib::base, sol::lib::package);
    lua.new_usertype<AssetManager>("AssetManager", sol::constructors<AssetManager()>(),
        "createTexture", &AssetManager::createTexture<>,
        "getTexture", &AssetManager::get<sf::Texture>,
        "createAnimation", &AssetManager::createAnimation<>,
        "getAnimation", &AssetManager::get<Animation>);
    lua.new_usertype<ObjectManager>("ObjectManager", sol::constructors<ObjectManager()>(),
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
    lua.new_usertype<Animation>("Animation", sol::constructors<Animation()>(),
        "setSpriteSheet", sol::resolve<void(std::shared_ptr<sf::Texture>)>(&Animation::setSpriteSheet),
        "addFrame", &Animation::addFrame);
    lua.new_usertype<sf::Texture>("Texture", sol::constructors<sf::Texture(), sf::Texture(const sf::Texture&)>(),
        "loadFromFile", &sf::Texture::loadFromFile,
        "create", &sf::Texture::create);    
    lua.new_usertype<sf::IntRect>("IntRect", sol::constructors<sf::IntRect(), sf::IntRect(int, int, int, int)>());
    lua.new_usertype<GameObject>("GameObject", sol::constructors<GameObject()>(),
        "addAnimation", &GameObject::addAnimation,
        "setAnimation", &GameObject::setAnimation,
        "playAnimation", &GameObject::playAnimation,
        "getAnimation", &GameObject::getAnimation,
        "setRotation", &GameObject::setRotation,
        "setOrigin", sol::resolve<void(float, float)>(&GameObject::setOrigin),
        "setType", &GameObject::setType,
        "scale", &GameObject::scale,
        "setScale", &GameObject::setScale,
        "setInteractRadius", &GameObject::setInteractRadius,
        "setBoundyBoxVertex", sol::resolve<void(float, float, std::size_t)>(&GameObject::setBoundyBoxVertex),
        "setPosition", sol::overload(
            sol::resolve<void(float, float)>(&GameObject::setPosition),
            sol::resolve<void(const sf::Vector2f&)>(&GameObject::setPosition)));    
    lua.new_usertype<ParameterObject>("ParameterObject", sol::constructors<ParameterObject(int)>(),
        "setHp", ParameterObject::setHp,
        "getHp", ParameterObject::getHp,
        "setMaxHp", ParameterObject::setMaxHp,
        "setDamage", ParameterObject::setDamage,
        "getDamage", ParameterObject::getDamage);
    lua.new_usertype<StaticGameObject>("StaticGameObject", sol::constructors<StaticGameObject()>(),
        sol::base_classes, sol::bases<GameObject>());
    lua.new_usertype<DynamicGameObject>("DynamicGameObject", sol::constructors<DynamicGameObject()>(),
        sol::base_classes, sol::bases<GameObject, ParameterObject>(),
        "setAIclass", DynamicGameObject::setAIclass);
    lua.new_usertype<SpriteObject>("SpriteObject", sol::constructors<SpriteObject()>(),
        "sprite", &SpriteObject::sprite,
        "setTexture", &SpriteObject::setTexture,
        "getType", &SpriteObject::getType,
        "setType", &SpriteObject::setType);
    lua.new_usertype<sf::Sprite>("Sprite", sol::constructors<sf::Sprite()>(),
        "setTexture", &sf::Sprite::setTexture,
        "setTextureRect", &sf::Sprite::setTextureRect,
        "setScale", sol::resolve<void(float, float)>(&sf::Sprite::setScale));
    lua.new_usertype<Player>("Player", sol::constructors<Player(ObjectManager&)>(),
        "setObject", sol::resolve<void(std::shared_ptr<DynamicGameObject>)>(&Player::setObject));
    lua["asManager"] = std::ref(asManager); 
    lua["oManager"] = std::ref(oManager);
    lua["player"] = std::ref(player);
    lua.script_file("scripts/gamestate_init.lua");
    map.generate(200, 0.4, 4, 3, 4);
    map.load();
}
void GameState::pause()
{
	isPaused = true;
}
void GameState::resume()
{
	isPaused = false;
}
void GameState::processInput()
{
    static sf::Keyboard::Key lastKey;
    static bool RMBblock;
    auto obj_ptr = player.getObject();
    assert(obj_ptr);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) 
        oManager.action(*obj_ptr, ActionType::MoveDown);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) 
        oManager.action(*obj_ptr, ActionType::MoveUp);
    bool A = sf::Keyboard::isKeyPressed(sf::Keyboard::A);
    bool D = sf::Keyboard::isKeyPressed(sf::Keyboard::D);
    do {
        if (A && D && lastKey == sf::Keyboard::D) {
            oManager.action(*obj_ptr, ActionType::MoveLeft);
            break;
        } 
        if (D && A && lastKey == sf::Keyboard::A) {
            oManager.action(*obj_ptr, ActionType::MoveRight);
            break;
        }
        if (D) {
            oManager.action(*obj_ptr, ActionType::MoveRight);
            lastKey = sf::Keyboard::D;
            break;
        }
        if (A) {
            oManager.action(*obj_ptr, ActionType::MoveLeft);
            lastKey = sf::Keyboard::A;
            break;
        }
    } while (false);
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        sf::Mouse mouse;
    	sf::Vector2i mouse_pos = mouse.getPosition(window);
		auto new_mouse_pos = window.mapPixelToCoords(mouse_pos);
        oManager.action(*obj_ptr, ActionType::Attack, new_mouse_pos);
        //player.shoot(window);
    }
    if (sf::Mouse::isButtonPressed(sf::Mouse::Right) && !RMBblock) {
        RMBblock = true;
        sf::Mouse mouse;
        sf::Vector2i mouse_pos = mouse.getPosition(window);
		auto new_mouse_pos = window.mapPixelToCoords(mouse_pos);
        oManager.action(*obj_ptr, ActionType::Special, new_mouse_pos);
    }
    if (!sf::Mouse::isButtonPressed(sf::Mouse::Right)) 
        RMBblock = false;
}
void GameState::update()
{
    oManager.update();
    oManager.interact();
    aiManager.update();
    sf::Mouse mouse;
    auto player_pos = player.getPosition();
    auto size = player.getObject()->getAnimation(0)->getFrame(0);
    auto window_size = window.getSize();
    auto mouse_pos = mouse.getPosition(window);
    auto new_mouse_pos = sf::Vector2f(mouse_pos.x, mouse_pos.y) - (sf::Vector2f(window_size.x, window_size.y) / 2.f);
    auto view_pos = player_pos + new_mouse_pos * 0.2f + sf::Vector2f(size.width / 2, size.height / 2); 
    view.setCenter(view_pos);
}
void GameState::render(sf::RenderTarget& target, sf::Time frameTime)
{
    target.clear(sf::Color(0, 0, 0));
    window.setView(view);
    oManager.render(target, frameTime, view);
    //window.setView(minimap);
    //oManager.render(target, frameTime);
}
int GameState::gameLoop()
{
	sf::Time accumulator = sf::seconds(0.f);
    sf::Time currentTime = sf::seconds(0.f);
    sf::Clock clock;
    while (window.isOpen() && !isPaused)
    {
        sf::Time newTime = clock.getElapsedTime();
        sf::Time frameTime = newTime - currentTime;
        //printf("FPS: %lf \n", 1 / frameTime.asSeconds());
        if (frameTime >= sf::seconds(0.25f))
            frameTime = sf::seconds(0.25f);
        currentTime = newTime;
        accumulator += frameTime;
        sf::Event event;
        processInput();
        while (window.pollEvent(event)) {
            switch (event.type) {
            case sf::Event::Closed:
                window.close();
                return STATE::CLOSED;
            case sf::Event::KeyPressed:
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
                return STATE::CLOSED;
            default:
                break;
            }
        }
        while (accumulator >= deltaTime) {
            accumulator -= deltaTime;
            update();
        }
        render(window, frameTime);   
        window.display();
    }
    return nextStateIndex;
}
