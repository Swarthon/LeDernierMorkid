#ifndef _GRAPHICSGAMESTATE_H_
#define _GRAPHICSGAMESTATE_H_

#include <OgrePrerequisites.h>

#include "GameState.h"
#include "Terrain/GraphicsTerrain.h"

namespace Common {
	class GraphicsSystem;
	class CameraController;
} // namespace Common

/**
 * @class GraphicsGameState
 * @brief
 *	GameState describing how graphics are
 */
class GraphicsGameState : public Common::GameState {
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
	GraphicsGameState();
	/// Simple Destructor
	virtual ~GraphicsGameState();

	/**
	 * Set mGraphicsSystem value
	 * @param graphicsSystem
	 *	GraphicsSystem to set
	 */
	void _notifyGraphicsSystem(Common::GraphicsSystem* graphicsSystem);

	/// Create the scene
	virtual void createScene(void);

	/**
	 * Update the State. Method to call once per frame
	 * @param timeSinceLast
	 *	Time past since last frame
	 */
	virtual void update(float timeSinceLast);

	/// Method called when a key is pressed
	virtual void keyPressed(const SDL_KeyboardEvent& arg);
	/// Method called when a key is pressed
	virtual void keyReleased(const SDL_KeyboardEvent& arg);

	/// Method called when the mouse is moved
	virtual void mouseMoved(const SDL_Event& arg);
};

#endif // _GRAPHICSGAMESTATE_H_
