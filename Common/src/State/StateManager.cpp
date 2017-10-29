#include "State/StateManager.h"

Common::StateManager* Common::StateManager::mInstance = NULL;

namespace Common {

        StateManager::StateManager(GraphicsSystem* graphicsSystem, LogicSystem* logicSystem) {
                mEmitter = graphicsSystem;
                mGraphicsSystem = graphicsSystem;
                mLogicSystem = logicSystem;
        }
        //----------------------------------------------------------------------------------------------------
        StateManager::~StateManager() {
        }
        //----------------------------------------------------------------------------------------------------
        void StateManager::manageState(Ogre::String stateName, State* state) {
                mStates [stateName] = StatePtr(state);
                state->_notifyGraphicsSystem(mGraphicsSystem);
                state->_notifyLogicSystem(mLogicSystem);
        }
        //----------------------------------------------------------------------------------------------------
        State* StateManager::findByName(Ogre::String stateName) {
                return mStates[stateName].get();
        }
        //----------------------------------------------------------------------------------------------------
        void StateManager::changeState(State* state) {
                if(!mActiveStates.empty()) {
                        emit(Mq::STATE_EXIT, mActiveStates.back());
                        mActiveStates.pop_back();
                }
                
                mActiveStates.push_back(state);
                emit(Mq::STATE_ENTER, state);
        }
        //----------------------------------------------------------------------------------------------------
        void StateManager::pushState(State* state) {
                if(!mActiveStates.empty())
                        emit(Mq::STATE_PAUSE, mActiveStates.back());
                mActiveStates.push_back(state);
                emit(Mq::STATE_ENTER, state);
        }
        //----------------------------------------------------------------------------------------------------
        void StateManager::popState() {
                if(!mActiveStates.empty()) {
                        emit(Mq::STATE_EXIT, mActiveStates.back());
                        mActiveStates.pop_back();
                }
                if(!mActiveStates.empty())
                        emit(Mq::STATE_RESUME, mActiveStates.back());
                else
                        shutdown();
        }
        //----------------------------------------------------------------------------------------------------
        void StateManager::pauseState() {
                if(!mActiveStates.empty())
                        emit(Mq::STATE_PAUSE, mActiveStates.back());
                if(mActiveStates.size() > 2) 
                        emit(Mq::STATE_RESUME, mActiveStates.at(mActiveStates.size() - 2));
        }
        //----------------------------------------------------------------------------------------------------
        void StateManager::shutdown() {
                emit(Mq::SHUTDOWN, 0);
        }
        //----------------------------------------------------------------------------------------------------
        void StateManager::popAllAndPushState(State* state) {
                while(!mActiveStates.empty()) {
                        emit(Mq::STATE_EXIT, mActiveStates.back());
                        mActiveStates.pop_back();
                }
                pushState(state);
        }
        //----------------------------------------------------------------------------------------------------
        void StateManager::updateGraphics(double timeSinceLast) {
                mActiveStates.back()->updateGraphics(timeSinceLast);
        }
        //----------------------------------------------------------------------------------------------------
        void StateManager::updateLogics(double timeSinceLast) {
                mActiveStates.back()->updateLogics(timeSinceLast);
        }
        //----------------------------------------------------------------------------------------------------
        void StateManager::enterGraphics() {
                mActiveStates.back()->enterGraphics();
        }
        //----------------------------------------------------------------------------------------------------
        void StateManager::enterLogics() {
                mActiveStates.back()->enterLogics();
        }
        //----------------------------------------------------------------------------------------------------
	void StateManager::addReceiver(BaseSystem* receiver) {
		mReceivers.push_back(receiver);
	}
	//----------------------------------------------------------------------------------------------------
	void StateManager::emit(const Mq::MessageId message, const State* state) {
		for (size_t i = 0; i < mReceivers.size(); i++)
			mEmitter->queueSendMessage(mReceivers[i], message, state);
	}
        //----------------------------------------------------------------------------------------------------
        StateManager* StateManager::createStateManager(GraphicsSystem* graphicsSystem, LogicSystem* logicSystem) {
                if(mInstance)
                        throw "StateManager instance has already been created";
                mInstance = new StateManager(graphicsSystem, logicSystem);
                return mInstance;
        }
        //----------------------------------------------------------------------------------------------------
        StateManager* StateManager::getSingleton() {
                if(!mInstance)
                        throw "Trying to get Singleton of StateManager without having created it";
                return mInstance;
        }
        //----------------------------------------------------------------------------------------------------
}
