#include "GUISystem.h"

#include "State/State.h"
#include "State/StateManager.h"

#include <CEGUI/CEGUI.h>
#include <CEGUI/RendererModules/Ogre/Renderer.h>

#include <OgreRoot.h>
#include <OgreTextureManager.h>
#include <OgreHardwarePixelBuffer.h>
#include <OgreRenderTexture.h>

#include <SDL2/SDL.h>

#include <iostream>

namespace Common {
        //----------------------------------------------------------------------------------------------------
        GUISystem::GUISystem()
                : BaseSystem() {
        }
        //----------------------------------------------------------------------------------------------------
        GUISystem::~GUISystem() {
        }
        //----------------------------------------------------------------------------------------------------
        void GUISystem::initialize(Ogre::RenderTarget& renderTarget) {
                CEGUI::OgreRenderer::bootstrapSystem(renderTarget);
		CEGUI::ImageManager::setImagesetDefaultResourceGroup("Imagesets");
		CEGUI::Font::setDefaultResourceGroup("Fonts");
		CEGUI::Scheme::setDefaultResourceGroup("Schemes");
		CEGUI::WidgetLookManager::setDefaultResourceGroup("LookNFeel");
		CEGUI::WindowManager::setDefaultResourceGroup("Layouts");

                CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(CEGUI::WindowManager::getSingleton().createWindow("DefaultWindow", "RootWindow"));

                CEGUI::SchemeManager::getSingleton().createFromFile("AlfiskoSkin.scheme");
                CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setDefaultImage("AlfiskoSkin/MouseArrow");
                CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setVisible(true);

                CEGUI::Window* mWindow = CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow()->createChild("DefaultWindow", "PauseMenu");
                mWindow->setSize(CEGUI::USize(CEGUI::UDim(1,0),CEGUI::UDim(1,0)));
                mWindow->setPosition(CEGUI::UVector2(CEGUI::UDim(0,0), CEGUI::UDim(0,0)));
                mWindow->setAlwaysOnTop(true);
                
                CEGUI::FontManager::getSingleton().createFreeTypeFont ("PauseMenuButtonFont", 40, true, "GreatVibes-Regular.ttf");
                
                CEGUI::Window* mContinueButton = mWindow->createChild("AlfiskoSkin/Button", "PauseMenuContinuerButton");
                mContinueButton->setProperty("Text", "Continue");
                mContinueButton->setProperty("Font", "PauseMenuButtonFont");
                mContinueButton->setSize(CEGUI::USize(CEGUI::UDim(0.3,0), CEGUI::UDim(0.1,0)));
                mContinueButton->setHorizontalAlignment(CEGUI::HA_RIGHT);
                mContinueButton->setPosition(CEGUI::UVector2(CEGUI::UDim(-0.16,0), CEGUI::UDim(0.1,0)));

                Ogre::Root::getSingleton().addFrameListener(this);
        }
        //----------------------------------------------------------------------------------------------------
        void GUISystem::deinitialize() {
                CEGUI::Renderer* renderer = CEGUI::System::getSingleton().getRenderer();
                CEGUI::System::destroy();
                CEGUI::OgreRenderer::destroy(static_cast<CEGUI::OgreRenderer&>(*renderer)); 
        }
        //----------------------------------------------------------------------------------------------------
        bool GUISystem::frameRenderingQueued(const Ogre::FrameEvent &evt) {
                CEGUI::System::getSingleton().injectTimePulse(evt.timeSinceLastFrame);
                CEGUI::System& guiSystem = CEGUI::System::getSingleton();
                CEGUI::Renderer* guiRenderer(guiSystem.getRenderer());
                
                guiRenderer->beginRendering();
                
                guiSystem.getDefaultGUIContext().injectTimePulse(evt.timeSinceLastEvent);
                guiSystem.getDefaultGUIContext().draw();
                
                guiRenderer->endRendering();
                
                CEGUI::WindowManager::getSingleton().cleanDeadPool();
                
                return true;
        }
        //-----------------------------------------------------------------------------------------------------
        void GUISystem::mouseMoved(const SDL_Event& arg) {
                CEGUI::GUIContext &gui = CEGUI::System::getSingleton().getDefaultGUIContext();
                gui.injectMouseMove(arg.motion.xrel, arg.motion.yrel);
                gui.injectMouseWheelChange(arg.wheel.y);

                StateManager::getSingleton()->getActiveState()->mouseMoved(arg);
        }
        //----------------------------------------------------------------------------------------------------
        void GUISystem::mousePressed(const SDL_MouseButtonEvent& arg, Ogre::uint8 id) {
                CEGUI::GUIContext& context = CEGUI::System::getSingleton().getDefaultGUIContext(); 
                switch (id) {
		case SDL_BUTTON_LEFT:
			context.injectMouseButtonDown(CEGUI::LeftButton);
			break;
		case SDL_BUTTON_MIDDLE:
			context.injectMouseButtonDown(CEGUI::MiddleButton);
			break;
		case SDL_BUTTON_RIGHT:
			context.injectMouseButtonDown(CEGUI::RightButton);
			break;
                }
        }
        //----------------------------------------------------------------------------------------------------
        void GUISystem::mouseReleased(const SDL_MouseButtonEvent& arg, Ogre::uint8 id) {
                CEGUI::GUIContext& context = CEGUI::System::getSingleton().getDefaultGUIContext(); 
                switch (id) {
                case SDL_BUTTON_LEFT:
                        context.injectMouseButtonUp(CEGUI::LeftButton);
                        break;
                case SDL_BUTTON_MIDDLE:
                        context.injectMouseButtonUp(CEGUI::MiddleButton);
                        break;
                case SDL_BUTTON_RIGHT:
                        context.injectMouseButtonUp(CEGUI::RightButton);
                        break;
                }
        }
        //----------------------------------------------------------------------------------------------------
        void GUISystem::initialize(void) {}
        //----------------------------------------------------------------------------------------------------
        void GUISystem::update(double) {}
        //----------------------------------------------------------------------------------------------------
        void GUISystem::textInput(const SDL_TextInputEvent& arg) {}
        //----------------------------------------------------------------------------------------------------
        void GUISystem::keyPressed(const SDL_KeyboardEvent& arg) {
                StateManager::getSingleton()->getActiveState()->keyPressed(arg);                
        }
        //----------------------------------------------------------------------------------------------------
        void GUISystem::keyReleased(const SDL_KeyboardEvent& arg) {
                StateManager::getSingleton()->getActiveState()->keyReleased(arg);
        }
        //----------------------------------------------------------------------------------------------------
        void GUISystem::joyButtonPressed(const SDL_JoyButtonEvent& evt, int button) {}
        //----------------------------------------------------------------------------------------------------
        void GUISystem::joyButtonReleased(const SDL_JoyButtonEvent& evt, int button) {}
        //----------------------------------------------------------------------------------------------------
        void GUISystem::joyAxisMoved(const SDL_JoyAxisEvent& arg, int axis) {}
        //----------------------------------------------------------------------------------------------------
        void GUISystem::joyPovMoved(const SDL_JoyHatEvent& arg, int index) {}
}
