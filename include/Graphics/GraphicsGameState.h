#ifndef _GRAPHICSState_H_
#define _GRAPHICSState_H_

#include <OgrePrerequisites.h>

#include "State/State.h"
#include "Terrain/GraphicsTerrain.h"

namespace Common {
	class GraphicsSystem;
	class CameraController;
} // namespace Common

/**
 * @class GraphicsState
 * @brief
 *	State describing how graphics are
 */
class GraphicsState : public Common::State {
protected:
	/// Interpolation enabled
	bool                      mEnableInterpolation;
	/// GraphicsSystem containing graphics data
	Common::GraphicsSystem*   mGraphicsSystem;
	/// Actual CameraController to which send events
	Common::CameraController* mCameraController;
	/// Graphics part of Terrain
	GraphicsTerrain*          mTerrain;
	/// Light of the sun
	Ogre::Light*              mSunLight;

	/**
	 * Method setting up the Compositor
	 * @see GraphicsSystem::setupCompositor
	 * @returns
	 *	Created CompositorWorkspace
	 */
	Ogre::CompositorWorkspace* setupCompositor();

public:
	/// Simple Constructor
	GraphicsState();
	/// Simple Destructor
	virtual ~GraphicsState();

	/**
	 * Set mGraphicsSystem value
	 * @param graphicsSystem
	 *	GraphicsSystem to set
	 */
	void _notifyGraphicsSystem(Common::GraphicsSystem* graphicsSystem);

	/// Create the scene
	virtual void enter(void);
        virtual void exit() {}

	/**
	 * Update the State. Method to call once per frame
	 * @param timeSinceLast
	 *	Time past since last frame
	 */
	virtual void update(double timeSinceLast);

	/// Method called when a key is pressed
	virtual void keyPressed(const SDL_KeyboardEvent& arg);
	/// Method called when a key is pressed
	virtual void keyReleased(const SDL_KeyboardEvent& arg);

	/// Method called when the mouse is moved
	virtual void mouseMoved(const SDL_Event& arg);
};

#endif // _GRAPHICSState_H_
