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

#include "b2_friction_joint.h"

#include "b2_body.h"
#include "b2_time_step.h"

// Point-to-point constraint
// Cdot = v2 - v1
//      = v2 + cross(w2, r2) - v1 - cross(w1, r1)
// J = [-I -r1_skew I r2_skew ]
// Identity used:
// w k % (rx i + ry j) = w * (-ry i + rx j)

// Angle constraint
// Cdot = w2 - w1
// J = [0 0 -1 0 0 1]
// K = invI1 + invI2

void b2FrictionJointDef::Initialize(b2Body* bA, b2Body* bB, const b2Vec2& anchor) {
    bodyA = bA;
    bodyB = bB;
    localAnchorA = bodyA->GetLocalPoint(anchor);
    localAnchorB = bodyB->GetLocalPoint(anchor);
}

b2FrictionJoint::b2FrictionJoint(const b2FrictionJointDef* def) : b2Joint(def) {
    m_localAnchorA = def->localAnchorA;
    m_localAnchorB = def->localAnchorB;

    m_linearImpulse.SetZero();
    m_angularImpulse = 0.0f;

    m_maxForce = def->maxForce;
    m_maxTorque = def->maxTorque;
}

void b2FrictionJoint::InitVelocityConstraints(const b2SolverData& data) {
    m_indexA = m_bodyA->m_islandIndex;
    m_indexB = m_bodyB->m_islandIndex;
    m_localCenterA = m_bodyA->m_sweep.localCenter;
    m_localCenterB = m_bodyB->m_sweep.localCenter;
    m_invMassA = m_bodyA->m_invMass;
    m_invMassB = m_bodyB->m_invMass;
    m_invIA = m_bodyA->m_invI;
    m_invIB = m_bodyB->m_invI;

    float aA = data.positions[m_indexA].a;
    b2Vec2 vA = data.velocities[m_indexA].v;
    float wA = data.velocities[m_indexA].w;

    float aB = data.positions[m_indexB].a;
    b2Vec2 vB = data.velocities[m_indexB].v;
    float wB = data.velocities[m_indexB].w;

    b2Rot qA(aA), qB(aB);

    // Compute the effective mass matrix.
    m_rA = b2Mul(qA, m_localAnchorA - m_localCenterA);
    m_rB = b2Mul(qB, m_localAnchorB - m_localCenterB);

    // J = [-I -r1_skew I r2_skew]
    //     [ 0       -1 0       1]
    // r_skew = [-ry; rx]

    // Matlab
    // K = [ mA+r1y^2*iA+mB+r2y^2*iB,  -r1y*iA*r1x-r2y*iB*r2x,          -r1y*iA-r2y*iB]
    //     [  -r1y*iA*r1x-r2y*iB*r2x, mA+r1x^2*iA+mB+r2x^2*iB,           r1x*iA+r2x*iB]
    //     [          -r1y*iA-r2y*iB,           r1x*iA+r2x*iB,                   iA+iB]

    float mA = m_invMassA, mB = m_invMassB;
    float iA = m_invIA, iB = m_invIB;

    b2Mat22 K;
    K.ex.x = mA + mB + iA * m_rA.y * m_rA.y + iB * m_rB.y * m_rB.y;
    K.ex.y = -iA * m_rA.x * m_rA.y - iB * m_rB.x * m_rB.y;
    K.ey.x = K.ex.y;
    K.ey.y = mA + mB + iA * m_rA.x * m_rA.x + iB * m_rB.x * m_rB.x;

    m_linearMass = K.GetInverse();

    m_angularMass = iA + iB;
    if (m_angularMass > 0.0f) {
        m_angularMass = 1.0f / m_angularMass;
    }

    if (data.step.warmStarting) {
        // Scale impulses to support a variable time step.
        m_linearImpulse *= data.step.dtRatio;
        m_angularImpulse *= data.step.dtRatio;

        b2Vec2 P(m_linearImpulse.x, m_linearImpulse.y);
        vA -= mA * P;
        wA -= iA * (b2Cross(m_rA, P) + m_angularImpulse);
        vB += mB * P;
        wB += iB * (b2Cross(m_rB, P) + m_angularImpulse);
    } else {
        m_linearImpulse.SetZero();
        m_angularImpulse = 0.0f;
    }

    data.velocities[m_indexA].v = vA;
    data.velocities[m_indexA].w = wA;
    data.velocities[m_indexB].v = vB;
    data.velocities[m_indexB].w = wB;
}

