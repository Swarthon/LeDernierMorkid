#ifndef _LEDERNIERMORKID_H
#define _LEDERNIERMORKID_H_

#include "GraphicsSystem.h"
#include "LogicSystem.h"
#include "GameEntityManager.h"
#include "GraphicsGameState.h"
#include "LogicGameState.h"
#include "CameraController.h"
#include "SdlInputHandler.h"

#include "Threading/YieldTimer.h"

#include <OgreRenderWindow.h>
#include <OgreTimer.h>

#include <Threading/OgreThreads.h>
#include <Threading/OgreBarrier.h>

#include <iostream>

using namespace Common;

extern const double cFrametime;

class LeDernierMorkid {
public:
	LeDernierMorkid();
	void run();
protected:
	struct ThreadData {
		GraphicsSystem  *graphicsSystem;
		LogicSystem     *logicSystem;
		Ogre::Barrier   *barrier;
	};

	static unsigned long renderThread(Ogre::ThreadHandle *threadHandle);
	static unsigned long renderThreadApp(Ogre::ThreadHandle *threadHandle);
	static unsigned long logicThread(Ogre::ThreadHandle *threadHandle);

	static THREAD_DECLARE( renderThread );
	static THREAD_DECLARE( logicThread );

	GraphicsGameState	*mGraphicsGameState;
	LogicGameState		*mLogicGameState;
	GraphicsSystem		*mGraphicsSystem;
	LogicSystem		*mLogicSystem;
	GameEntityManager	*mGameEntityManager;
	CameraController	*mCameraController;
};

#endif // _LEDERNIERMORKID_H_
