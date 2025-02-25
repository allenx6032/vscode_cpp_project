// Metadot physics engine is enhanced based on box2d modification
// Metadot code Copyright(c) 2022-2023, KaoruXun All rights reserved.
// Box2d code by Erin Catto licensed under the MIT License
// https://github.com/erincatto/box2d

// MIT License
// Copyright (c) 2022-2023 KaoruXun
// Copyright (c) 2019 Erin Catto

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "b2_body.h"

#include "b2_contact.h"
#include "b2_fixture.h"
#include "b2_joint.h"
#include "b2_world.h"

b2Body::b2Body(const b2BodyDef* bd, b2World* world) {
    b2Assert(bd->position.IsValid());
    b2Assert(bd->linearVelocity.IsValid());
    b2Assert(b2IsValid(bd->angle));
    b2Assert(b2IsValid(bd->angularVelocity));

#ifdef ENABLE_DAMPING
    b2Assert(b2IsValid(bd->angularDamping) && bd->angularDamping >= 0.0f);
    b2Assert(b2IsValid(bd->linearDamping) && bd->linearDamping >= 0.0f);
#endif  // ENABLE_DAMPING

    m_flags = 0;

    if (bd->bullet) {
        m_flags |= e_bulletFlag;
    }
    if (bd->fixedRotation) {
        m_flags |= e_fixedRotationFlag;
    }

#ifdef ENABLE_SLEEPING
    if (bd->allowSleep) {
        m_flags |= e_autoSleepFlag;
    }
    if (bd->awake && bd->type != b2_staticBody) {
        m_flags |= e_awakeFlag;
    }
#endif  // ENABLE_SLEEPING

    if (bd->enabled) {
        m_flags |= e_enabledFlag;
    }

    m_world = world;

    m_xf.p = bd->position;
    m_xf.q.Set(bd->angle);
    m_xf0 = m_xf;

    m_sweep.localCenter.SetZero();
    m_sweep.c0 = m_xf.p;
    m_sweep.c = m_xf.p;
    m_sweep.a0 = bd->angle;
    m_sweep.a = bd->angle;
    m_sweep.alpha0 = 0.0f;

    m_jointList = nullptr;
    m_contactCount = 0;
    m_contactCapacity = 2;
    m_contactList = (b2Contact**)b2Alloc(m_contactCapacity * sizeof(b2Contact*));

    m_prev = nullptr;
    m_next = nullptr;

    m_linearVelocity = bd->linearVelocity;
    m_angularVelocity = bd->angularVelocity;

#ifdef ENABLE_DAMPING
    m_linearDamping = bd->linearDamping;
    m_angularDamping = bd->angularDamping;
#endif  // ENABLE_DAMPING

#ifdef ENABLE_GRAVITY_SCALE
    m_gravityScale = bd->gravityScale;
#endif  // ENABLE_GRAVITY_SCALE

    m_force.SetZero();
    m_torque = 0.0f;

#ifdef ENABLE_SLEEPING
    m_sleepTime = 0.0f;
#endif  // ENABLE_SLEEPING

    m_type = bd->type;

    m_mass = 0.0f;
    m_invMass = 0.0f;

    m_I = 0.0f;
    m_invI = 0.0f;

#ifdef ENABLE_USER_DATA
    m_userData = bd->userData;
#endif  // ENABLE_USER_DATA

    m_fixtureList = nullptr;
    m_fixtureCount = 0;
}

b2Body::~b2Body() {
    // shapes and joints are destroyed in b2World::Destroy
}

