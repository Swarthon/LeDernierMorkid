#include "Logic/LogicThread.h"

#include "LeDernierMorkid.h"

unsigned long logicThread(Ogre::ThreadHandle* threadHandle) {
	LeDernierMorkid::LeDernierMorkidThreadData* threadData     = reinterpret_cast<LeDernierMorkid::LeDernierMorkidThreadData*>(threadHandle->getUserParam());
	GraphicsSystem*                             graphicsSystem = threadData->graphicsSystem;
	LogicSystem*                                logicSystem    = threadData->logicSystem;
	Ogre::Barrier*                              barrier        = threadData->barrier;

	logicSystem->initialize();
	barrier->sync();

	if (graphicsSystem->getQuit()) {
		logicSystem->deinitialize();
		return 0; // Render thread cancelled early
	}

	logicSystem->createScene();
	barrier->sync();

	Ogre::RenderWindow* renderWindow = graphicsSystem->getRenderWindow();

	Ogre::Timer timer;
	YieldTimer  yieldTimer(&timer);

	unsigned long startTime = timer.getMicroseconds();

	while (!graphicsSystem->getQuit()) {
		logicSystem->beginFrameParallel();
		logicSystem->update(static_cast<float>(cFrametime));
		logicSystem->finishFrameParallel();

		logicSystem->finishFrame();

		if (!renderWindow->isVisible()) {
			// Don't burn CPU cycles unnecessary when we're minimized.
			Ogre::Threads::Sleep(500);
		}

		// YieldTimer will wait until the current time is greater than startTime +
		// cFrametime
		startTime = yieldTimer.yield(cFrametime, startTime);
	}

	barrier->sync();

	logicSystem->destroyScene();
	barrier->sync();

	logicSystem->deinitialize();
	barrier->sync();

	return 0;
}
//-----------------------------------------------------------------------------
extern THREAD_DECLARE(logicThread);
