/**
 * Copyright (c) 2006-2016 LOVE Development Team
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 **/

#ifndef LOVE_PHYSICS_BOX2D_BODY_H
#define LOVE_PHYSICS_BOX2D_BODY_H

// LOVE
#include "love_math.h"
#include "love_runtime.h"
#include "love_Object.h"
#include "love_Body.h"

// Box2D
#include <Box2D.h>

namespace love
{
namespace physics
{
namespace box2d
{
// Forward declarations.
class World;
class Shape;
class Fixture;

/**
 * This struct is stored in a void pointer in the Box2D Body class. For now, all
 * we need is a Lua reference to arbitrary data, but we might need more later.
 **/
struct bodyudata
{
	// Reference to arbitrary data.
	Reference *ref = nullptr;
};

/**
 * A Body is an entity which has position and orientation
 * in world space. A Body does have collision geometry
 * by itself, but depend on an arbitrary number of child Shape objects
 * which together constitute the final geometry for the Body.
 **/
class Body : public love::physics::Body
{
public:
	// Friends.
	friend class Joint;
	friend class DistanceJoint;
	friend class MouseJoint;
	friend class CircleShape;
	friend class PolygonShape;
	friend class Shape;
	friend class Fixture;

	// The Box2D body. (Should not be public?)
	b2Body *body;

	/**
	 * Create a Body at position p.
	 **/
	Body(World *world, b2Vec2 p, Type type);

	/**
	 * Create a Body from an extant b2Body.
	 **/
	Body(b2Body *b);

	virtual ~Body();

	/**
	 * Gets the current x-position of the Body.
	 **/
	float getX();

	/**
	 * Gets the current y-position of the Body.
	 **/
	float getY();

	/**
	 * Gets the current angle (deg) of the Body.
	 **/
	float getAngle();

	/**
	 * Gets the current position of the Body.
	 * @param[out] x_o The x-component of the position.
	 * @param[out] y_o The y-component of the position.
	 **/
	void getPosition(float &x_o, float &y_o);

	/**
	 * Gets the velocity in the current center of mass.
	 * @param[out] x_o The x-component of the velocity.
	 * @param[out] y_o The y-component of the velocity.
	 **/
	void getLinearVelocity(float &x_o, float &y_o);

	/**
	 * The current center of mass for the Body in world
	 * coordinates.
	 * @param[out] x_o The x-component of the center of mass.
	 * @param[out] y_o The y-component of the center of mass.
	 **/
	void getWorldCenter(float &x_o, float &y_o);

	/**
	 * The current center of mass for the Body in local
	 * coordinates.
	 * @param[out] x_o The x-component of the center of mass.
	 * @param[out] y_o The y-component of the center of mass.
	 **/
	void getLocalCenter(float &x_o, float &y_o);

	/**
	 * Get the current Body spin. (Angular velocity).
	 **/
	float getAngularVelocity() const;

	/**
	 * Gets the Body's mass.
	 **/
	float getMass() const;

	/**
	 * Gets the Body's intertia.
	 **/
	float getInertia() const;

	/**
	 * Gets mass properties.
	 **/
	int getMassData(lua_State *L);

	/**
	 * Gets the Body's angular damping.
	 **/
	float getAngularDamping() const;

	/**
	 * Gets the Body's linear damping.
	 **/
	float getLinearDamping() const;

	/**
	 * Gets the Body's gravity scale.
	 **/
	float getGravityScale() const;

	/**
	 * Gets the type of body this is.
	 **/
	Type getType() const;

	/**
	 * Apply an impulse (jx, jy) with offset (0, 0).
	 **/
	void applyLinearImpulse(float jx, float jy, bool wake);

	/**
	 * Apply an impulse (jx, jy) with offset (rx, ry).
	 **/
	void applyLinearImpulse(float jx, float jy, float rx, float ry, bool wake);

	/**
	 * Apply an angular impulse to the body.
	 **/
	void applyAngularImpulse(float impulse, bool wake);

	/**
	 * Apply torque (t).
	 **/
	void applyTorque(float t, bool wake);

	/**
	 * Apply force (fx, fy) with offset (0, 0).
	 **/
	void applyForce(float fx, float fy, bool wake);

	/**
	 * Apply force (fx, fy) with offset (rx, ry).
	 **/
	void applyForce(float fx, float fy, float rx, float ry, bool wake);

	/**
	 * Sets the x-position of the Body.
	 **/
	void setX(float x);

	/**
	 * Sets the Y-position of the Body.
	 **/
	void setY(float y);

	/**
	 * Sets the current velocity of the Body.
	 **/
	void setLinearVelocity(float x, float y);

	/**
	 * Sets the angle of the Body.
	 **/
	void setAngle(float d);

	/**
	 * Sets the current spin of the Body.
	 **/
	void setAngularVelocity(float r);

	/**
	 * Sets the current position of the Body.
	 **/
	void setPosition(float x, float y);

	/**
	 * Sets the mass from the currently attatched shapes.
	 **/
	void resetMassData();

	/**
	 * Sets mass properties.
	 * @param x The x-coordinate for the local center of mass.
	 * @param y The y-coordinate for the local center of mass.
	 * @param m The mass.
	 * @param i The inertia.
	 **/
	void setMassData(float x, float y, float m, float i);

	/**
	 * Sets just the mass. This is provided as a LOVE bonus. Lovely!
	 * @param m The mass.
	 **/
	void setMass(float m);

	/**
	 * Sets the inertia while keeping the other properties
	 * (mass and local center).
	 * @param i The inertia.
	 **/
	void setInertia(float i);

