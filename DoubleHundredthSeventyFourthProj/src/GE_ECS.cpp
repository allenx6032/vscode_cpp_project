#include "GE_pch.h"

#include "GE_ECS.h"

#include "GE_Logger.h"

std::size_t IComponent::m_nextId = 0;

/// ----------------- ENTITY  ----------------------

Entity::Entity(std::size_t id)
	:m_id(id)
{
}

void Entity::Destroy()
{
	m_registry->DestroyEntity(*this);
}

bool Entity::operator==(const Entity& other) const
{
	return GetId() == other.GetId();
}

bool Entity::operator!=(const Entity& other) const
{
	return GetId() != other.GetId();
}

bool Entity::operator<(const Entity& other) const
{
	return GetId() < other.GetId();
}

std::size_t Entity::GetId() const
{
	return m_id;
}

void Entity::Tag(const std::string& tag)
{
	m_registry->TagEntity(*this, tag);
}

bool Entity::HasTag(const std::string& tag) const
{
	return m_registry->EntityHasTag(*this, tag);
}

void Entity::Group(const std::string& group)
{
	m_registry->GroupEntity(*this, group);
}

bool Entity::BelongsToGroup(const std::string& group) const
{
	return m_registry->EntityBelongsToGroup(*this, group);
}

/// ----------------- SYSTEM ----------------------
void System::AddEntity(Entity entityToAdd)
{
	m_entities.push_back(entityToAdd);
}

void System::RemoveEntity(Entity entityToRemove)
{
	m_entities.erase(
		std::remove_if(m_entities.begin(), m_entities.end(), 
			[&entityToRemove](Entity other) 
			{ 
				return other == entityToRemove; 
			}
			), 
		m_entities.end());
}

 std::vector<Entity>& System::GetSystemEntities()
{
	return m_entities;
}

const Signature& System::GetComponentSignature() const
{
	return m_componentSignature;
}

/// ----------------- REGISTRY ----------------------

Entity Registry::CreateEntity()
{
	std::size_t entityId;

	const bool thereAreFreeIds = !m_freeIds.empty();
	if (thereAreFreeIds)
	{
		entityId = m_freeIds.front();
		m_freeIds.pop_front();
	}
	else
	{
		entityId = m_numEntities++;

		bool entityComponentSignatureNotBigEnough = entityId >= m_entityComponentSignatures.size();
		if (entityComponentSignatureNotBigEnough)
		{
			m_entityComponentSignatures.resize(entityId + 1);
		}
	}

	Entity createdEntity(entityId);
	createdEntity.m_registry = this;

	m_entitiesToAdd.insert(createdEntity);
	 
	//Logger::Log("Entity created with id: " + std::to_string(entityId));

	return createdEntity;
}

void Registry::DestroyEntity(Entity& entityToDestroy)
{
	m_entitiesToDestroy.insert(entityToDestroy);
}

void Registry::AddEntityToSystems(Entity entityToAdd)
{
	const auto entityToAddId = entityToAdd.GetId();
	const auto& entityComponentSignature = m_entityComponentSignatures.at(entityToAddId);
	for (auto& system : m_systems)
	{
		const auto& systemComponentSignature = system.second->GetComponentSignature();
		const bool entityHasSystemRequirements = (entityComponentSignature & systemComponentSignature) == systemComponentSignature;
		if (entityHasSystemRequirements)
		{
			system.second->AddEntity(entityToAdd);
		}
	}
}

void Registry::RemoveEntityFromSystems(Entity entityToRemove)
{
	for (auto& system : m_systems)
	{
		system.second->RemoveEntity(entityToRemove);
	}
}

// TAG MANAGEMENT
void  Registry::TagEntity(Entity entity, const std::string& tag)
{
	m_tagPerEntity.emplace(entity.GetId(), tag);

	m_entityPerTag.emplace(tag, entity);
}

bool  Registry::EntityHasTag(Entity entity, const std::string& tag) const
{
	if(m_tagPerEntity.find(entity.GetId()) == m_tagPerEntity.end()) 
	{
		return false;
	}
	return m_entityPerTag.find(tag)->second == entity;
}

Entity  Registry::GetEntityByTag(const std::string& tag) const 
{
	return m_entityPerTag.at(tag);
}

void  Registry::RemoveEntityTag(Entity entity)
{
	auto taggedEntity = m_tagPerEntity.find(entity.GetId());
	if (taggedEntity != m_tagPerEntity.end())
	{
		auto tag = taggedEntity->second;
		m_entityPerTag.erase(tag);

		m_tagPerEntity.erase(taggedEntity);
	}
}

// GROUP MANAGEMENT
void  Registry::GroupEntity(Entity entity, const std::string& group)
{
	m_entitiesPerGroup.emplace(group, std::set<Entity>());
	m_entitiesPerGroup[group].emplace(entity);

	m_groupPerEntity.emplace(entity.GetId(), group);
}

bool  Registry::EntityBelongsToGroup(Entity entity, const std::string& group) const
{
	if (m_entitiesPerGroup.find(group) == m_entitiesPerGroup.end())
	{
		//Logger::Log("checking if an entity belongs to a non existing group:[" + group + "]");
		return false;
	}
	auto groupEntities = m_entitiesPerGroup.at(group);
	return groupEntities.find(entity.GetId()) != groupEntities.end();
}

std::vector<Entity> Registry::GetEntitiesByGroup(const std::string& group) const
{
	auto& setOfEntities = m_entitiesPerGroup.at(group);
	return std::vector<Entity>(setOfEntities.begin(), setOfEntities.end());
}

void  Registry::RemoveEntityFromGroup(Entity entity)
{
	auto groupedEntity = m_groupPerEntity.find(entity.GetId());
	if (groupedEntity != m_groupPerEntity.end())
	{
		auto group = m_entitiesPerGroup.find(groupedEntity->second);
		if (group != m_entitiesPerGroup.end())
		{
			auto entityInGroup = group->second.find(entity);
			if (entityInGroup != group->second.end())
			{
				group->second.erase(entityInGroup);
			}
		}
		m_groupPerEntity.erase(groupedEntity);
	}
}

void Registry::Update()
{
	// add new entities
	for (auto& entity : m_entitiesToAdd)
	{
		AddEntityToSystems(entity);
	}
	m_entitiesToAdd.clear();

	// destroy entities
	for (auto& entity : m_entitiesToDestroy)
	{
		RemoveEntityFromSystems(entity);

		m_entityComponentSignatures.at(entity.GetId()).reset();

		// remove the entity from the component pools
		for (auto& pool : m_componentPools)
		{
			if (pool)
			{
				pool->RemoveEntityFromPool(entity.GetId());
			}
		}

		// make the entity id available to be reused
		m_freeIds.push_back(entity.GetId());

		RemoveEntityTag(entity);
		RemoveEntityFromGroup(entity);
	}
	m_entitiesToDestroy.clear(); 
}