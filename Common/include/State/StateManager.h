#ifndef _STATEMANAGER_H_
#define _STATEMANAGER_H_

#include "State/State.h"
#include "LogicSystem.h"
#include "GraphicsSystem.h"

#include <OGRE/OgreString.h>

namespace Common {
        class StateManager : public StateListener {
        public:
                ~StateManager();

                State* getActiveState() { return mActiveStates.back(); };
                void manageState(Ogre::String stateName, State* state);

		State* findByName(Ogre::String stateName);

		void changeState(State* state);
		void pushState(State* state);
		void popState();
		void pauseState();
		void shutdown();
                void popAllAndPushState(State* state);

                void updateGraphics(double timeSinceLast);
                void updateLogics(double timeSinceLast);

                void enterGraphics();
                void enterLogics();
                
                void addReceiver(BaseSystem* receiver);

                static StateManager* createStateManager(GraphicsSystem* graphicsSystem, LogicSystem* logicSystem);
                static StateManager* getSingleton();

        protected:
                void emit(const Mq::MessageId message, const State* state);
        private:
                StateManager(GraphicsSystem* graphicsSystem, LogicSystem* logicSystem);
                
                std::vector<State*> mActiveStates;
                StatePtrStringMap mStates;

                BaseSystem* mEmitter;
                GraphicsSystem* mGraphicsSystem;
                LogicSystem* mLogicSystem;
                std::vector<BaseSystem*> mReceivers;

                static StateManager* mInstance;
        };
}

#endif // _STATEMANAGER_H_