void b2Body::SetType(b2BodyType type) {
    b2Assert(m_world->IsLocked() == false);
    if (m_world->IsLocked() == true) {
        return;
    }

    if (m_type == type) {
        return;
    }

    if (m_type == b2_staticBody || type == b2_staticBody) {
        m_world->m_contactManager.m_broadPhase.InvalidateStaticBodies();

        // since the world list is split into two groups with static and non static bodies
        // If the body type changes we must remove and re-insert the body in the correct group
        // Code below is copied from world CreateBody and DestroyBody accordingly

        // Remove world body list.
        if (m_prev) {
            m_prev->m_next = m_next;
        }

        if (m_next) {
            m_next->m_prev = m_prev;
        }

        if (this == m_world->m_bodyListHead) {
            m_world->m_bodyListHead = m_next;
        }

        if (this == m_world->m_bodyListTail) {
            m_world->m_bodyListTail = m_prev;
        }

        // Add to world doubly linked list in the correct group.
        if (type == b2_staticBody) {
            m_prev = m_world->m_bodyListTail;
            m_next = nullptr;

            if (m_world->m_bodyListTail) {
                m_world->m_bodyListTail->m_next = this;
            } else {
                m_world->m_bodyListHead = this;
            }

            m_world->m_bodyListTail = this;
        } else {
            m_prev = nullptr;
            m_next = m_world->m_bodyListHead;

            if (m_world->m_bodyListHead) {
                m_world->m_bodyListHead->m_prev = this;
            } else {
                m_world->m_bodyListTail = this;
            }

            m_world->m_bodyListHead = this;
        }
    }

    m_type = type;

    ResetMassData();

    if (m_type == b2_staticBody) {
        m_linearVelocity.SetZero();
        m_angularVelocity = 0.0f;
        m_sweep.a0 = m_sweep.a;
        m_sweep.c0 = m_sweep.c;

        m_flags &= ~e_awakeFlag;
        UpdateAABBs();
    }

    SET_AWAKE_OR_NONE(this);

    m_force.SetZero();
    m_torque = 0.0f;

    // Delete the attached contacts.
    for (int32 i = 0; i < m_contactCount; ++i) {
        m_world->m_contactManager.Destroy(m_contactList[i]);
    }

    m_contactCount = 0;

    // Touch the proxies so that new contacts will be created (when appropriate)
    // TODO? probably not needed
    /*b2BroadPhase* broadPhase = &m_world->m_contactManager.m_broadPhase;
    for (b2Fixture* f = m_fixtureList; f; f = f->m_next)
    {
      int32 proxyCount = f->m_proxyCount;
      for (int32 i = 0; i < proxyCount; ++i)
      {
        broadPhase->TouchProxy(f->m_proxies[i].proxyId);
      }
    }*/
}

b2Fixture* b2Body::CreateFixture(const b2FixtureDef* def) {
    b2Assert(m_world->IsLocked() == false);
    if (m_world->IsLocked() == true) {
        return nullptr;
    }

    b2BlockAllocator* allocator = &m_world->m_blockAllocator;

    void* memory = allocator->Allocate(sizeof(b2Fixture));
    b2Fixture* fixture = new (memory) b2Fixture;
    fixture->Create(allocator, this, def);

    fixture->m_next = m_fixtureList;
    m_fixtureList = fixture;
    ++m_fixtureCount;

    fixture->m_body = this;

    // Adjust mass properties if needed.
    if (fixture->m_density > 0.0f) {
        ResetMassData();
    }

    if (m_flags & e_enabledFlag) {
        b2BroadPhase* broadPhase = &m_world->m_contactManager.m_broadPhase;
        fixture->UpdateAABB();
        broadPhase->Add(fixture);
        // fixture->CreateProxies(broadPhase, m_xf);
    }

    // Let the world know we have a new fixture. This will cause new contacts
    // to be created at the beginning of the next time step.
    m_world->m_newContacts = true;

    return fixture;
}

b2Fixture* b2Body::CreateFixture(const b2Shape* shape, float density) {
    b2FixtureDef def;
    def.shape = shape;
    def.density = density;

    return CreateFixture(&def);
}

void b2Body::DestroyFixture(b2Fixture* fixture) {
    if (fixture == NULL) {
        return;
    }

    b2Assert(m_world->IsLocked() == false);
    if (m_world->IsLocked() == true) {
        return;
    }

    b2Assert(fixture->m_body == this);

    // Remove the fixture from this body's singly linked list.
    b2Assert(m_fixtureCount > 0);
    b2Fixture** node = &m_fixtureList;
    bool found = false;
    while (*node != nullptr) {
        if (*node == fixture) {
            *node = fixture->m_next;
            found = true;
            break;
        }

        node = &(*node)->m_next;
    }

    // You tried to remove a shape that is not attached to this body.
    b2Assert(found);

    // Destroy any contacts associated with the fixture.
    for (int32 i = 0; i < m_contactCount; ++i) {
        b2Contact* c = m_contactList[i];

        b2Fixture* fixtureA = c->GetFixtureA();
        b2Fixture* fixtureB = c->GetFixtureB();

        if (fixture == fixtureA || fixture == fixtureB) {
            // This destroys the contact and removes it from
            // this body's contact list.
            m_world->m_contactManager.Destroy(c);

            m_contactCount--;
            m_contactList[i] = m_contactList[m_contactCount];
            i--;
        }
    }

    b2BlockAllocator* allocator = &m_world->m_blockAllocator;

    if (m_flags & e_enabledFlag) {
        b2BroadPhase* broadPhase = &m_world->m_contactManager.m_broadPhase;
        broadPhase->Remove(fixture);
    }

    fixture->m_body = nullptr;
    fixture->m_next = nullptr;
    fixture->Destroy(allocator);
    fixture->~b2Fixture();
    allocator->Free(fixture, sizeof(b2Fixture));

    --m_fixtureCount;

    // Reset the mass data.
    ResetMassData();
}

