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

	/**
	 * @class LogicSystem
	 * @brief
	 *	LogicSystem managing the logics (Collisions ...)
	 * @ingroup Common
	 */
	class LogicSystem : public BaseSystem {
	public:
		/// LogicSystem to which communicate
		BaseSystem* mGraphicsSystem;

	protected:
		/// GameEntityManager to manage entities
		GameEntityManager* mGameEntityManager;

		/// Bullet Collision World
		btDynamicsWorld*                     mWorld;
		/// Collision Configuration
		btDefaultCollisionConfiguration*     mCollisionConfiguration;
		/// Collision Dispatcher
		btCollisionDispatcher*               mDispatcher;
		/// Collision Broadphase
		btDbvtBroadphase*                    mBroadphase;
		/// Collision Constraint Solver
		btSequentialImpulseConstraintSolver* mSolver;

		/// Current transform id
		Ogre::uint32             mCurrentTransformIdx;
		/// Available transform id
		std::deque<Ogre::uint32> mAvailableTransformIdx;

		/**
		 * Process received messages
		 * @param messageId
 		 *	The Mq::MessageId of the message, used to know what kind of message it is
		 * @param data
 		 *	The data sent with the message, could be anything. Depending on the messageId, a conversion will be needed
		 * @see
 		 *	Common::Mq::MessageQueueSystem::processIncomingMessage
		 */
		virtual void processIncomingMessage(Mq::MessageId messageId, const void* data);

	public:
		/**
		 * Constructor
		 * @param gameState
 		 *	GameState to which transmit the events
		 */
		LogicSystem(GameState* gameState);
		/// Simple Destructor
		virtual ~LogicSystem();

		/// Initialize the system
		void initialize();

		/**
		 * Set the GraphicsSystem to which send the notifications
		 * @param graphicsSystem
		 *	GraphicsSystem to set
		 */
		void _notifyGraphicsSystem(BaseSystem* graphicsSystem) { mGraphicsSystem = graphicsSystem; }
		/**
		 * Set the GameEntityManager through which create the entities
		 * @param mgr
		 *	GameEntityManager to set
		 */
		void _notifyGameEntityManager(GameEntityManager* mgr) { mGameEntityManager = mgr; }

		/**
		 * Update the system. To call every frame
		 * @param timeSinceLast
		 *	Time past since last frame
		 */
		void update(float timeSinceLast);
		/// Method to call at the end of the frame
		void finishFrameParallel(void);

		/// Get mGameEntityManager
		GameEntityManager* getGameEntityManager(void) { return mGameEntityManager; }
		/// Get mCurrentTransformIdx
		Ogre::uint32       getCurrentTransformIdx(void) const { return mCurrentTransformIdx; }
		/// Get mWorld
		btDynamicsWorld*   getWorld(void) const { return mWorld; }
		/**
		 * Adding collision to GameEntity
		 * @param cge
		 *	The created GameEntity that need to be completed
		 */
		void addGameEntity(const GameEntityManager::CreatedGameEntity* cge);
	};
}

#endif // _LOGICSYSTEM_H_
