/*
* Copyright (c) 2006-2009 Erin Catto http://www.box2d.org
*
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*/

#include "b2Test.h"

#include "b2AddPair.h"
#include "b2ApplyForce.h"
#include "b2BasicSliderCrank.h"
#include "b2BodyTypes.h"
#include "b2Breakable.h"
#include "b2Bridge.h"
#include "b2BulletTest.h"
#include "b2Cantilever.h"
#include "b2Car.h"
#include "b2ContinuousTest.h"
#include "b2Chain.h"
#include "b2CharacterCollision.h"
#include "b2CollisionFiltering.h"
#include "b2CollisionProcessing.h"
#include "b2CompoundShapes.h"
#include "b2Confined.h"
#include "b2ConvexHull.h"
#include "b2ConveyorBelt.h"
#include "b2DistanceTest.h"
#include "b2Dominos.h"
#include "b2DumpShell.h"
#include "b2DynamicTreeTest.h"
#include "b2EdgeShapes.h"
#include "b2EdgeTest.h"
#include "b2Gears.h"
#include "b2HeavyOnLight.h"
#include "b2HeavyOnLightTwo.h"
#include "b2Mobile.h"
#include "b2MobileBalanced.h"
#include "b2MotorJoint_.h"
#include "b2OneSidedPlatform.h"
#include "b2Pinball.h"
#include "b2PolyCollision.h"
#include "b2PolyShapes.h"
#include "b2Prismatic.h"
#include "b2Pulleys.h"
#include "b2Pyramid.h"
#include "b2RayCast.h"
#include "b2Revolute.h"
#include "b2RopeJoint_.h"
#include "b2SensorTest.h"
#include "b2ShapeEditing.h"
#include "b2SliderCrank.h"
#include "b2SphereStack.h"
#include "b2TheoJansen.h"
#include "b2Tiles.h"
#include "b2TimeOfImpact_.h"
#include "b2Tumbler.h"
#include "b2VaryingFriction.h"
#include "b2VaryingRestitution.h"
#include "b2VerticalStack.h"
#include "b2Web.h"

TestEntry g_testEntries[] =
{
	{"Tiles", Tiles::Create},
	{"Heavy on Light", HeavyOnLight::Create},
	{"Heavy on Light Two", HeavyOnLightTwo::Create},
	{"Vertical Stack", VerticalStack::Create},
	{"Basic Slider Crank", BasicSliderCrank::Create},
	{"Slider Crank", SliderCrank::Create},
	{"Sphere Stack", SphereStack::Create},
	{"Convex Hull", ConvexHull::Create},
	{"Tumbler", Tumbler::Create},
	{"Ray-Cast", RayCast::Create},
	{"Dump Shell", DumpShell::Create},
	{"Apply Force", ApplyForce::Create},
	{"Continuous Test", ContinuousTest::Create},
	{"Time of Impact", TimeOfImpact::Create},
	{"Motor Joint", MotorJoint::Create},
	{"One-Sided Platform", OneSidedPlatform::Create},
	{"Mobile", Mobile::Create},
	{"MobileBalanced", MobileBalanced::Create},
	{"Conveyor Belt", ConveyorBelt::Create},
	{"Gears", Gears::Create},
	{"Varying Restitution", VaryingRestitution::Create},
	{"Cantilever", Cantilever::Create},
	{"Character Collision", CharacterCollision::Create},
	{"Edge Test", EdgeTest::Create},
	{"Body Types", BodyTypes::Create},
	{"Shape Editing", ShapeEditing::Create},
	{"Car", Car::Create},
	{"Prismatic", Prismatic::Create},
	{"Revolute", Revolute::Create},
	{"Pulleys", Pulleys::Create},
	{"Polygon Shapes", PolyShapes::Create},
	{"Web", Web::Create},
	{"RopeJoint", RopeJoint::Create},
	{"Pinball", Pinball::Create},
	{"Bullet Test", BulletTest::Create},
	{"Confined", Confined::Create},
	{"Pyramid", Pyramid::Create},
	{"Theo Jansen's Walker", TheoJansen::Create},
	{"Edge Shapes", EdgeShapes::Create},
	{"PolyCollision", PolyCollision::Create},
	{"Bridge", Bridge::Create},
	{"Breakable", Breakable::Create},
	{"Chain", Chain::Create},
	{"Collision Filtering", CollisionFiltering::Create},
	{"Collision Processing", CollisionProcessing::Create},
	{"Compound Shapes", CompoundShapes::Create},
	{"Distance Test", DistanceTest::Create},
	{"Dominos", Dominos::Create},
	{"Dynamic Tree", DynamicTreeTest::Create},
	{"Sensor Test", SensorTest::Create},
	{"Varying Friction", VaryingFriction::Create},
	{"Add Pair Stress Test", AddPair::Create},
	{NULL, NULL}
};
