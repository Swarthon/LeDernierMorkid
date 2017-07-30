#include "LeDernierMorkid.h"

#include <Compositor/OgreCompositorManager2.h>
#include <OgreArchive.h>
#include <OgreArchiveManager.h>
#include <OgreConfigFile.h>
#include <OgreHlms.h>
#include <OgreHlmsManager.h>
#include <OgreRoot.h>

#include "Terrain/Hlms/HlmsTerrain.h"

#include <iostream>

const double cFrametime = 1.0 / 25.0;

void LeDernierMorkidGraphicsSystem::registerHlms(void) {
	Ogre::ConfigFile cf;
	cf.load(mResourcePath + "resources.cfg");

	Ogre::String dataFolder = cf.getSetting("DoNotUseAsResource", "Hlms", "");

	if (dataFolder.empty())
		dataFolder = "./";
	else if (*(dataFolder.end() - 1) != '/')
		dataFolder += "/";

	Ogre::String        shaderSyntax = "GLSL";

	Ogre::Archive* archiveLibrary = Ogre::ArchiveManager::getSingletonPtr()->load(
	        dataFolder + "Hlms/Common/" + shaderSyntax, "FileSystem", true);
	Ogre::Archive* archivePbsLibraryAny = Ogre::ArchiveManager::getSingletonPtr()->load(
	        dataFolder + "Hlms/Pbs/Any", "FileSystem", true);
	Ogre::Archive* archivePbs = Ogre::ArchiveManager::getSingletonPtr()->load(
	        dataFolder + "Hlms/Pbs/" + shaderSyntax, "FileSystem", true);
	Ogre::Archive* archiveUnlit = Ogre::ArchiveManager::getSingletonPtr()->load(
		dataFolder + "Hlms/Unlit/" + shaderSyntax, "FileSystem", true);

	Ogre::ArchiveVec library;
	library.push_back(archiveLibrary);
	library.push_back(archivePbsLibraryAny);
	library.push_back(archivePbs);

	Ogre::HlmsPbs* hlmsPbs = OGRE_NEW Ogre::HlmsPbs(archivePbs, &library);
	Ogre::Root::getSingleton().getHlmsManager()->registerHlms(hlmsPbs);

	Ogre::HlmsUnlit* hlmsUnlit = OGRE_NEW Ogre::HlmsUnlit(archiveUnlit, &library);
	Ogre::Root::getSingleton().getHlmsManager()->registerHlms(hlmsUnlit);

	Ogre::Archive* archiveTerrain = Ogre::ArchiveManager::getSingletonPtr()->load(
	        dataFolder + "Hlms/Terrain/" + shaderSyntax, "FileSystem", true);
	HlmsTerrain* hlmsTerrain                        = OGRE_NEW HlmsTerrain(archiveTerrain, &library);
	Ogre::HlmsManager*                  hlmsManager = mRoot->getHlmsManager();
	hlmsManager->registerHlms(hlmsTerrain);
}
//---------------------------------------------------------------------
LeDernierMorkid::LeDernierMorkid() {
	mGraphicsGameState = new GraphicsGameState();
	mLogicGameState    = new LogicGameState();

	mGraphicsSystem = new LeDernierMorkidGraphicsSystem(mGraphicsGameState,
	                                                    Ogre::ColourValue(0.41, 0.62, 1));
	mLogicSystem = new LogicSystem(mLogicGameState);

	mGraphicsGameState->_notifyGraphicsSystem(mGraphicsSystem);
	mLogicGameState->_notifyLogicSystem(mLogicSystem);

	mGraphicsSystem->_notifyLogicSystem(mLogicSystem);
	mLogicSystem->_notifyGraphicsSystem(mGraphicsSystem);

	mGameEntityManager = new GameEntityManager(mGraphicsSystem, mLogicSystem);
}
//---------------------------------------------------------------------
void LeDernierMorkid::run() {
	Ogre::Barrier barrier(2);
	ThreadData    threadData;
	threadData.graphicsSystem = mGraphicsSystem;
	threadData.logicSystem    = mLogicSystem;
	threadData.barrier        = &barrier;

	Ogre::ThreadHandlePtr threadHandles[2];
	threadHandles[0] = Ogre::Threads::CreateThread(THREAD_GET(renderThread), 0, &threadData);
	threadHandles[1] = Ogre::Threads::CreateThread(THREAD_GET(logicThread), 1, &threadData);

	Ogre::Threads::WaitForThreads(2, threadHandles);
}

//---------------------------------------------------------------------
unsigned long LeDernierMorkid::renderThread(Ogre::ThreadHandle* threadHandle) {
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
unsigned long LeDernierMorkid::renderThreadApp(Ogre::ThreadHandle* threadHandle) {
	ThreadData*     threadData     = reinterpret_cast<ThreadData*>(threadHandle->getUserParam());
	GraphicsSystem* graphicsSystem = threadData->graphicsSystem;
	Ogre::Barrier*  barrier        = threadData->barrier;

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
unsigned long LeDernierMorkid::logicThread(Ogre::ThreadHandle* threadHandle) {
	ThreadData*     threadData     = reinterpret_cast<ThreadData*>(threadHandle->getUserParam());
	GraphicsSystem* graphicsSystem = threadData->graphicsSystem;
	LogicSystem*    logicSystem    = threadData->logicSystem;
	Ogre::Barrier*  barrier        = threadData->barrier;

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
