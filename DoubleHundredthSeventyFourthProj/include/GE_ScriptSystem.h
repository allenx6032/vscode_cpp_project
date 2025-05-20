#pragma once

#include <tuple>
#include <sol.hpp>

#include "GE_ECS.h"

#include "GE_ScriptComponent.h"
#include "GE_TransformComponent.h"
#include "GE_RigidbodyComponent.h"
#include "GE_ProjectileEmitterComponent.h"
#include "GE_AnimationComponent.h"


//// declaration of native c++ functions that we will bind with lua functions
std::tuple<double, double> GetEntityPosition(Entity entity)
{
	if (entity.HasComponent<TransformComponent>())
	{
		const auto& position = entity.GetComponent<TransformComponent>().m_position;
		return std::make_tuple(position.x, position.y);
	}
	else
	{
		Logger::Error("trying to get the position of an entity that does not have a transform component");
		return std::make_tuple(0, 0);
	}
}

std::tuple<double, double> GetEntityVelocity(Entity entity)
{
	if (entity.HasComponent<RigidbodyComponent>())
	{
		const auto& velocity = entity.GetComponent<RigidbodyComponent>().m_velocity;
		return std::make_tuple(velocity.x, velocity.y);
	}
	else
	{
		Logger::Error("trying to get the velocity of an entity that does not have a rigidbody component");
		return std::make_tuple(0, 0);
	}
}

void SetEntityPosition(Entity entity, double newX, double newY)
{
	if (entity.HasComponent<TransformComponent>())
	{
		auto& position = entity.GetComponent<TransformComponent>().m_position;
		position.x = newX;
		position.y = newY;
	}
	else
	{
		Logger::Error("trying to set the position of an entity that does not have a transform component");
	}
}

void SetEntityRotation(Entity entity, double newRotation)
{
	if (entity.HasComponent<TransformComponent>())
	{
		entity.GetComponent<TransformComponent>().m_rotation = newRotation;
	}
	else
	{
		Logger::Error("trying to set the rotation of an entity that does not have a transform component");
	}
}

void SetEntityVelocity(Entity entity, double velocityX, double velocityY)
{
	if (entity.HasComponent<RigidbodyComponent>())
	{
		auto& entityVelocity = entity.GetComponent<RigidbodyComponent>().m_velocity;

		entityVelocity.x = velocityX;
		entityVelocity.y = velocityY;
	}
	else
	{
		Logger::Error("trying to set the velocity of an entity that does not have a rigidbody component");
	}
}

void SetProjectileVelocity(Entity entity, double velocityX, double velocityY)
{
	if (entity.HasComponent<ProjectileEmitterComponent>())
	{
		auto& projectileVelocity = entity.GetComponent<ProjectileEmitterComponent>().m_velocity;

		projectileVelocity.x = velocityX;
		projectileVelocity.y = velocityY;
	}
	else
	{
		Logger::Error("trying to set the velocity of an entity that does not have a projectile emitter component");
	}
}

void SetAnimationFrame(Entity entity, int frame)
{
	if (entity.HasComponent<AnimationComponent>())
	{
		entity.GetComponent<AnimationComponent>().m_currentFrame = frame;
	}
	else
	{
		Logger::Error("trying to set the animation frame of an entity that does not have a animation component");
	}
}

class ScriptSystem : public System
{
public:
	ScriptSystem()
	{
		RequireComponent<ScriptComponent>();
	}

	void Update(double deltaTime, int ellapsedTime)
	{
		for (auto& entity : GetSystemEntities())
		{
			const auto script = entity.GetComponent<ScriptComponent>();
			script.m_scriptFunction(entity, deltaTime, ellapsedTime);
		}
	}

	void CreateLuaFunctionBindings(sol::state& lua)
	{
		// create entity usertype so that Lua knows what an entity is
		lua.new_usertype<Entity>(
			"entity",
			"get_id", &Entity::GetId,
			"destroy", &Entity::Destroy,
			"has_tag", &Entity::HasTag,
			"belongs_to_group", &Entity::BelongsToGroup
		);
		
		// transform
		lua.set_function("get_position", GetEntityPosition);
		lua.set_function("set_position", SetEntityPosition);		
		lua.set_function("set_rotation", SetEntityRotation);
		
		// rigidbody
		lua.set_function("get_velocity", GetEntityVelocity);
		lua.set_function("set_velocity", SetEntityVelocity);

		// projectile emitter
		lua.set_function("set_projectile_velocity", SetProjectileVelocity);
		
		// sprite renderer
		lua.set_function("set_animation_frame", SetAnimationFrame);
	}
};