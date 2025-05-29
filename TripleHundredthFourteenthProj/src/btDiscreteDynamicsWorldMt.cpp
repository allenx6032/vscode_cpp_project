/*
Bullet Continuous Collision Detection and Physics Library
Copyright (c) 2003-2009 Erwin Coumans  http://bulletphysics.org

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it freely,
subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/


#include "btDiscreteDynamicsWorldMt.h"

//collision detection
#include "btCollisionDispatcher.h"
#include "btSimpleBroadphase.h"
#include "btCollisionAlgorithm.h"
#include "btCollisionShape.h"
#include "btSimulationIslandManagerMt.h"
#include "btTransformUtil.h"
#include "btQuickprof.h"

//rigidbody & constraints
#include "btRigidBody.h"
#include "btSequentialImpulseConstraintSolver.h"
#include "btContactSolverInfo.h"
#include "btTypedConstraint.h"
#include "btPoint2PointConstraint.h"
#include "btHingeConstraint.h"
#include "btConeTwistConstraint.h"
#include "btGeneric6DofConstraint.h"
#include "btGeneric6DofSpring2Constraint.h"
#include "btSliderConstraint.h"
#include "btContactConstraint.h"


#include "btIDebugDraw.h"
#include "btSphereShape.h"


#include "btActionInterface.h"
#include "btQuickprof.h"
#include "btMotionState.h"

#include "btSerializer.h"


struct InplaceSolverIslandCallbackMt : public btSimulationIslandManagerMt::IslandCallback
{
	btContactSolverInfo*	m_solverInfo;
	btConstraintSolver*		m_solver;
	btIDebugDraw*			m_debugDrawer;
	btDispatcher*			m_dispatcher;

	InplaceSolverIslandCallbackMt(
		btConstraintSolver*	solver,
		btStackAlloc* stackAlloc,
		btDispatcher* dispatcher)
		:m_solverInfo(NULL),
		m_solver(solver),
		m_debugDrawer(NULL),
		m_dispatcher(dispatcher)
	{

	}

	InplaceSolverIslandCallbackMt& operator=(InplaceSolverIslandCallbackMt& other)
	{
		btAssert(0);
		(void)other;
		return *this;
	}

	SIMD_FORCE_INLINE void setup ( btContactSolverInfo* solverInfo, btIDebugDraw* debugDrawer)
	{
		btAssert(solverInfo);
		m_solverInfo = solverInfo;
		m_debugDrawer = debugDrawer;
	}


	virtual	void	processIsland( btCollisionObject** bodies,
                                   int numBodies,
                                   btPersistentManifold** manifolds,
                                   int numManifolds,
                                   btTypedConstraint** constraints,
                                   int numConstraints,
                                   int islandId
                                   )
	{
        m_solver->solveGroup( bodies,
                              numBodies,
                              manifolds,
                              numManifolds,
                              constraints,
                              numConstraints,
                              *m_solverInfo,
                              m_debugDrawer,
                              m_dispatcher
                              );
    }

};



btDiscreteDynamicsWorldMt::btDiscreteDynamicsWorldMt(btDispatcher* dispatcher,btBroadphaseInterface* pairCache,btConstraintSolver* constraintSolver, btCollisionConfiguration* collisionConfiguration)
: btDiscreteDynamicsWorld(dispatcher,pairCache,constraintSolver,collisionConfiguration)
{
	if (m_ownsIslandManager)
	{
		m_islandManager->~btSimulationIslandManager();
		btAlignedFree( m_islandManager);
	}
    {
		void* mem = btAlignedAlloc(sizeof(InplaceSolverIslandCallbackMt),16);
		m_solverIslandCallbackMt = new (mem) InplaceSolverIslandCallbackMt (m_constraintSolver, 0, dispatcher);
    }
	{
		void* mem = btAlignedAlloc(sizeof(btSimulationIslandManagerMt),16);
		btSimulationIslandManagerMt* im = new (mem) btSimulationIslandManagerMt();
        m_islandManager = im;
        im->setMinimumSolverBatchSize( m_solverInfo.m_minimumSolverBatchSize );
	}
}


btDiscreteDynamicsWorldMt::~btDiscreteDynamicsWorldMt()
{
	if (m_solverIslandCallbackMt)
	{
		m_solverIslandCallbackMt->~InplaceSolverIslandCallbackMt();
		btAlignedFree(m_solverIslandCallbackMt);
	}
	if (m_ownsConstraintSolver)
	{
		m_constraintSolver->~btConstraintSolver();
		btAlignedFree(m_constraintSolver);
	}
}


void	btDiscreteDynamicsWorldMt::solveConstraints(btContactSolverInfo& solverInfo)
{
	BT_PROFILE("solveConstraints");

	m_solverIslandCallbackMt->setup(&solverInfo, getDebugDrawer());
	m_constraintSolver->prepareSolve(getCollisionWorld()->getNumCollisionObjects(), getCollisionWorld()->getDispatcher()->getNumManifolds());

	/// solve all the constraints for this island
    btSimulationIslandManagerMt* im = static_cast<btSimulationIslandManagerMt*>(m_islandManager);
    im->buildAndProcessIslands( getCollisionWorld()->getDispatcher(), getCollisionWorld(), m_constraints, m_solverIslandCallbackMt );

	m_constraintSolver->allSolved(solverInfo, m_debugDrawer);
}


