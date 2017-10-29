#ifndef _BASESYSTEM_H_
#define _BASESYSTEM_H_

#include "Threading/MessageQueueSystem.h"
#include "InputListeners.h"

namespace Common {
        class State;

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
	class BaseSystem : public Mq::MessageQueueSystem, public MouseListener, public KeyboardListener, public JoystickListener {
	protected:
	public:
		/**
		 * Constructor of BaseSystem
		 * @param State
 		 *	State to which transmit the events
		 */
		BaseSystem();
		/// Simple Destructor
		virtual ~BaseSystem();

		/**
		 * Initialize the System
		 * @see
 		 *	State::initialize
		 */
		virtual void initialize(void);
		/**
		 * Deinitialize the System
		 * @see
 		 *	State::uninitialize
		 */
		virtual void deinitialize(void);

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

                /// Function called when mouse is moved
		virtual void mouseMoved(const SDL_Event& arg);
		/// Function called when mouse's button is pressed
		virtual void mousePressed(const SDL_MouseButtonEvent& arg, Ogre::uint8 id);
		/// Function called when mouse's button is released
		virtual void mouseReleased(const SDL_MouseButtonEvent& arg, Ogre::uint8 id);
                /// Function called when text is entered
		virtual void textInput(const SDL_TextInputEvent& arg);
		/// Function called when a key is pressed
		virtual void keyPressed(const SDL_KeyboardEvent& arg);
		/// Function called when a key is released
		virtual void keyReleased(const SDL_KeyboardEvent& arg);
                /// Function called when a joystick's button is pressed
		virtual void joyButtonPressed(const SDL_JoyButtonEvent& evt, int button);
		/// Function called when a joystick's button is released
		virtual void joyButtonReleased(const SDL_JoyButtonEvent& evt, int button);
		/// Function called when a joystick is moved
		virtual void joyAxisMoved(const SDL_JoyAxisEvent& arg, int axis);
		/// Function called when a joystick is moved
		virtual void joyPovMoved(const SDL_JoyHatEvent& arg, int index);
	};
}

#endif // _BASESYSTEM_H_
