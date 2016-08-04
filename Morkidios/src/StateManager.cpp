#include "StateManager.h"

#include <OgreWindowEventUtilities.h>

namespace Morkidios {

	StateManager::StateManager()
	{
		mbShutdown = false;
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
	
		int timeSinceLastFrame = 1;
		int startTime = 0;
	
		while(!mbShutdown)
		{
			if(Framework::getSingletonPtr()->mRenderWindow->isClosed())mbShutdown = true;
	
			Ogre::WindowEventUtilities::messagePump();
	
			if(Framework::getSingletonPtr()->mRenderWindow->isActive())
			{
				startTime = Framework::getSingletonPtr()->mTimer->getMillisecondsCPU();
	
				Framework::getSingletonPtr()->mKeyboard->capture();
				Framework::getSingletonPtr()->mMouse->capture();
	
				mActiveStateStack.back()->update((float)(timeSinceLastFrame));
		
				Framework::getSingletonPtr()->updateOgre((float)(timeSinceLastFrame));
				Framework::getSingletonPtr()->mRoot->renderOneFrame();

				timeSinceLastFrame = Framework::getSingletonPtr()->mTimer->getMillisecondsCPU() - startTime;
			}
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
	
	
	void StateManager::init(State* state)
	{
		Framework::getSingletonPtr()->mKeyboard->setEventCallback(state);
		Framework::getSingletonPtr()->mMouse->setEventCallback(state);

		Framework::getSingletonPtr()->mRenderWindow->resetStatistics();
	}

}