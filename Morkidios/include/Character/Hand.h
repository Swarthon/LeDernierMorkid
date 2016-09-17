#ifndef HAND_H
#define HAND_H

// TinyXML includes
#ifndef TIXML_USE_STL
#define TIXML_USE_STL
#endif
#include <tinyxml.h>

// Ogre includes
#include <OGRE/Ogre.h>

// C++ includes
#include <string>
#include <iostream>

// My includes
#include "Global.h"
#include "Utils.h"

namespace Morkidios {
	class Hand {
	public:
		static Hand* load(std::string XMLFileName, Ogre::SceneManager* smgr);
		void setParent(Ogre::SceneNode* s);

		void setAnimation(std::string name, bool b);
		void addTime(double time);

		void setLeftHandObject(Ogre::Entity* sn);
		void setRightHandObject(Ogre::Entity* sn);
	private:
		// Constructeurs et Destructeurs privates
		Hand();
		~Hand();

		// Private methodes
		void setAttribute(TiXmlElement* elem);
		void setAnimation(TiXmlElement* elem);
		void setBone(TiXmlElement* elem);

		std::string mName;
		Ogre::Vector3 mOffset;
		Ogre::Vector3 mScale;
		double mPitch;
		double mRoll;
		double mYaw;
		double mTimeScale;
		Ogre::Entity* mEntity;
		Ogre::SceneNode* mSceneNode;
		Ogre::SceneManager* mSceneManager;

		Ogre::Entity* mLeftHand;
		Ogre::Entity* mRightHand;

		std::map<std::string,std::pair<std::string,bool> > mAnimations;
		std::vector<Ogre::AnimationState*> mStates;
		std::map<std::string,std::string> mBones;
	};
}

#endif // HAND_H