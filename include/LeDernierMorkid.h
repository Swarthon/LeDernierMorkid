#ifndef _LEDERNIERMORKID_H
#define _LEDERNIERMORKID_H_

#include "CameraController.h"
#include "GameEntityManager.h"
#include "SdlInputHandler.h"

#include "Graphics/GraphicsGameState.h"
#include "Graphics/GraphicsThread.h"
#include "GraphicsSystem.h"

#include "Logic/LogicGameState.h"
#include "Logic/LogicThread.h"
#include "LogicSystem.h"

#include "Threading/ThreadManager.h"
#include "Threading/YieldTimer.h"

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

	struct LeDernierMorkidThreadData : ThreadData {
		GraphicsSystem* graphicsSystem;
		LogicSystem*    logicSystem;
	};

protected:
	GraphicsGameState* mGraphicsGameState;
	LogicGameState*    mLogicGameState;
	GraphicsSystem*    mGraphicsSystem;
	LogicSystem*       mLogicSystem;
	GameEntityManager* mGameEntityManager;
};

#endif // _LEDERNIERMORKID_H_
