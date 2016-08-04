#ifndef STATE_H
#define STATE_H

#include "Framework.h"

namespace Morkidios {

	class State;

	class StateListener
	{
	public:
		StateListener(){};
		virtual ~StateListener(){};

		virtual void manageState(Ogre::String stateName, State* state) = 0;

		virtual State*	findByName(Ogre::String stateName) = 0;
		virtual void		changeState(State *state) = 0;
		virtual bool		pushState(State* state) = 0;
		virtual void		popState() = 0;
		virtual void		pauseState() = 0;
		virtual void		shutdown() = 0;
		virtual void		popAllAndPushState(State* state) = 0;
	};


	class State : public OIS::KeyListener, public OIS::MouseListener
	{
	public:
		static void create(StateListener* parent, const Ogre::String name){};
	
		void destroy(){delete this;}
	
		virtual void enter() = 0;
		virtual void exit() = 0;
		virtual bool pause(){return true;}
		virtual void resume(){};
		virtual void update(double timeSinceLastFrame) = 0;
	
	protected:
		State(){};

		State*	findByName(Ogre::String stateName){return mParent->findByName(stateName);}
		void		changeState(State* state){mParent->changeState(state);}
		bool		pushState(State* state){return mParent->pushState(state);}
		void		popState(){mParent->popState();}
		void		shutdown(){mParent->shutdown();}
		void		popAllAndPushState(State* state){mParent->popAllAndPushState(state);}
		
		StateListener*			mParent;

		Ogre::SceneManager*			mSceneManager;
		Ogre::FrameEvent			mFrameEvent;
	};

}

#define DECLARE_APPSTATE_CLASS(T)										\
static void create(Morkidios::StateListener* parent, const Ogre::String name)	\
{																		\
	T* myState = new T();											\
	myState->mParent = parent;										\
	parent->manageState(name, myState);							\
}

#endif