void b2Body::ResetMassData() {
    // Compute mass data from shapes. Each shape has its own density.
    m_mass = 0.0f;
    m_invMass = 0.0f;
    m_I = 0.0f;
    m_invI = 0.0f;
    m_sweep.localCenter.SetZero();

    // Static and kinematic bodies have zero mass.
    if (m_type == b2_staticBody || m_type == b2_kinematicBody) {
        m_sweep.c0 = m_xf.p;
        m_sweep.c = m_xf.p;
        m_sweep.a0 = m_sweep.a;
        return;
    }

    b2Assert(m_type == b2_dynamicBody);

    // Accumulate mass over all fixtures.
    b2Vec2 localCenter = b2Vec2_zero;
    for (b2Fixture* f = m_fixtureList; f; f = f->m_next) {
        if (f->m_density == 0.0f) {
            continue;
        }

        b2MassData massData;
        f->GetMassData(&massData);
        m_mass += massData.mass;
        localCenter += massData.mass * massData.center;
        m_I += massData.I;
    }

    // Compute center of mass.
    if (m_mass > 0.0f) {
        m_invMass = 1.0f / m_mass;
        localCenter *= m_invMass;
    }

    if (m_I > 0.0f && (m_flags & e_fixedRotationFlag) == 0) {
        // Center the inertia about the center of mass.
        m_I -= m_mass * b2Dot(localCenter, localCenter);
        b2Assert(m_I > 0.0f);
        m_invI = 1.0f / m_I;

    } else {
        m_I = 0.0f;
        m_invI = 0.0f;
    }

    // Move center of mass.
    b2Vec2 oldCenter = m_sweep.c;
    m_sweep.localCenter = localCenter;
    m_sweep.c0 = m_sweep.c = b2Mul(m_xf, m_sweep.localCenter);

    // Update center of mass velocity.
    m_linearVelocity += b2Cross(m_angularVelocity, m_sweep.c - oldCenter);
}

void b2Body::SetMassData(const b2MassData* massData) {
    b2Assert(m_world->IsLocked() == false);
    if (m_world->IsLocked() == true) {
        return;
    }

    if (m_type != b2_dynamicBody) {
        return;
    }

    m_invMass = 0.0f;
    m_I = 0.0f;
    m_invI = 0.0f;

    m_mass = massData->mass;
    if (m_mass <= 0.0f) {
        m_mass = 1.0f;
    }

    m_invMass = 1.0f / m_mass;

    if (massData->I > 0.0f && (m_flags & b2Body::e_fixedRotationFlag) == 0) {
        m_I = massData->I - m_mass * b2Dot(massData->center, massData->center);
        b2Assert(m_I > 0.0f);
        m_invI = 1.0f / m_I;
    }

    // Move center of mass.
    b2Vec2 oldCenter = m_sweep.c;
    m_sweep.localCenter = massData->center;
    m_sweep.c0 = m_sweep.c = b2Mul(m_xf, m_sweep.localCenter);

    // Update center of mass velocity.
    m_linearVelocity += b2Cross(m_angularVelocity, m_sweep.c - oldCenter);
}

bool b2Body::ShouldCollide(const b2Body* other) const {
    // At least one body should be dynamic.
    if (m_type != b2_dynamicBody && other->m_type != b2_dynamicBody) {
        return false;
    }

    // Does a joint prevent collision?
    for (b2JointEdge* jn = m_jointList; jn; jn = jn->next) {
        if (jn->other == other) {
            if (jn->joint->m_collideConnected == false) {
                return false;
            }
        }
    }

    return true;
}

void b2Body::SetTransform(const b2Vec2& position, float angle) {
    b2Assert(m_world->IsLocked() == false);
    if (m_world->IsLocked() == true) {
        return;
    }

    m_xf.q.Set(angle);
    m_xf.p = position;
    m_xf0 = m_xf;

    m_sweep.c = b2Mul(m_xf, m_sweep.localCenter);
    m_sweep.a = angle;

    m_sweep.c0 = m_sweep.c;
    m_sweep.a0 = angle;

    UpdateAABBs();

    // Check for new contacts the next step
    m_world->m_newContacts = true;
}

