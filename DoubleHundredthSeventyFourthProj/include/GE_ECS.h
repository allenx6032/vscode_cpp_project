#pragma once

#include <bitset>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <set>
#include <memory>
#include <deque>
#include <unordered_map>

#include "GE_Logger.h"

namespace CONST
{
	namespace ECS
	{
		constexpr unsigned int MAX_COMPONENTS = 32;
	}
}

// Signature represents the components that a Entity has or that
// an entity must have for a system to work on it
typedef std::bitset<CONST::ECS::MAX_COMPONENTS> Signature;

class Entity
{
public:
	Entity(std::size_t id);
	void Destroy();

	bool operator==(const Entity& other) const;
	bool operator!=(const Entity& other) const;
	bool operator< (const Entity& other) const;

	std::size_t GetId() const;

	template <typename TComponent, typename ...TArgs>
	void AddComponent(TArgs&&... args);
	template <typename TComponent>
	void RemoveComponent();
	template <typename TComponent>
	bool HasComponent() const;
	template <typename TComponent>
	TComponent& GetComponent() const;

	void Tag(const std::string& tag);
	bool HasTag(const std::string& tag) const;
	void Group(const std::string& group);
	bool BelongsToGroup(const std::string& group) const;


	class Registry* m_registry = nullptr; // forward declaration
private:
	std::size_t m_id;
};

struct IComponent
{
protected:
	static std::size_t m_nextId;
};

// used to assign a unique id to a component type
// ALL COMPONENTS CTOR NEED DEFAULT VALUES OR AN ERROR WILL OCCUR
template <typename T>
class Component : public IComponent
{
public:
	static std::size_t GetId();
};

template<typename T>
inline std::size_t Component<T>::GetId()
{
	static auto id = m_nextId++;
	return id;
}

class System
{
public:
	System() = default;
	~System() = default;

	void AddEntity(Entity entityToAdd);
	void RemoveEntity(Entity entityToRemove);

	std::vector<Entity>& GetSystemEntities();
	const Signature& GetComponentSignature() const;

	// what components will be a part of this system's signature?
	template <typename TComponent> void RequireComponent();
private:
	Signature m_componentSignature;
	std::vector<Entity> m_entities;
};

// this class is used so that in 'Registry' we are not forced to specify
// the Pool data type
class IPool
{
public:
	virtual ~IPool() = default;
	virtual void RemoveEntityFromPool(std::size_t entityId) = 0;
};

// wrapper around a vector
template <typename T>
class Pool : public IPool
{
public:
	Pool(std::size_t capacity = 100)
		: m_size(0)
	{
		m_data.resize(capacity);
	}
	
	virtual ~Pool() = default;

	bool IsEmpty() const { return m_size == 0; }
	std::size_t GetSize() const { return m_size; }
	void Resize(std::size_t size) { m_data.resize(size); }
	void Clear() 
	{
		m_data.clear();  
		m_size = 0;
	}

	void Add(T objectToAdd) { m_data.push_back(objectToAdd); }

	void Set(std::size_t entityId, T objectToSet)
	{
		const bool entityAlreadyExists = m_entityIdToIndex.find(entityId) != m_entityIdToIndex.end();
		if (entityAlreadyExists)
		{
			int index = m_entityIdToIndex[entityId];
			m_data[index] = objectToSet;
		}
		else
		{
			int index = m_size;
			m_entityIdToIndex.emplace(entityId, index);
			m_indexToEntityId.emplace(index, entityId);
			if (index >= m_data.capacity())
			{
				m_data.resize(m_size * 2);
			}
			m_data.at(index) = objectToSet;
			m_size++;
		}
	}

	void RemoveEntityFromPool(std::size_t entityId) override
	{
		if (m_entityIdToIndex.find(entityId) != m_entityIdToIndex.end())
		{
			Remove(entityId);
		}
	}

