#ifndef _INPUTLISTERNERS_H_
#define _INPUTLISTERNERS_H_

union SDL_Event;
struct SDL_MouseButtonEvent;

struct SDL_TextInputEvent;
struct SDL_KeyboardEvent;

struct SDL_JoyButtonEvent;
struct SDL_JoyAxisEvent;
struct SDL_JoyHatEvent;

namespace Common {
	/**
	 * @class MouseListener
	 * @brief
	 *	Simple Listener implementing mouse functions
	 * @remarks
	 *	Does nothing and needs an inheritance
	 * @ingroup Common
	 */
	class MouseListener {
	public:
		/// Function called when mouse is moved
		virtual void mouseMoved(const SDL_Event& arg) {}
		/// Function called when mouse's button is pressed
		virtual void mousePressed(const SDL_MouseButtonEvent& arg, Ogre::uint8 id) {}
		/// Function called when mouse's button is released
		virtual void mouseReleased(const SDL_MouseButtonEvent& arg, Ogre::uint8 id) {}
	};

	/**
	 * @class KeyboardListener
	 * @brief
	 *	Simple Listener implementing keyboard functions
	 * @remarks
	 *	Does nothing and needs an inheritance
	 * @ingroup Common
	 */
	class KeyboardListener {
	public:
		/// Function called when text is entered
		virtual void textInput(const SDL_TextInputEvent& arg) {}
		/// Function called when a key is pressed
		virtual void keyPressed(const SDL_KeyboardEvent& arg) {}
		/// Function called when a key is released
		virtual void keyReleased(const SDL_KeyboardEvent& arg) {}
	};

	/**
	 * @class JoystickListener
	 * @brief
	 *	Simple Listener implementing joystick functions
	 * @remarks
	 *	Does nothing and needs an inheritance
	 * @ingroup Common
	 */
	class JoystickListener {
	public:
		/// Function called when a joystick's button is pressed
		virtual void joyButtonPressed(const SDL_JoyButtonEvent& evt, int button) {}
		/// Function called when a joystick's button is released
		virtual void joyButtonReleased(const SDL_JoyButtonEvent& evt, int button) {}
		/// Function called when a joystick is moved
		virtual void joyAxisMoved(const SDL_JoyAxisEvent& arg, int axis) {}
		/// Function called when a joystick is moved
		virtual void joyPovMoved(const SDL_JoyHatEvent& arg, int index) {}
	};
}

#endif // _INPUTLISTERNERS_H_
