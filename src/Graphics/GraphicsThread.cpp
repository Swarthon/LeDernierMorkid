#include "Graphics/GraphicsThread.h"

#include <iostream>

#include "LeDernierMorkid.h"

unsigned long renderThread(Ogre::ThreadHandle* threadHandle) {
	unsigned long retVal = -1;

	try {
		retVal = renderThreadApp(threadHandle);
	}
	catch (Ogre::Exception& e) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
		MessageBoxA(NULL,
		            e.getFullDescription().c_str(),
		            "An exception has occured!",
		            MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
		std::cerr << "An exception has occured: " << e.getFullDescription().c_str()
		          << std::endl;
#endif
		abort();
	}
	return retVal;
}
//---------------------------------------------------------------------
unsigned long renderThreadApp(Ogre::ThreadHandle* threadHandle) {
	LeDernierMorkid::LeDernierMorkidThreadData* threadData     = reinterpret_cast<LeDernierMorkid::LeDernierMorkidThreadData*>(threadHandle->getUserParam());
	GraphicsSystem*                             graphicsSystem = threadData->graphicsSystem;
	Ogre::Barrier*                              barrier        = threadData->barrier;

	graphicsSystem->initialize("Le Dernier Morkid");
	graphicsSystem->getInputHandler()->setGrabMousePointer(true);
	graphicsSystem->getInputHandler()->setMouseVisible(false);
	barrier->sync();

	if (graphicsSystem->getQuit()) {
		graphicsSystem->deinitialize();
		return 0; // User cancelled config
	}

	graphicsSystem->createScene();
	barrier->sync();

	Ogre::RenderWindow* renderWindow = graphicsSystem->getRenderWindow();

	Ogre::Timer   timer;
	unsigned long startTime     = timer.getMicroseconds();
	double        timeSinceLast = 1.0 / 60.0;

	while (!graphicsSystem->getQuit()) {
		graphicsSystem->beginFrameParallel();
		graphicsSystem->update(timeSinceLast);
		graphicsSystem->finishFrameParallel();

		if (!renderWindow->isVisible()) {
			// Don't burn CPU cycles unnecessary when we're minimized.
			Ogre::Threads::Sleep(500);
		}

		unsigned long endTime = timer.getMicroseconds();
		timeSinceLast         = (endTime - startTime) / 1000000.0;
		timeSinceLast         = std::min(1.0, timeSinceLast); // Prevent from going haywire.
		startTime             = endTime;
	}

	barrier->sync();

	graphicsSystem->destroyScene();
	barrier->sync();

	graphicsSystem->deinitialize();
	barrier->sync();

	return 0;
}
//---------------------------------------------------------------------
extern THREAD_DECLARE(renderThread);
