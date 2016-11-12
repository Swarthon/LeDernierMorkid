#include "Hand.h"

namespace Morkidios {
	Hand* Hand::load(std::string XMLFileName, Ogre::SceneManager* smgr){
		TiXmlDocument doc(XMLFileName);
		if(!doc.LoadFile()){
			std::cerr << "Error while loading file : " << XMLFileName << std::endl;
			std::cerr << "error #" << doc.ErrorId() << " : " << doc.ErrorDesc() << std::endl;
			return NULL;
		}

		Hand* h = new Hand();
		h->mSceneManager = smgr;

		// Load all the properties
		TiXmlHandle hdl(&doc);
		TiXmlElement *name = hdl.FirstChildElement().Element();
		name->QueryStringAttribute("name", &h->mName);

		TiXmlHandle hdl2(&doc);
		for(TiXmlElement *elem = hdl2.FirstChildElement().FirstChildElement("Property").Element(); elem; elem = elem->NextSiblingElement())
			h->setAttribute(elem);

		TiXmlHandle hdl3(&doc);
		for(TiXmlElement *elem = hdl3.FirstChildElement().FirstChildElement("Property").Element(); elem; elem = elem->NextSiblingElement())
			h->setAnimation(elem);


		TiXmlHandle hdl4(&doc);
		for(TiXmlElement *elem = hdl4.FirstChildElement().FirstChildElement("Property").Element(); elem; elem = elem->NextSiblingElement())
			h->setBone(elem);

		return h;
	}
	void Hand::setParent(Ogre::SceneNode* s){
		if(!mEntity){
			std::cerr << "Entity has not been initialized for Hand in setParent\nPlease use load with a mesh name in the .xml file\n";
			exit(1);
		}
		if(mSceneNode){
			mSceneNode->getParent()->removeChild(mSceneNode);
			mSceneNode->detachAllObjects();
		}
		mSceneNode = s->createChildSceneNode(mEntity->getName());
		mSceneNode->attachObject(mEntity);

		mSceneNode->scale(mScale);
		mSceneNode->pitch(Ogre::Degree(mPitch));
		mSceneNode->yaw(Ogre::Degree(mYaw), Ogre::SceneNode::TS_LOCAL);
		mSceneNode->roll(Ogre::Degree(mRoll));
		mSceneNode->translate(mOffset, Ogre::SceneNode::TS_PARENT);
	}

	void Hand::setAnimation(std::string name, bool b){
		if(mAnimations[name].first == std::string()){
			std::cout << "WARNING : Trying to access to animation " << name << " but it doesn't exist\n";
			return;
		}

		Ogre::AnimationState* state = mEntity->getAnimationState(mAnimations[name].first);
		state->setLoop(mAnimations[name].second);
		state->setEnabled(b);

		std::vector<Ogre::AnimationState*>::iterator it;
		it = std::find(mStates.begin(), mStates.end(), state);
		if(it == mStates.end())
			mStates.push_back(state);
	}
	void Hand::addTime(double time){
		for(int i = 0; i < mStates.size(); i++)
			if(mStates[i]->getEnabled())
				mStates[i]->addTime(time * mTimeScale);
	}

	void Hand::setLeftHandObject(Ogre::Entity* ent){
		if(mBones["LeftHand"] == std::string()){
			std::cout << "WARNING : Trying to set an object to LeftHand bone but this one itsn't set\n";
			return;
		}

		if(ent){
			if(mLeftHand)
				mEntity->detachObjectFromBone(mLeftHand);
			if(ent->getParentSceneNode())
				ent->detachFromParent();
			mEntity->attachObjectToBone(mBones["LeftHand"], ent);

		}
		else		// if ent is NULL just clear the hand
			if(mLeftHand)
				mEntity->detachObjectFromBone(mLeftHand);

		mLeftHand = ent;
	}
	void Hand::setRightHandObject(Ogre::Entity* ent){
		if(mBones["RightHand"] == std::string()){
			std::cout << "WARNING : Trying to set an object to RightHand bone but this one itsn't set\n";
			return;
		}

		if(ent){
			if(mRightHand)
				mEntity->detachObjectFromBone(mRightHand);
			if(ent->getParentSceneNode())
				ent->detachFromParent();
			mEntity->attachObjectToBone(mBones["RightHand"], ent);
		}
		else		// if ent is NULL just cler the hand
			if(mRightHand)
				mEntity->detachObjectFromBone(mRightHand);
		mRightHand = ent;
	}

	// Constructeurs et Destructeurs privates
	Hand::Hand(){
		mOffset = Ogre::Vector3::ZERO;
		mScale = Ogre::Vector3::ZERO;
		mPitch = 0;
		mYaw = 0;
		mRoll = 0;
		mTimeScale = 1;

		mEntity = NULL;
		mSceneNode = NULL;

		mLeftHand = NULL;
		mRightHand = NULL;
	}
	Hand::~Hand(){
	}

	// Private methodes
	void Hand::setAttribute(TiXmlElement* elem){
		if(elem->Attribute("name")){
			if(std::string(elem->Attribute("name")) == std::string("offset")){
				double x,y,z;
				elem->QueryDoubleAttribute("x", &x);
				elem->QueryDoubleAttribute("y", &y);
				elem->QueryDoubleAttribute("z", &z);
				mOffset = Ogre::Vector3(x,y,z);
			}
			if(std::string(elem->Attribute("name")) == std::string("scale")){
				double x,y,z;
				elem->QueryDoubleAttribute("x", &x);
				elem->QueryDoubleAttribute("y", &y);
				elem->QueryDoubleAttribute("z", &z);
				mScale = Ogre::Vector3(x,y,z);
			}
			if(std::string(elem->Attribute("name")) == std::string("pitch"))
				elem->QueryDoubleAttribute("value", &mPitch);
			if(std::string(elem->Attribute("name")) == std::string("yaw"))
				elem->QueryDoubleAttribute("value", &mYaw);
			if(std::string(elem->Attribute("name")) == std::string("roll"))
				elem->QueryDoubleAttribute("value", &mRoll);
			if(std::string(elem->Attribute("name")) == std::string("timeScale"))
				elem->QueryDoubleAttribute("value", &mTimeScale);
			if(std::string(elem->Attribute("name")) == std::string("mesh")){
				std::string meshName;
				elem->QueryStringAttribute("value", &meshName);
				if(mName != std::string())
					mEntity = mSceneManager->createEntity(mName.c_str(),meshName.c_str());
				else {
					static int num = 0;
					std::ostringstream os;
					os << num;
					mEntity = mSceneManager->createEntity(os.str().c_str(),meshName.c_str());
					num++;
				}
			}
		}
	}
	void Hand::setAnimation(TiXmlElement* elem){
		if(elem->Attribute("name") && elem->Attribute("meshName")){
			bool loop;
			if(elem->Attribute("loop"))
				elem->QueryBoolAttribute("loop", &loop);
			mAnimations[elem->Attribute("name")] = std::pair<std::string,bool>(elem->Attribute("meshName"),loop);
		}
	}
	void Hand::setBone(TiXmlElement* elem){
		if(elem->Attribute("name") && elem->Attribute("boneName"))
			mBones[elem->Attribute("name")] = std::string(elem->Attribute("boneName"));
	}

}
