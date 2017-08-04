#include "CollisionCameraController.h"

#include "Converter.h"

#include <OgreCamera.h>

#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <BulletCollision/CollisionShapes/btCapsuleShape.h>
#include <BulletDynamics/Character/btKinematicCharacterController.h>
#include <BulletDynamics/Dynamics/btDynamicsWorld.h>

namespace Collision {
	CollisionCameraController::CollisionCameraController(Common::GraphicsSystem* graphicsSystem, btDynamicsWorld* world)
	                : mWorld(world),
	                  CameraController(graphicsSystem, true) {
		mGhostObject = new btPairCachingGhostObject();
		mWorld->getBroadphase()->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
		btConvexShape* shape;
		shape = new btCapsuleShape(1, 1.80);
		mGhostObject->setCollisionShape(shape);
		mGhostObject->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);
		mGhostObject->getWorldTransform().setOrigin(Converter::to(mGraphicsSystem->getCamera()->getPosition()));

		mWorld->addCollisionObject(mGhostObject);
		mCharacter = new btKinematicCharacterController(mGhostObject, shape, 0.75);

		mWorld->addAction(mCharacter);
	}
	//-----------------------------------------------------------------------------------------
	void CollisionCameraController::update(float timeSinceLast) {
		Ogre::Camera* camera     = mGraphicsSystem->getCamera();
		Ogre::Node*   cameraNode = camera->getParentNode();

		{
			btTransform  transform = mGhostObject->getWorldTransform();
			btQuaternion q         = transform.getRotation();
			q *= btQuaternion(btVector3(0, 1, 0), mCameraYaw);
			mGhostObject->getWorldTransform().setRotation(q);
		}

		camera->pitch(Ogre::Radian(mCameraPitch));

		mCameraYaw   = 0.0f;
		mCameraPitch = 0.0f;

		int camMovementZ = mKeymapState[Backward].second - mKeymapState[Forward].second;
		int camMovementX = mKeymapState[Rightward].second - mKeymapState[Leftward].second;
		int slideUpDown  = mKeymapState[Up].second - mKeymapState[Down].second;

		Ogre::Vector3 camMovementDir(camMovementX, slideUpDown, camMovementZ);
		camMovementDir.normalise();
		camMovementDir *= timeSinceLast * mCameraBaseSpeed * (1 + mSpeedModifier * mCameraSpeedBoost);
		if (camMovementDir.y > 0) {
			mCharacter->jump();
			camMovementDir.y = 0;
		}
		mCharacter->setWalkDirection(Collision::Converter::to(Collision::Converter::to(mGhostObject->getWorldTransform().getRotation()) * camMovementDir));

		{
			Ogre::Node*      cameraNode = camera->getParentNode();
			Ogre::Quaternion q;
			q = q * (Collision::Converter::to(mGhostObject->getWorldTransform().getRotation()));
			q = q * Ogre::Quaternion(cameraNode->getOrientation().getPitch(), Ogre::Vector3(1, 0, 0));

			cameraNode->setOrientation(q);
			cameraNode->setPosition(Collision::Converter::to(mGhostObject->getWorldTransform().getOrigin() /* + btVector3(0,1.8,0)*/));
		}
	}
}