	void Remove(std::size_t entityId)
	{
		// swap last element with the element to be removed in the vector
		std::size_t indexOfRemoved = m_entityIdToIndex.at(entityId);
		std::size_t indexOfLast = m_size - 1;
		m_data[indexOfRemoved] = m_data[indexOfLast];

		//  swap last element with the element to be removed on the maps
		int entityIdOfLastElement = m_indexToEntityId[indexOfLast];
		m_entityIdToIndex[entityIdOfLastElement] = indexOfRemoved;
		m_indexToEntityId[indexOfRemoved] = entityIdOfLastElement;

		m_entityIdToIndex.erase(entityId);
		m_indexToEntityId.erase(indexOfLast);

		m_size--;
	}

	T& Get(std::size_t entityId) 
	{ 
		auto index = m_entityIdToIndex.at(entityId);
		return static_cast<T&>(m_data.at(index));
	}

	T& operator[](std::size_t indexToGet) { return m_data.at(indexToGet); }
private:
	std::vector<T> m_data;
	std::size_t m_size;

	// helper maps to keep track of entity ids per m_data index so that it does not have gaps
	std::unordered_map<int, int> m_entityIdToIndex;
	std::unordered_map<int, int> m_indexToEntityId;
};

// manages creation and destruction of entities, add systems,
// and components
class Registry
{
public:
	Registry() = default;

	// ENTITY MANAGEMENT
	Entity CreateEntity();
	void DestroyEntity(Entity& entityToDestroy);

	// COMPONENT MANAGEMENT
	template <typename TComponent, typename ...TArgs>
	void AddComponent(Entity entity, TArgs&& ...args);

	template<typename TComponent>
	void RemoveComponent(Entity entity);

	template <typename TComponent>
	bool HasComponent(Entity entity) const;
	
	template <typename TComponent>
	TComponent& GetComponent(Entity entity) const;

	// SYSTEM MANAGEMENT
	template <typename TSystem, typename ...TArgs>
	void AddSystem(TArgs&& ...args);

	template <typename TSystem> 
	void RemoveSystem();

	template <typename TSystem> 
	bool HasSystem() const;

	template <typename TSystem> 
	TSystem& GetSystem() const;

	void AddEntityToSystems(Entity entityToAdd);
	void RemoveEntityFromSystems(Entity entityToAdd);

	// TAG MANAGEMENT
	void TagEntity(Entity entity, const std::string& tag);
	bool EntityHasTag(Entity entity, const std::string& tag) const;
	Entity GetEntityByTag(const std::string& tag) const;
	void RemoveEntityTag(Entity entity);

	// GROUP MANAGEMENT
	void GroupEntity(Entity entity, const std::string& group);
	bool EntityBelongsToGroup(Entity entity, const std::string& group) const;
	std::vector<Entity> GetEntitiesByGroup(const std::string& group) const;
	void RemoveEntityFromGroup(Entity entity);

	void Update();

private:
	std::size_t m_numEntities = 0;

	// stores the component's signature for each entity.
	// this informs us of which components each entity has
	// [ vector index = entity id ]
	std::vector<Signature> m_entityComponentSignatures;

	// these sets serve as temp Entity holder so that  they are removed/added at the end 
	// of the frame rather than at any random time during the frame
	std::set<Entity> m_entitiesToAdd;
	std::set<Entity> m_entitiesToDestroy;

	// entity tags (one tag per entity)
	std::unordered_map<std::string, Entity> m_entityPerTag;
	std::unordered_map<std::size_t, std::string> m_tagPerEntity;

	// entity groups (a set of entities per group name)
	std::unordered_map<std::string, std::set<Entity>> m_entitiesPerGroup;
	std::unordered_map<std::size_t, std::string> m_groupPerEntity;

	// list of free ids from previously destroyed entitites
	std::deque<std::size_t> m_freeIds;

	// each pool contains all the information for a certain component type
	// [ vector index = component type id ]
	// [ pool index = entity id ]
	std::vector<std::shared_ptr<IPool>> m_componentPools;

	std::unordered_map<std::type_index, std::shared_ptr<System>> m_systems;
};


// ------------------------------------ ENTITY TEMPLATE FUNCTIONS-----------------

template <typename TComponent, typename ...TArgs>
void Entity::AddComponent(TArgs&&... args)
{
	m_registry->AddComponent<TComponent>(*this, args...);
}

