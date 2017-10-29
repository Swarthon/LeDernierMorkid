#ifndef _GAMESTATE_H_
#define _GAMESTATE_H_

#include <OgrePrerequisites.h>

#include "State/State.h"
#include "Terrain/GraphicsTerrain.h"
#include "Terrain/CollisionTerrain.h"

namespace Common {
	class GraphicsSystem;
        class LogicSystem;
	class CameraController;
} // namespace Common

class GameState : public Common::State {
public:
        GameState()
                : mEnableInterpolation(true), mCameraController(0), mTerrainGraphics(0) {}
        ~GameState(){}
        
        void enterLogics();
        void updateLogics(double timeSinceLast);
        void exitLogics();

        void enterGraphics();
        void updateGraphics(double timeSinceLast);
        void exitGraphics() {}
        Ogre::CompositorWorkspace* setupCompositor();

        void mouseMoved(const SDL_Event& arg);
        void keyPressed(const SDL_KeyboardEvent& arg);
        void keyReleased(const SDL_KeyboardEvent& arg);

        
 protected:
        /* GRAPHICS */
	/// Interpolation enabled
	bool                      mEnableInterpolation;
	/// Actual CameraController to which send events
	Common::CameraController* mCameraController;
	/// Graphics part of Terrain
	GraphicsTerrain*          mTerrainGraphics;
	/// Light of the sun
	Ogre::Light*              mSunLight;

        /* LOGICS */
	/// Collision part of Terrain
	CollisionTerrain*    mTerrainLogics;

 public:
        DECLARE_APPSTATE_CLASS(GameState)
};

#endif // _GAMESTATE_H_
