#ifndef _STATE_H_
#define _STATE_H_

#include <memory>
#include <vector>
#include <map>

#include <OGRE/OgreString.h>

#include "InputListeners.h"

#include "LogicSystem.h"

namespace Common {

	class State;

        typedef std::unique_ptr<State>         StatePtr;
        typedef std::vector<StatePtr>          StatePtrVec;
        typedef std::map<std::string,StatePtr> StatePtrStringMap;
        
	class StateListener {
	public:
		StateListener() {};
		virtual ~StateListener() {};

		virtual void manageState(Ogre::String stateName, State* state) = 0;

		virtual State* findByName(Ogre::String stateName) = 0;
		virtual void changeState(State* state) = 0;
		virtual void pushState(State* state) = 0;
		virtual void popState() = 0;
		virtual void pauseState() = 0;
		virtual void shutdown() = 0;
		virtual void popAllAndPushState(State* state) = 0;
                
                virtual void updateGraphics(double timeSinceLast) = 0;
                virtual void updateLogics(double timeSinceLast) = 0;

                virtual void enterGraphics() = 0;
                virtual void enterLogics() = 0;
	};


	class State : public MouseListener, public KeyboardListener, public JoystickListener {
	public:
		static void create(StateListener* parent, const Ogre::String name) {};
		void destroy() {delete this;}

                virtual void enterGraphics() = 0;
                virtual void enterLogics() = 0;
		virtual void exitGraphics() = 0;
		virtual void exitLogics() = 0;
		virtual bool pause() {return true;}
		virtual void resume() {};
		virtual void updateGraphics(double timeSinceLast) = 0;
                virtual void updateLogics(double timeSinceLast) = 0;

                void _notifyLogicSystem(LogicSystem* system) {mLogicSystem = system;}
                void _notifyGraphicsSystem(GraphicsSystem* system) {mGraphicsSystem = system;}

	protected:
		State() {};

		State* findByName(Ogre::String stateName) {return mParent->findByName(stateName);}
		void changeState(State* state) {mParent->changeState(state);}
		void pushState(State* state) {mParent->pushState(state);}
		void popState() {mParent->popState();}
		void shutdown() {mParent->shutdown();}
		void popAllAndPushState(State* state) {mParent->popAllAndPushState(state);}

		StateListener* mParent;
                LogicSystem* mLogicSystem;
                GraphicsSystem* mGraphicsSystem;

	private:

	};
}

#define DECLARE_APPSTATE_CLASS(T)						\
static void create(Common::StateListener* parent, const Ogre::String name)	\
{										\
	T* myState = new T();							\
	myState->mParent = parent;						\
	parent->manageState(name, myState);					\
}

#endif // _STATE_H_
