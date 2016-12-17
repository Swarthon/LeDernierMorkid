#include "StateManager.h"

#include <OgreWindowEventUtilities.h>

std::vector<std::pair<std::string,void(*)(Morkidios::StateListener*,const Ogre::String)>> Morkidios::StateManager::mToLoadStates = std::vector<std::pair<std::string,void(*)(Morkidios::StateListener*,const Ogre::String)>>();

namespace Morkidios {

	StateManager::StateManager()
	{
		mbShutdown = false;
		Framework::getSingletonPtr()->mRoot->addFrameListener(this);
	}

	StateManager::~StateManager()
	{
		state_info si;

		while(!mActiveStateStack.empty())
		{
			mActiveStateStack.back()->exit();
			mActiveStateStack.pop_back();
		}

		while(!mStates.empty())
		{
			si = mStates.back();
		        si.state->destroy();
		        mStates.pop_back();
		}
	}

	void StateManager::manageState(Ogre::String stateName, State* state)
	{
		try
		{
			state_info new_state_info;
			new_state_info.name = stateName;
			new_state_info.state = state;
			mStates.push_back(new_state_info);
		}
		catch(std::exception& e)
		{
			delete state;
			throw Ogre::Exception(Ogre::Exception::ERR_INTERNAL_ERROR, "Error while trying to manage a new State\n" + Ogre::String(e.what()), "StateManager.cpp (39)");
		}
	}

	State* StateManager::findByName(Ogre::String stateName)
	{
		std::vector<state_info>::iterator itr;

		for(itr=mStates.begin();itr!=mStates.end();itr++)
		{
			if(itr->name==stateName)
				return itr->state;
		}

		return 0;
	}

	void StateManager::start(State* state)
	{
		changeState(state);

		while(!mbShutdown)
		{
			if(Framework::getSingletonPtr()->mRenderWindow->isClosed())mbShutdown = true;

			Ogre::WindowEventUtilities::messagePump();

			if(Framework::getSingletonPtr()->mRenderWindow->isActive())
				Framework::getSingletonPtr()->mRoot->renderOneFrame();
			else
			{
		#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
				Sleep(1000);
		#else
				sleep(1);
		#endif
			}
		}

		Framework::getSingletonPtr()->mLog->logMessage("Main loop quit");
	}
	void StateManager::changeState(State* state)
	{
		if(!mActiveStateStack.empty())
		{
			mActiveStateStack.back()->exit();
			mActiveStateStack.pop_back();
		}

		mActiveStateStack.push_back(state);
		init(state);
		mActiveStateStack.back()->enter();
	}
	bool StateManager::pushState(State* state)
	{
		if(!mActiveStateStack.empty())
		{
			if(!mActiveStateStack.back()->pause())
				return false;
		}

		mActiveStateStack.push_back(state);
		init(state);
		mActiveStateStack.back()->enter();

		return true;
	}
	void StateManager::popState()
	{
		if(!mActiveStateStack.empty())
		{
			mActiveStateStack.back()->exit();
			mActiveStateStack.pop_back();
		}

		if(!mActiveStateStack.empty())
		{
			init(mActiveStateStack.back());
			mActiveStateStack.back()->resume();
		}
		else
			shutdown();
	}
	void StateManager::popAllAndPushState(State* state)
	{
		while(!mActiveStateStack.empty())
		{
			mActiveStateStack.back()->exit();
			mActiveStateStack.pop_back();
		}
		pushState(state);
	}
	void StateManager::pauseState()
	{
		if(!mActiveStateStack.empty())
		{
			mActiveStateStack.back()->pause();
		}

		if(mActiveStateStack.size() > 2)
		{
			init(mActiveStateStack.at(mActiveStateStack.size() - 2));
			mActiveStateStack.at(mActiveStateStack.size() - 2)->resume();
		}
	}
	void StateManager::shutdown()
	{
		mbShutdown = true;
	}

	void StateManager::loadAllStates(StateManager* sm){
		for (size_t i = 0; i < mToLoadStates.size(); i++){
			mToLoadStates[i].second(sm, mToLoadStates[i].first);
		}
	}
	void StateManager::addToLoadState(std::string name, void(*createMethod)(StateListener*,const Ogre::String)){
		mToLoadStates.push_back(std::pair<std::string,void(*)(StateListener*,const Ogre::String)>(name,createMethod));
	}


	void StateManager::init(State* state)
	{
		Framework::getSingletonPtr()->mInput->mKeyboard->setEventCallback(state);
		Framework::getSingletonPtr()->mInput->mMouse->setEventCallback(state);

		Framework::getSingletonPtr()->mRenderWindow->resetStatistics();
	}
	bool StateManager::frameRenderingQueued(const Ogre::FrameEvent& evt){
		Framework::getSingletonPtr()->mInput->mKeyboard->capture();
		Framework::getSingletonPtr()->mInput->mMouse->capture();

		mActiveStateStack.back()->update((float)(evt.timeSinceLastFrame));

		Framework::getSingletonPtr()->updateOgre((float)(evt.timeSinceLastFrame));

		return true;
	}
}
