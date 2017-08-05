#ifndef OBJECTSTATE_H_
#define OBJECTSTATE_H_

#include <LinearMath/btMotionState.h>
#include <LinearMath/btTransform.h>

namespace Common {
	class GameEntity;
	class LogicSystem;
}

namespace Collision {

	/**
	 * @class ObjectState
	 * @brief Class defines the way Collision and Graphics are synchronized
	 *
	 * ObjectState derives from btMotionState and overloads the functions to synchronize Ogre::SceneNode and btRigidBody from a Common::GameEntity.
	 * To use it, do as follow
	 * @code
	 *	rigidBody->setMotionState(new Collision::ObjectState(transform, gameEntity, logicSystem));
	 * @endcode
	 */
	class ObjectState : public btMotionState {
	public:
		/**
		 * Constructor of CollisionCameraController
		 * @param initial the initial transform of the Object
		 * @param gameEntity the Common::GameEntity to synchronize
		 * @param logicSystem the LogicSystem to which belongs the gameEntity, used to get the transform of the Common::GameEntity
		 */
		ObjectState(btTransform& initial, Common::GameEntity* gameEntity, const Common::LogicSystem* logicSystem);
		/**
		 * Simple destructor
		 */
		virtual ~ObjectState();

		/**
		 * Method to get the btTransform of the Object
		 * @param worldTrans the btTransform to define with the internal btTransform
		 * @remarks This method is used internaly by Bullet. Do not use it
		 */
		virtual void getWorldTransform(btTransform& worldTrans) const;
		/**
		 * Method to get the btTransform of the Object
		 * @param worldTrans the btTransform used to set the internal btTransform
		 * @remarks This method is used internaly by Bullet. Do not use it
		 */
		virtual void setWorldTransform(const btTransform& worldTrans);

	private:
		/// Common::GameEntity to synchronize
		Common::GameEntity*        mGameEntity;
		/// Common::LogicSystem to which belongs mGameEntity
		const Common::LogicSystem* mLogicSystem;
		/// btTransform value
		btTransform                mWorldTransform;
	};
}

#endif // OBJECTSTATE_H_
