#include "AI.h"

namespace Morkidios {

	// Navigation methodes
	void AI::goTo(Ogre::Vector2 v){
		Ogre::Vector3 pos = Ogre::Vector3(mSceneNode->getPosition().x, 0, mSceneNode->getPosition().z);
		Ogre::Vector3 newPos = Ogre::Vector3(v.x, 0, v.y);

		if(pos.distance(newPos) < mStepHeight){
			mGhostObject->getWorldTransform().setOrigin(btVector3(newPos.x, mGhostObject->getWorldTransform().getOrigin().y(), newPos.z));
			stop();
			return;
		}

		Ogre::Vector3 direction = newPos - pos;
		direction.normalise();

		btTransform ghostTransform = mGhostObject->getWorldTransform();
		btVector3 forwardBullet = ghostTransform.getBasis()[2];
		forwardBullet[0] *= -1;
		forwardBullet.normalize();
		Ogre::Vector3 forward = OgreBulletCollisions::BtOgreConverter::to(forwardBullet);
		Ogre::Quaternion q = Ogre::Quaternion(forward.getRotationTo(direction).getYaw(), Ogre::Vector3(0,1,0)) * Ogre::Quaternion(forward.getRotationTo(direction).getPitch(), Ogre::Vector3(0,1,0));
		mGhostObject->getWorldTransform().setRotation(mGhostObject->getWorldTransform().getRotation() * OgreBulletCollisions::OgreBtConverter::to(q));
		mCharacter->setWalkDirection(OgreBulletCollisions::OgreBtConverter::to(direction) * 0.3);
	}
	void AI::stop(){
		mCharacter->setWalkDirection(btVector3(0,0,0));
	}

	// Return value methodes
	std::string AI::getName(){
		return mName;
	}
	Memories* AI::getMemories(){
		return mMemories;
	}
	std::deque<Task>* AI::getTasks(){
		return &mTasks;
	}
	std::vector<Character*> AI::getEnemy(){
		return mEnemy;
	}

	// Various methodes
	void AI::setDead(){
		mWorld->removeCollisionObject(mGhostObject);
		mWorld->removeAction(mCharacter);
		mSceneNode->setVisible(false);
		mDead = true;
	}

	// Construction methodes
	void AI::addEnemy(Character* c){
		mEnemy.push_back(c);
	}
}