template <typename TComponent>
void Entity::RemoveComponent()
{
	m_registry->RemoveComponent<TComponent>(*this);
}

template <typename TComponent>
bool Entity::HasComponent() const
{
	return m_registry->HasComponent<TComponent>(*this);
}

template <typename TComponent>
TComponent& Entity::GetComponent() const
{
	return m_registry->GetComponent<TComponent>(*this);
}


// ------------------------------------ SYSTEM TEMPLATE FUNCTIONS-----------------

template <typename TComponent>
void System::RequireComponent()
{
	const auto componentId = Component<TComponent>::GetId();
	m_componentSignature.set(componentId);
}


// ------------------------------------ REGISTRY TEMPLATE FUNCTIONS-----------------

template <typename TComponent, typename ...TArgs>
void Registry::AddComponent(Entity entity, TArgs&& ...args)
{
	const auto componentId = Component<TComponent>::GetId();
	const auto entityId = entity.GetId();

	// check if the component already exists in the componentPool
	// resize the componentPool if it does not
	const bool componentAlreadyExists = m_componentPools.size() > componentId;
	if (!componentAlreadyExists)
	{
		m_componentPools.resize(componentId + 1, nullptr);
	}

	if (!m_componentPools.at(componentId))
	{
		std::shared_ptr<Pool<TComponent>> newComponentPool = std::make_shared<Pool<TComponent>>();
		m_componentPools.at(componentId) = newComponentPool;
	}

	std::shared_ptr<Pool<TComponent>> componentToAddPool = std::static_pointer_cast<Pool<TComponent>>(m_componentPools.at(componentId));
	// check if the pool is sized large enough to fit the new component

	TComponent componentToAdd(std::forward<TArgs>(args)...);
	componentToAddPool->Set(entityId, componentToAdd);

	// update the entity's signature to have the added component
	m_entityComponentSignatures.at(entityId).set(componentId);

	//Logger::Log("Component id = " + std::to_string(componentId) + " was added to entity id = " + std::to_string(entityId));
}

template<typename TComponent>
void Registry::RemoveComponent(Entity entity)
{
	const auto componentId = Component<TComponent>::GetId();
	const auto entityId = entity.GetId();
	
	std::shared_ptr<Pool<TComponent>> componentToAddPool = std::static_pointer_cast<Pool<TComponent>>(m_componentPools.at(componentId));
	componentToAddPool->Remove(entityId);
	
	m_entityComponentSignatures.at(entityId).set(componentId, false);

	Logger::Log("Component id = " + std::to_string(componentId) + " was removed from entity id = " + std::to_string(entityId));
}

template <typename TComponent>
bool Registry::HasComponent(Entity entity) const
{
	const auto componentId = Component<TComponent>::GetId();
	const auto entityId = entity.GetId();
	
	return m_entityComponentSignatures.at(entityId).test(componentId);
}

template <typename TComponent>
TComponent& Registry::GetComponent(Entity entity) const
{
	const auto componentId = Component<TComponent>::GetId();
	const auto entityId = entity.GetId();
	auto componentPool = std::static_pointer_cast<Pool<TComponent>>(m_componentPools.at(componentId));

	return componentPool->Get(entityId);
}

template <typename TSystem, typename ...TArgs>
void Registry::AddSystem(TArgs&& ...args)
{
	std::shared_ptr<TSystem> newSystem =std::make_shared<TSystem>(std::forward<TArgs>(args)...);

	m_systems.insert(std::make_pair(std::type_index(typeid(TSystem)), newSystem));
}

template <typename TSystem>
void Registry::RemoveSystem()
{
	const auto sytemToRemoveIndex = m_systems.find(std::type_index(typeid(TSystem)));
	m_systems.erase(sytemToRemoveIndex);
}

template <typename TSystem>
bool Registry::HasSystem() const
{
	const auto requestedSystemIndex = m_systems.find(std::type_index(typeid(TSystem)));
	return requestedSystemIndex != m_systems.end();
}

template <typename TSystem>
TSystem& Registry::GetSystem() const
{
	auto systemToGet = m_systems.find(std::type_index(typeid(TSystem)));
	return *(std::static_pointer_cast<TSystem>(systemToGet->second));
}