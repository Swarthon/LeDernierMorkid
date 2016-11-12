#ifndef STATE_MANAGER_H
#define	STATE_MANAGER_H

#include <MorkidiosPrerequisites.h>

#include "State.h"

namespace Morkidios {

	class StateManager : public StateListener, public Ogre::FrameListener
	{
	public:
		typedef struct
		{
			Ogre::String name;
			State* state;
		} state_info;

		StateManager();
		~StateManager();

		void manageState(Ogre::String stateName, State* state);

		State* findByName(Ogre::String stateName);

		void start(State* state);
		void changeState(State* state);
		bool pushState(State* state);
		void popState();
		void pauseState();
		void shutdown();
		void popAllAndPushState(State* state);

		static void loadAllStates(StateManager* sm);
		static void addToLoadState(std::string name, void(*createMethod)(StateListener*, const Ogre::String name));
	protected:
		void init(State *state);
		virtual bool frameRenderingQueued(const Ogre::FrameEvent& fe);

		std::vector<State*>		mActiveStateStack;
		std::vector<state_info>		mStates;
		bool				mbShutdown;

		static std::vector<std::pair<std::string,void(*)(StateListener*,const Ogre::String)>> mToLoadStates;
	};

}

#endif
