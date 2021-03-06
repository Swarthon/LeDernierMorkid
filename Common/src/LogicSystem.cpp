#include "LogicSystem.h"
#include "GameEntityManager.h"
#include "GameState.h"
#include "SdlInputHandler.h"

#include "Converter.h"

#include <BulletCollision/CollisionShapes/btCollisionShape.h>

#include <OgreConfigFile.h>
#include <OgreException.h>
#include <OgreRoot.h>

#include <OgreCamera.h>
#include <OgreRenderWindow.h>

#include <OgreArchiveManager.h>
#include <OgreHlmsManager.h>
#include <OgreHlmsPbs.h>
#include <OgreHlmsUnlit.h>

#include <Compositor/OgreCompositorManager2.h>

#include <SDL_syswm.h>

namespace Common {
	LogicSystem::LogicSystem(GameState* gameState)
	                : BaseSystem(gameState),
	                  mGraphicsSystem(0),
	                  mGameEntityManager(0),
	                  mCurrentTransformIdx(1),
	                  mWorld(NULL) {
		// mCurrentTransformIdx is 1, 0 and NUM_GAME_ENTITY_BUFFERS - 1 are taken by
		// GraphicsSytem at startup
		// The range to fill is then [2; NUM_GAME_ENTITY_BUFFERS-1]
		for (Ogre::uint32 i = 2; i < NUM_GAME_ENTITY_BUFFERS - 1; ++i)
			mAvailableTransformIdx.push_back(i);
	}
	//------------------------------------------------------------------------------------------------
	LogicSystem::~LogicSystem() {}
	//------------------------------------------------------------------------------------------------
	void LogicSystem::initialize() {
		mCollisionConfiguration = new btDefaultCollisionConfiguration();
		mDispatcher             = new btCollisionDispatcher(mCollisionConfiguration);
		mBroadphase             = new btDbvtBroadphase();
		mSolver                 = new btSequentialImpulseConstraintSolver();
		mWorld                  = new btDiscreteDynamicsWorld(
		        mDispatcher, mBroadphase, mSolver, mCollisionConfiguration);
		mWorld->setGravity(btVector3(0.0, -9.8, 0.0));
	}
	//------------------------------------------------------------------------------------------------
	void LogicSystem::update(float timeSinceLast) {
		if (mWorld)
			mWorld->stepSimulation(timeSinceLast, 1, 1.0 / 60.0);
		BaseSystem::update(timeSinceLast);
	}
	//------------------------------------------------------------------------------------------------
	void LogicSystem::finishFrameParallel(void) {
		if (mGameEntityManager)
			mGameEntityManager->finishFrameParallel();

		// Notify the GraphicsSystem we're done rendering this frame.
		if (mGraphicsSystem) {
			size_t idxToSend = mCurrentTransformIdx;

			if (mAvailableTransformIdx.empty()) {
				// Don't relinquish our only ID left.
				// If you end up here too often, Graphics' thread is too slow,
				// or you need to increase NUM_GAME_ENTITY_BUFFERS
				idxToSend = std::numeric_limits<Ogre::uint32>::max();
			}
			else {
				// Until Graphics constantly releases the indices we send them, to
				// avoid writing
				// to transform data that may be in use by the other thread (race
				// condition)
				mCurrentTransformIdx = mAvailableTransformIdx.front();
				mAvailableTransformIdx.pop_front();
			}

			this->queueSendMessage(mGraphicsSystem, Mq::LOGICFRAME_FINISHED, idxToSend);
		}

		BaseSystem::finishFrameParallel();
	}
	//------------------------------------------------------------------------------------------------
	void LogicSystem::processIncomingMessage(Mq::MessageId messageId, const void* data) {
		BaseSystem::processIncomingMessage(messageId, data);
		switch (messageId) {
		case Mq::LOGICFRAME_FINISHED: {
			Ogre::uint32 newIdx = *reinterpret_cast<const Ogre::uint32*>(data);
			assert((mAvailableTransformIdx.empty() || newIdx == (mAvailableTransformIdx.back() + 1) % NUM_GAME_ENTITY_BUFFERS) && "Indices are arriving out of order!!!");

			mAvailableTransformIdx.push_back(newIdx);
		} break;
		case Mq::GAME_ENTITY_SCHEDULED_FOR_REMOVAL_SLOT:
			mGameEntityManager->_notifyGameEntitiesRemoved(
			        *reinterpret_cast<const Ogre::uint32*>(data));
			break;
		case Mq::SDL_EVENT: break;
		default: break;
		}
	}
	//------------------------------------------------------------------------------------------------
	void LogicSystem::addGameEntity(const GameEntityManager::CreatedGameEntity* cge) {
		if (cge->gameEntity->mCoDefinition->coType == CoRigidBody) {
			btVector3 localInertia(0, 0, 0);
			if (cge->gameEntity->mCoDefinition->mass)
				cge->gameEntity->mCoDefinition->shape->calculateLocalInertia(cge->gameEntity->mCoDefinition->mass,
				                                                             localInertia);
			cge->gameEntity->mCollisionObject = new btRigidBody(cge->gameEntity->mCoDefinition->mass,
			                                                    new btDefaultMotionState(),
			                                                    cge->gameEntity->mCoDefinition->shape,
			                                                    localInertia);
			cge->gameEntity->mCollisionObject->setWorldTransform(Collision::Converter::to(cge->initialTransform));
			cge->gameEntity->mCollisionObject->setRestitution(cge->gameEntity->mCoDefinition->restitution);
			cge->gameEntity->mCollisionObject->setFriction(cge->gameEntity->mCoDefinition->friction);
			mWorld->addRigidBody(
			        static_cast<btRigidBody*>(cge->gameEntity->mCollisionObject));
		}
		else {
			OGRE_EXCEPT(Ogre::Exception::ERR_NOT_IMPLEMENTED,
			            "This functionality has to be implemented",
			            "LogicSystem::addGameEntity");
		}
	}
}
