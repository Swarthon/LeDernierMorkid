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

#include "GUISystem.h"

#include "Threading/ThreadManager.h"
#include "Threading/YieldTimer.h"

#include <OgreRenderWindow.h>
#include <OgreTimer.h>

using namespace Common;

extern const double cFrametime;

/**
 * @class LeDernierMorkidGraphicsSystem
 * @brief
 *	Implementation of GraphicsSystem
 */
class LeDernierMorkidGraphicsSystem : public GraphicsSystem {
public:
	/**
	 * Constructor
	 * @param State
	 *	State to which transmit the events
	 * @param colourValue
	 *	Background colour value
	 */
	LeDernierMorkidGraphicsSystem(GraphicsState* State, Ogre::ColourValue colourValue)
	                : GraphicsSystem(State, colourValue){};

protected:
	/// Method to register hlms resources
	virtual void registerHlms(void);
};

/**
 * @class LeDernierMorkid
 * @brief
 *	Main class of the game
 *
 * Class setting up and running game threads
 */
class LeDernierMorkid : public ThreadManager {
public:
	/// Constructor setting up threads
	LeDernierMorkid();

	/**
	 * @struct LeDernierMorkidThreadData
	 * @brief Struct containing data needed by LeDernierMorkid's threads
	 */
	struct LeDernierMorkidThreadData : ThreadData {
		GraphicsSystem* graphicsSystem;
		LogicSystem*    logicSystem;
                GUISystem*      guiSystem;
	};

protected:
	GraphicsState* mGraphicsState;
	LogicState*    mLogicState;
	GraphicsSystem*    mGraphicsSystem;
	LogicSystem*       mLogicSystem;
        GUISystem*         mGUISystem;
	GameEntityManager* mGameEntityManager;
};

#endif // _LEDERNIERMORKID_H_
