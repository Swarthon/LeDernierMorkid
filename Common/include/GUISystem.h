#ifndef _GUISYSTEM_H_
#define _GUISYSTEM_H_

#include "BaseSystem.h"

#include <OgreFrameListener.h>

namespace Common {
        /**
         * @class GUISystem
         * @brief
         *      GUISystem managing GUI
         * @ingroup Common
         */
        class GUISystem : public BaseSystem, public Ogre::FrameListener {
        public:
                /**
                 * Constructor
                 */
                GUISystem();
                /**
                 * Destructor
                 */
                virtual ~GUISystem();
                /**
		 * Initialize the system
		 * @param renderTarget
		 *	RenderTarget to which draw the GUI (e.g. RenderWindow or RenderTexture)
		 */
                void initialize(Ogre::RenderTarget& renderTarget);
                		/**
		 * Deinitialize the system
		 */
		void deinitialize(void);
                /**
		 * Update the system.
		 * @param timeSinceLast
		 *	Time past since last frame
                 * @rmarks
                 *      Called every frame by Ogre::Root
		 */
                bool frameRenderingQueued(const Ogre::FrameEvent &evt);
                /// Function called when mouse is moved
		virtual void mouseMoved(const SDL_Event& arg);
		/// Function called when mouse's button is pressed
		virtual void mousePressed(const SDL_MouseButtonEvent& arg, Ogre::uint8 id);
		/// Function called when mouse's button is released
		virtual void mouseReleased(const SDL_MouseButtonEvent& arg, Ogre::uint8 id);


                
                void initialize(void);
                void update(double);
                void textInput(const SDL_TextInputEvent& arg);
                void keyPressed(const SDL_KeyboardEvent& arg);
                void keyReleased(const SDL_KeyboardEvent& arg);
                void joyButtonPressed(const SDL_JoyButtonEvent& evt, int button);
                void joyButtonReleased(const SDL_JoyButtonEvent& evt, int button);
                void joyAxisMoved(const SDL_JoyAxisEvent& arg, int axis);
                void joyPovMoved(const SDL_JoyHatEvent& arg, int index);
        protected:
        };
}

#endif // _GUISYSTEM_H_