	/**
	 * Sets the Body's angular damping.
	 **/
	void setAngularDamping(float d);

	/**
	 * Sets the Body's linear damping.
	 **/
	void setLinearDamping(float d);

	/**
	 * Sets the Body's gravity scale.
	 **/
	void setGravityScale(float scale);

	/**
	 * Sets the type of body this is.
	 **/
	void setType(Type type);

	/**
	 * Transforms a point (x, y) from local coordinates
	 * to world coordinates.
	 * @param x The x-coordinate of the local point.
	 * @param y The y-coordinate of the local point.
	 * @param[out] x_o The x-coordinate of the point in world coordinates.
	 * @param[out] y_o The y-coordinate of the point in world coordinates.
	 **/
	void getWorldPoint(float x, float y, float &x_o, float &y_o);

	/**
	 * Transforms a vector (x, y) from local coordinates
	 * to world coordinates.
	 * @param x The x-coordinate of the local vector.
	 * @param y The y-coordinate of the local vector.
	 * @param[out] x_o The x-coordinate of the vector in world coordinates.
	 * @param[out] y_o The y-coordinate of the vector in world coordinates.
	 **/
	void getWorldVector(float x, float y, float &x_o, float &y_o);

	/**
	 * Transforms a series of points (x, y) from local coordinates
	 * to world coordinates.
	 **/
	int getWorldPoints(lua_State *L);

	/**
	 * Transforms a point (x, y) from world coordinates
	 * to local coordinates.
	 * @param x The x-coordinate of the world point.
	 * @param y The y-coordinate of the world point.
	 * @param[out] x_o The x-coordinate of the point in local coordinates.
	 * @param[out] y_o The y-coordinate of the point in local coordinates.
	 **/
	void getLocalPoint(float x, float y, float &x_o, float &y_o);

	/**
	 * Transforms a vector (x, y) from world coordinates
	 * to local coordinates.
	 * @param x The x-coordinate of the world vector.
	 * @param y The y-coordinate of the world vector.
	 * @param[out] x_o The x-coordinate of the vector in local coordinates.
	 * @param[out] y_o The y-coordinate of the vector in local coordinates.
	 **/
	void getLocalVector(float x, float y, float &x_o, float &y_o);

	/**
	 * Gets the velocity on the Body for the given world point.
	 * @param x The x-coordinate of the world point.
	 * @param y The y-coordinate of the world point.
	 * @param[out] x_o The x-component of the velocity vector.
	 * @param[out] y_o The y-component of the velocity vector.
	 **/
	void getLinearVelocityFromWorldPoint(float x, float y, float &x_o, float &y_o);

	/**
	 * Gets the velocity on the Body for the given local point.
	 * @param x The x-coordinate of the local point.
	 * @param y The y-coordinate of the local point.
	 * @param[out] x_o The x-component of the velocity vector.
	 * @param[out] y_o The y-component of the velocity vector.
	 **/
	void getLinearVelocityFromLocalPoint(float x, float y, float &x_o, float &y_o);

	/**
	 * Returns true if the Body is a bullet, false otherwise.
	 **/
	bool isBullet() const;

	/**
	 * Set whether this Body should be treated as a bullet.
	 * Bullets require more processing power than normal shapes.
	 **/
	void setBullet(bool bullet);

	/**
	 * Checks whether a Body is active or not. An inactive body
	 * cannot be interacted with.
	 **/
	bool isActive() const;

	/**
	 * Checks whether a Body is awake or not. A Body
	 * will fall to sleep if nothing happens to it for while.
	 **/
	bool isAwake() const;

	/**
	 * Controls whether this Body should be allowed to sleep.
	 **/
	void setSleepingAllowed(bool allow);
	bool isSleepingAllowed() const;

	/**
	 * Changes the body's active state.
	 **/
	void setActive(bool active);

	/**
	 * Changes the body's sleep state.
	 **/
	void setAwake(bool awake);

	void setFixedRotation(bool fixed);
	bool isFixedRotation() const;

	/**
	 * Get the World this Body resides in.
	 */
	World *getWorld() const;

	/**
	 * Get an array of all the Fixtures attached to this Body.
	 * @return An array of Fixtures.
	 **/
	int getFixtureList(lua_State *L) const;

	/**
	 * Get an array of all Joints attached to this Body.
	 **/
	int getJointList(lua_State *L) const;

	/**
	 * Get an array of all active Contacts attached to this Body.
	 * This list changes during World:update and you may miss some collisions
	 * if you don't use the collision callbacks.
	 **/
	int getContactList(lua_State *L) const;

	/**
	 * Destroy this body.
	 **/
	void destroy();

	/**
	 * This function stores an in-C reference to
	 * arbitrary Lua data in the Box2D Body object.
	 **/
	int setUserData(lua_State *L);

	/**
	 * Gets the data set with setData. If no
	 * data is set, nil is returned.
	 **/
	int getUserData(lua_State *L);

private:

	/**
	 * Gets a 2d vector from the arguments on the stack.
	 **/
	b2Vec2 getVector(lua_State *L);

	/**
	 * Pushed the x- and y-components of a vector on
	 * the stack.
	 **/
	int pushVector(lua_State *L, const b2Vec2 &v);

	// FIXME: This should be a weak reference, rather than being completely
	// unowned?
	World *world;

	bodyudata *udata;

}; // Body

} // box2d
} // physics
} // love

#endif // LOVE_PHYSICS_BOX2D_BODY_H
