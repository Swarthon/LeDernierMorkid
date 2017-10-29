#include "LeDernierMorkid.h"

#include <Compositor/OgreCompositorManager2.h>
#include <OgreArchive.h>
#include <OgreArchiveManager.h>
#include <OgreConfigFile.h>
#include <OgreHlms.h>
#include <OgreHlmsManager.h>
#include <OgreHlmsPbs.h>
#include <OgreHlmsUnlit.h>
#include <OgreRoot.h>

#include "Terrain/Hlms/HlmsTerrain.h"
#include "State/StateManager.h"
#include "GameState.h"

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

	Ogre::String shaderSyntax = "GLSL";

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
	mGraphicsSystem = new LeDernierMorkidGraphicsSystem(mGraphicsState,
	                                                    Ogre::ColourValue(0.41, 0.62, 1));
        mGUISystem   = new GUISystem();
	mLogicSystem = new LogicSystem(mLogicState);

        Common::StateManager* stateManager = Common::StateManager::createStateManager(mGraphicsSystem,mLogicSystem);
        GameState::create(stateManager, "GameState");
        stateManager->pushState(stateManager->findByName("GameState"));

	mGraphicsSystem->_notifyLogicSystem(mLogicSystem);
	mLogicSystem->_notifyGraphicsSystem(mGraphicsSystem);

        mGraphicsSystem->mGUISystem = mGUISystem;

	mGameEntityManager = new GameEntityManager(mGraphicsSystem, mLogicSystem);

	registerThread(logicThread, THREAD_GET(logicThread));
	registerThread(renderThread, THREAD_GET(renderThread));

	mThreadData                                                = new LeDernierMorkidThreadData;
	((LeDernierMorkidThreadData*) mThreadData)->graphicsSystem = mGraphicsSystem;
	((LeDernierMorkidThreadData*) mThreadData)->logicSystem    = mLogicSystem;
	((LeDernierMorkidThreadData*) mThreadData)->guiSystem      = mGUISystem;
}
//---------------------------------------------------------------------
