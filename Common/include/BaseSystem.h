#ifndef _BASESYSTEM_H_
#define _BASESYSTEM_H_

#include "Threading/MessageQueueSystem.h"

namespace Common {
	class GameState;

	/**
	 * @class BaseSystem
	 * @ingroup Common
	 * @brief
	 *	Simple System implementing basic functions
	 * @remarks
	 *	This class is not useful without inheritance, you should implement it to fit with your expectations
	 * @see
	 *	GraphicsSystem, LogicSystem
	 */
	class BaseSystem : public Mq::MessageQueueSystem {
	protected:
		/**
		 * @var mCurrentGameState
		 * @brief
 		 *	Current GameState to which is transmitted the events
		 * @see
 		 *	BaseSystem::BaseSystem
		 */
		GameState* mCurrentGameState;

	public:
		/**
		 * Constructor of BaseSystem
		 * @param gameState
 		 *	GameState to which transmit the events
		 */
		BaseSystem(GameState* gameState);
		/// Simple Destructor
		virtual ~BaseSystem();

		/**
		 * Initialize the System
		 * @see
 		 *	GameState::initialize
		 */
		virtual void initialize(void);
		/**
		 * Deinitialize the System
		 * @see
 		 *	GameState::uninitialize
		 */
		virtual void deinitialize(void);

		/**
		 * Create the Scene through GameState::createScene
		 * @see
 		 *	GameState::createScene
		 */
		virtual void createScene(void);
		/**
		 * Destroy the Scene through GameState::destroyScene
		 * @see
 		 *	GameState::destroyScene
		 */
		virtual void destroyScene(void);

		/// Method to call at the beginning of the Frame
		void beginFrameParallel(void);
		/**
		 * Method to update the System. Call it once per frame
		 * @param timeSinceLast
 		 *	Time since the last frame
		 */
		void update(float timeSinceLast);
		/// Method to call at the end of the frame
		void finishFrameParallel(void);
		/// Method to call at the end of the frame
		void finishFrame(void);
		/**
		 * Process received messages
		 * @param messageId
 		 *	The Mq::MessageId of the message, used to know what kind of message it is
		 * @param data
 		 *	The data sent with the message, could be anything. Depending on the messageId, a conversion will be needed
		 * @see
 		 *	Common::Mq::MessageQueueSystem::processIncomingMessage
		 */
		void processIncomingMessage(Mq::MessageId messageId, const void* data);
	};
}

#endif // _BASESYSTEM_H_
