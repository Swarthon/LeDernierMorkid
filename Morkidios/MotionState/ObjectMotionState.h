#ifndef OBJECTMOTIONSTATE
#define OBJECTMOTIONSTATE

// Bullet includes
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <BulletDynamics/Character/btKinematicCharacterController.h>

// My includes
#include "Utils.h"

namespace Morkidios {

	class ObjectMotionState : public btMotionState {
	public:
		ObjectMotionState(btTransform& i, Ogre::SceneNode* sn){
			mSceneNode = sn;
			mInitialPosition = i;
		}
		virtual ~ObjectMotionState(){
		}
	
		virtual void getWorldTransform(btTransform& worldTrans) const {
			worldTrans = mInitialPosition;
		}
		virtual void setWorldTransform(const btTransform& worldTrans) {
			if(mSceneNode == NULL)
				return;

			mSceneNode->setPosition(OgreBulletCollisions::BtOgreConverter::to(worldTrans.getOrigin()));
			mSceneNode->setOrientation(OgreBulletCollisions::BtOgreConverter::to(worldTrans.getRotation()));
		}
	private:
		Ogre::SceneNode* mSceneNode;
		btTransform mInitialPosition;
	};

}

#endif // OBJECTMOTIONSTATE