void b2FrictionJoint::SolveVelocityConstraints(const b2SolverData& data) {
    b2Vec2 vA = data.velocities[m_indexA].v;
    float wA = data.velocities[m_indexA].w;
    b2Vec2 vB = data.velocities[m_indexB].v;
    float wB = data.velocities[m_indexB].w;

    float mA = m_invMassA, mB = m_invMassB;
    float iA = m_invIA, iB = m_invIB;

    float h = data.step.dt;

    // Solve angular friction
    {
        float Cdot = wB - wA;
        float impulse = -m_angularMass * Cdot;

        float oldImpulse = m_angularImpulse;
        float maxImpulse = h * m_maxTorque;
        m_angularImpulse = b2Clamp(m_angularImpulse + impulse, -maxImpulse, maxImpulse);
        impulse = m_angularImpulse - oldImpulse;

        wA -= iA * impulse;
        wB += iB * impulse;
    }

    // Solve linear friction
    {
        b2Vec2 Cdot = vB + b2Cross(wB, m_rB) - vA - b2Cross(wA, m_rA);

        b2Vec2 impulse = -b2Mul(m_linearMass, Cdot);
        b2Vec2 oldImpulse = m_linearImpulse;
        m_linearImpulse += impulse;

        float maxImpulse = h * m_maxForce;

        if (m_linearImpulse.LengthSquared() > maxImpulse * maxImpulse) {
            m_linearImpulse.Normalize();
            m_linearImpulse *= maxImpulse;
        }

        impulse = m_linearImpulse - oldImpulse;

        vA -= mA * impulse;
        wA -= iA * b2Cross(m_rA, impulse);

        vB += mB * impulse;
        wB += iB * b2Cross(m_rB, impulse);
    }

    data.velocities[m_indexA].v = vA;
    data.velocities[m_indexA].w = wA;
    data.velocities[m_indexB].v = vB;
    data.velocities[m_indexB].w = wB;
}

bool b2FrictionJoint::SolvePositionConstraints(const b2SolverData& data) {
    B2_NOT_USED(data);

    return true;
}

b2Vec2 b2FrictionJoint::GetAnchorA() const { return m_bodyA->GetWorldPoint(m_localAnchorA); }

b2Vec2 b2FrictionJoint::GetAnchorB() const { return m_bodyB->GetWorldPoint(m_localAnchorB); }

b2Vec2 b2FrictionJoint::GetReactionForce(float inv_dt) const { return inv_dt * m_linearImpulse; }

float b2FrictionJoint::GetReactionTorque(float inv_dt) const { return inv_dt * m_angularImpulse; }

void b2FrictionJoint::SetMaxForce(float force) {
    b2Assert(b2IsValid(force) && force >= 0.0f);
    m_maxForce = force;
}

float b2FrictionJoint::GetMaxForce() const { return m_maxForce; }

void b2FrictionJoint::SetMaxTorque(float torque) {
    b2Assert(b2IsValid(torque) && torque >= 0.0f);
    m_maxTorque = torque;
}

float b2FrictionJoint::GetMaxTorque() const { return m_maxTorque; }

void b2FrictionJoint::Dump() {
    int32 indexA = m_bodyA->m_islandIndex;
    int32 indexB = m_bodyB->m_islandIndex;

    b2Dump("  b2FrictionJointDef jd;\n");
    b2Dump("  jd.bodyA = bodies[%d];\n", indexA);
    b2Dump("  jd.bodyB = bodies[%d];\n", indexB);
    b2Dump("  jd.collideConnected = bool(%d);\n", m_collideConnected);
    b2Dump("  jd.localAnchorA.Set(%.9g, %.9g);\n", m_localAnchorA.x, m_localAnchorA.y);
    b2Dump("  jd.localAnchorB.Set(%.9g, %.9g);\n", m_localAnchorB.x, m_localAnchorB.y);
    b2Dump("  jd.maxForce = %.9g;\n", m_maxForce);
    b2Dump("  jd.maxTorque = %.9g;\n", m_maxTorque);
    b2Dump("  joints[%d] = m_world->CreateJoint(&jd);\n", m_index);
}
