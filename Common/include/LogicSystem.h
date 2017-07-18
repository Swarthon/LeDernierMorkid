#ifndef _LOGICSYSTEM_H_
#define _LOGICSYSTEM_H_

#include "BaseSystem.h"
#include "GameEntityManager.h"

#include <OgrePrerequisites.h>

#include <BulletCollision/BroadphaseCollision/btDbvtBroadphase.h>
#include <BulletCollision/CollisionDispatch/btCollisionConfiguration.h>
#include <BulletCollision/CollisionDispatch/btDefaultCollisionConfiguration.h>
#include <BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.h>
#include <BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>
#include <LinearMath/btDefaultMotionState.h>

namespace Common {
	class LogicSystem : public BaseSystem {
	public:	BaseSystem*        mGraphicsSystem;
	protected:
		GameEntityManager* mGameEntityManager;

		// Collsion
		btDynamicsWorld*                     mWorld;
		btDefaultCollisionConfiguration*     mCollisionConfiguration;
		btCollisionDispatcher*               mDispatcher;
		btDbvtBroadphase*                    mBroadphase;
		btSequentialImpulseConstraintSolver* mSolver;

		Ogre::uint32             mCurrentTransformIdx;
		std::deque<Ogre::uint32> mAvailableTransformIdx;

		virtual void processIncomingMessage(Mq::MessageId messageId, const void* data);

	public:
		LogicSystem(GameState* gameState);
		virtual ~LogicSystem();

		void initialize();

		void _notifyGraphicsSystem(BaseSystem* graphicsSystem) {
			mGraphicsSystem = graphicsSystem;
		}
		void _notifyGameEntityManager(GameEntityManager* mgr) { mGameEntityManager = mgr; }

		void update(float timeSinceLast);
		void finishFrameParallel(void);

		GameEntityManager* getGameEntityManager(void) { return mGameEntityManager; }
		Ogre::uint32       getCurrentTransformIdx(void) const { return mCurrentTransformIdx; }
		btDynamicsWorld*   getWorld(void) const { return mWorld; }
		void addGameEntity(const GameEntityManager::CreatedGameEntity* cge);
	};
}

#endif // _LOGICSYSTEM_H_