void b2Body::UpdateAABBs() {
    for (b2Fixture* f = m_fixtureList; f; f = f->m_next) {
        f->UpdateAABB();
    }
}

void b2Body::SetEnabled(bool flag) {
    b2Assert(m_world->IsLocked() == false);

    if (flag == IsEnabled()) {
        return;
    }

    if (flag) {
        m_flags |= e_enabledFlag;

        // Create all proxies.
        // TODO
        /*b2BroadPhase* broadPhase = &m_world->m_contactManager.m_broadPhase;
        for (b2Fixture* f = m_fixtureList; f; f = f->m_next)
        {
          f->CreateProxies(broadPhase, m_xf);
        }
        */

        // Contacts are created at the beginning of the next
        m_world->m_newContacts = true;
    } else {
        m_flags &= ~e_enabledFlag;

        // Destroy all proxies.
        // TODO
        /*b2BroadPhase* broadPhase = &m_world->m_contactManager.m_broadPhase;
        for (b2Fixture* f = m_fixtureList; f; f = f->m_next)
        {
          f->DestroyProxies(broadPhase);
        }

        // Destroy the attached contacts.
        b2ContactEdge* ce = m_contactList;
        while (ce)
        {
          b2ContactEdge* ce0 = ce;
          ce = ce->next;
          m_world->m_contactManager.Destroy(ce0->contact);
        }
        m_contactList = nullptr;*/
    }
}

void b2Body::SetFixedRotation(bool flag) {
    bool status = (m_flags & e_fixedRotationFlag) == e_fixedRotationFlag;
    if (status == flag) {
        return;
    }

    if (flag) {
        m_flags |= e_fixedRotationFlag;
    } else {
        m_flags &= ~e_fixedRotationFlag;
    }

    m_angularVelocity = 0.0f;

    ResetMassData();
}

void b2Body::AddContact(b2Contact* c) {
    if (m_contactCount == m_contactCapacity) {
        m_contactCapacity *= 2;
        b2Contact** newList = (b2Contact**)b2Alloc(m_contactCapacity * sizeof(b2Contact*));
        memcpy(newList, m_contactList, m_contactCount * sizeof(b2Contact*));
        b2Free(m_contactList);
        m_contactList = newList;
    }

    m_contactList[m_contactCount++] = c;
}

void b2Body::Dump() {
    int32 bodyIndex = m_islandIndex;

    // %.9g is sufficient to save and load the same value using text
    // FLT_DECIMAL_DIG == 9

    b2Dump("{\n");
    b2Dump("  b2BodyDef bd;\n");
    b2Dump("  bd.type = b2BodyType(%d);\n", m_type);
    b2Dump("  bd.position.Set(%.9g, %.9g);\n", m_xf.p.x, m_xf.p.y);
    b2Dump("  bd.angle = %.9g;\n", m_sweep.a);
    b2Dump("  bd.linearVelocity.Set(%.9g, %.9g);\n", m_linearVelocity.x, m_linearVelocity.y);
    b2Dump("  bd.angularVelocity = %.9g;\n", m_angularVelocity);

#ifdef ENABLE_DAMPING
    b2Dump("  bd.linearDamping = %.9g;\n", m_linearDamping);
    b2Dump("  bd.angularDamping = %.9g;\n", m_angularDamping);
#endif  // ENABLE_DAMPING

    b2Dump("  bd.allowSleep = bool(%d);\n", m_flags & e_autoSleepFlag);
    b2Dump("  bd.awake = bool(%d);\n", m_flags & e_awakeFlag);
    b2Dump("  bd.fixedRotation = bool(%d);\n", m_flags & e_fixedRotationFlag);
    b2Dump("  bd.bullet = bool(%d);\n", m_flags & e_bulletFlag);
    b2Dump("  bd.enabled = bool(%d);\n", m_flags & e_enabledFlag);

#ifdef ENABLE_GRAVITY_SCALE
    b2Dump("  bd.gravityScale = %.9g;\n", m_gravityScale);
#endif  // ENABLE_GRAVITY_SCALE

    b2Dump("  bodies[%d] = m_world->CreateBody(&bd);\n", m_islandIndex);
    b2Dump("\n");
    for (b2Fixture* f = m_fixtureList; f; f = f->m_next) {
        b2Dump("  {\n");
        f->Dump(bodyIndex);
        b2Dump("  }\n");
    }
    b2Dump("}\n");
}
