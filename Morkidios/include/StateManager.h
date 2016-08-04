#ifndef STATE_MANAGER_H
#define	STATE_MANAGER_H

#include "State.h"
#include <vector>

namespace Morkidios {

	class StateManager : public StateListener
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

	protected:
		void init(State *state);

		std::vector<State*>		mActiveStateStack;
		std::vector<state_info>		mStates;
		bool				mbShutdown;
	};

}

#endif