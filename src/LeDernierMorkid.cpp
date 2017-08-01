#include "LeDernierMorkid.h"

#include <Compositor/OgreCompositorManager2.h>
#include <OgreArchive.h>
#include <OgreArchiveManager.h>
#include <OgreConfigFile.h>
#include <OgreHlms.h>
#include <OgreHlmsPbs.h>
#include <OgreHlmsUnlit.h>
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

	registerThread(logicThread, THREAD_GET(logicThread));
	registerThread(renderThread, THREAD_GET(renderThread));

	mThreadData = new LeDernierMorkidThreadData;
	((LeDernierMorkidThreadData*)mThreadData)->graphicsSystem = mGraphicsSystem;
	((LeDernierMorkidThreadData*)mThreadData)->logicSystem    = mLogicSystem;
}
//---------------------------------------------------------------------
//---------------------------------------------------------------------
unsigned long LeDernierMorkid::logicThread(Ogre::ThreadHandle* threadHandle) {
	LeDernierMorkidThreadData*     threadData     = reinterpret_cast<LeDernierMorkidThreadData*>(threadHandle->getUserParam());
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
