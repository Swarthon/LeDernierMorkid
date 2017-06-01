#include "Orc.h"

// Construction methodes
Orc::Orc(){
	mActual.life = 10;
	mActual.intelligence = 1;
	mActual.agility = 1;
	mActual.force = 1;
	mActual.precision = 1;
	mActual.speed = 7.5;
	mActual.range = 10;

	mTotal = mActual;

	mMemories = new Morkidios::Memories();
}
Orc::~Orc(){
}
void Orc::init(Ogre::SceneManager* smgr, btDynamicsWorld* world){
	static int num = 0;
	mName = std::string("Orc") + Morkidios::Utils::convertIntToString(num);
	initGraphics(mName,"sinbad.mesh",smgr);
	initCollisions(world);
	mEntity->setCastShadows(true);

	setUpAnimations();

	num++;
}

// Various methodes
void Orc::synchronize(){
	if(mSceneNode && mGhostObject && !mDead){
		Ogre::Quaternion oq = OgreBulletCollisions::BtOgreConverter::to(mGhostObject->getWorldTransform().getRotation());
		mSceneNode->setOrientation(oq);
		mSceneNode->setPosition(OgreBulletCollisions::BtOgreConverter::to(mGhostObject->getWorldTransform().getOrigin()));
	}
}

// AI stuff
void Orc::animation(const Morkidios::AnimationInformations* infos){
	synchronize();

	auto set = mEntity->getAllAnimationStates()->getAnimationStateIterator();
	for(auto it = set.begin(); it != set.end(); it++)
		it->second->addTime(infos->timeSinceLastFrame);
}
void Orc::navigation(Morkidios::NavigationAlgorithm* na){
//	na->searchForEnnemy(this);

	mMemories->oldPos.push_back(Ogre::Vector2(mSceneNode->getPosition().x,mSceneNode->getPosition().z));
	if(mTasks.empty())
		na->navigate(this);

	if(!mTasks.empty()){
		if(mTasks.front().action == Morkidios::Task::Moving || mTasks.front().action == Morkidios::Task::Chasing)
			goTo(mTasks.front().position);
		Ogre::Vector2 pos = Ogre::Vector2(mSceneNode->getPosition().x, mSceneNode->getPosition().z);
		if(pos.distance(mTasks.front().position) <= POSITION_PRECISION){
			mMemories->oldTasks.push_front(mTasks.front());
			mTasks.pop_front();
		}
	}

	if(mTasks.empty())		// After execute if no task : stop
		stop();

}
void Orc::interact(){
}

// Animation methodes
void Orc::setUpAnimations(){
	enableAnimation("RunTop", true);
	setLoopAnimation("RunTop", true);
	enableAnimation("RunBase", true);
	setLoopAnimation("RunBase", true);
}
void Orc::enableAnimation(std::string name, bool b){
	Ogre::AnimationState* a = mEntity->getAnimationState(name);
	a->setEnabled(true);
}
void Orc::setLoopAnimation(std::string name, bool b){
	Ogre::AnimationState* a = mEntity->getAnimationState(name);
	a->setLoop(true);
}
