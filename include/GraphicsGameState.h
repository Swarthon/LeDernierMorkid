#ifndef _GRAPHICSGAMESTATE_H_
#define _GRAPHICSGAMESTATE_H_

#include <OgrePrerequisites.h>
#include "GameState.h"

namespace Common {
	class GraphicsSystem;
	class CameraController;
}

class GraphicsGameState : public Common::GameState {
protected:
	bool				mEnableInterpolation;
	Common::GraphicsSystem		*mGraphicsSystem;
        Common::CameraController	*mCameraController;

public:
	GraphicsGameState();
	virtual ~GraphicsGameState();

	void _notifyGraphicsSystem(Common::GraphicsSystem *graphicsSystem);

	virtual void createScene(void);

	virtual void update(float timeSinceLast);

	virtual void keyPressed(const SDL_KeyboardEvent &arg);
	virtual void keyReleased(const SDL_KeyboardEvent &arg);

	virtual void mouseMoved(const SDL_Event &arg);
};

#endif // _GRAPHICSGAMESTATE_H_
