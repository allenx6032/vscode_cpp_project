#ifndef OBJECTMANAGER_HPP_
#define OBJECTMANAGER_HPP_
#include <SFML_Graphics.hpp>
#include <vector>
#include <memory>
#include "2dge_collision.hpp"
#include "2dge_interaction.hpp"
class ObjectManager
{
	Collision collision;
	InteractionManager intManager;
	sol::state lua;
	sol::bytecode bcode;
	std::vector<std::vector<std::string>> scriptVec;
	std::vector<std::shared_ptr<GameObject>> typeObjVec;
	std::vector<std::shared_ptr<StaticGameObject>> stObjVec;
	std::vector<std::shared_ptr<DynamicGameObject>> dynObjVec;
	std::vector<std::shared_ptr<SpriteObject>> spriteObjVecType;
	std::vector<std::shared_ptr<SpriteObject>> spriteObjVec;
public:
	ObjectManager();
	void init();
	~ObjectManager();
	std::shared_ptr<DynamicGameObject> newDynamicObject(std::size_t type);
	std::shared_ptr<StaticGameObject> newStaticObject(std::size_t type);
	std::shared_ptr<SpriteObject> newSpriteObject(std::size_t type);
	std::shared_ptr<DynamicGameObject> findDynamicObject(std::size_t type);
	std::shared_ptr<StaticGameObject> findStaticObject(std::size_t type);
	template<typename T, typename... Args>
	std::shared_ptr<T> newObjectType(std::size_t type, Args... args) {
		auto obj_ptr = std::make_shared<T>(args...);
		obj_ptr->setType(type);
		assert(obj_ptr);
		typeObjVec.push_back(obj_ptr);
		return obj_ptr;
	}
	template<typename... Args>
	std::shared_ptr<SpriteObject> newSpriteObjectType(std::size_t type, Args... args) {
		auto sprite_ptr = std::make_shared<SpriteObject>(args...);
		sprite_ptr->setType(type);
		assert(sprite_ptr);
		spriteObjVecType.push_back(sprite_ptr);
		return sprite_ptr;
	}
	template<typename... Args>
	std::shared_ptr<StaticGameObject> createStaticObject(Args... args) {
		auto obj_ptr = std::make_shared<StaticGameObject>(args...);
		assert(obj_ptr);
		stObjVec.push_back(obj_ptr);
		return obj_ptr;
	}
	template<typename... Args>
	std::shared_ptr<DynamicGameObject> createDynamicObject(Args... args) {
		auto obj_ptr = std::make_shared<DynamicGameObject>(args...);
		assert(obj_ptr);
		dynObjVec.push_back(obj_ptr);
		return obj_ptr;
	}
	template<typename... Args>
	void action(DynamicGameObject& object, std::size_t actionType, Args... args) {
    	auto obj_type = object.getType();
    	assert(obj_type < scriptVec.size());
    	assert(actionType < scriptVec[obj_type].size());
    	auto script = lua.safe_script(bcode.as_string_view(), sol::script_pass_on_error);
    	assert(script.valid());
    	lua[scriptVec[obj_type][actionType]](object.getRef(), args...);
	}
	void addDynamicObject(std::shared_ptr<DynamicGameObject> obj_ptr);
	void addStaticObject(std::shared_ptr<StaticGameObject> obj_ptr);
	std::shared_ptr<DynamicGameObject> getDynObject(std::size_t num) const;
	std::shared_ptr<StaticGameObject> getStObject(std::size_t num) const;
	const std::size_t getDynSize() const;
	const std::size_t getStSize() const;
	const std::vector<std::shared_ptr<DynamicGameObject>>& getDynamicObjects() const;
	void render(sf::RenderTarget& target, sf::Time frameTime, sf::View view);
	void addScript(const std::string& name, std::size_t index1, std::size_t index2);
	void update();
	void interact();
};
#endif //OBJECTMANAGER_HPP_