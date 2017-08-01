#ifndef _LEDERNIERMORKID_H
#define _LEDERNIERMORKID_H_

#include "CameraController.h"
#include "GameEntityManager.h"
#include "GraphicsGameState.h"
#include "GraphicsSystem.h"
#include "LogicGameState.h"
#include "LogicSystem.h"
#include "SdlInputHandler.h"

#include "Threading/YieldTimer.h"
#include "Threading/ThreadManager.h"

#include <OgreRenderWindow.h>
#include <OgreTimer.h>

using namespace Common;

extern const double cFrametime;

class LeDernierMorkidGraphicsSystem : public GraphicsSystem {
public:
	LeDernierMorkidGraphicsSystem(GraphicsGameState* gameState, Ogre::ColourValue colourValue)
	: GraphicsSystem(gameState, colourValue){};

protected:
	virtual void registerHlms(void);

};

class LeDernierMorkid : public ThreadManager {
public:
	LeDernierMorkid();

protected:
	struct LeDernierMorkidThreadData : ThreadData {
		GraphicsSystem* graphicsSystem;
		LogicSystem*    logicSystem;
	};

	static unsigned long renderThread(Ogre::ThreadHandle* threadHandle);
	static unsigned long renderThreadApp(Ogre::ThreadHandle* threadHandle);
	static unsigned long logicThread(Ogre::ThreadHandle* threadHandle);

	static THREAD_DECLARE(renderThread);
	static THREAD_DECLARE(logicThread);

	GraphicsGameState* mGraphicsGameState;
	LogicGameState*    mLogicGameState;
	GraphicsSystem*    mGraphicsSystem;
	LogicSystem*       mLogicSystem;
	GameEntityManager* mGameEntityManager;
	CameraController*  mCameraController;
};

#endif // _LEDERNIERMORKID_H_
