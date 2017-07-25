#include "GraphicsSystem.h"
#include "GameEntity.h"
#include "GameState.h"
#include "SdlInputHandler.h"

#include "Converter.h"
#include "ObjectState.h"

#include <BulletDynamics/Dynamics/btRigidBody.h>

#include <OgreConfigFile.h>
#include <OgreException.h>
#include <OgreRoot.h>

#include <OgreCamera.h>
#include <OgreItem.h>
#include <OgreRenderWindow.h>

#include <OgreArchiveManager.h>
#include <OgreHlmsManager.h>
#include <OgreHlmsPbs.h>
#include <OgreHlmsUnlit.h>

#include <Compositor/OgreCompositorManager2.h>

#include <OgreWindowEventUtilities.h>

#include <SDL_syswm.h>

namespace Common {
	GraphicsSystem::GraphicsSystem(GameState* gameState, Ogre::ColourValue backgroundColour)
	                : BaseSystem(gameState),
	                  mLogicSystem(0),
	                  mSdlWindow(0),
	                  mInputHandler(0),
	                  mRoot(0),
	                  mRenderWindow(0),
	                  mSceneManager(0),
	                  mCamera(0),
	                  mWorkspace(0),
	                  mAccumTimeSinceLastLogicFrame(0),
	                  mCurrentTransformIdx(0),
	                  mThreadGameEntityToUpdate(0),
	                  mThreadWeight(0),
	                  mQuit(false),
	                  mBackgroundColour(backgroundColour) {}
	//------------------------------------------------------------------------------------------------
	GraphicsSystem::~GraphicsSystem() { assert(!mRoot && "deinitialize() not called!!!"); }
	//------------------------------------------------------------------------------------------------
	void GraphicsSystem::initialize(const Ogre::String& windowTitle) {
		if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
			OGRE_EXCEPT(Ogre::Exception::ERR_INTERNAL_ERROR,
			            "Cannot initialize SDL2!",
			            "GraphicsSystem::initialize");

		Ogre::String pluginsPath;
// only use plugins.cfg if not static
#ifndef OGRE_STATIC_LIB
#if OGRE_DEBUG_MODE
		pluginsPath = mPluginsPath + "plugins_d.cfg";
#else
		pluginsPath = mPluginsPath + "plugins.cfg";
#endif
#endif

		mRoot = OGRE_NEW Ogre::Root(
		        pluginsPath, mWriteAccessFolder + "ogre.cfg", mWriteAccessFolder + "Ogre.log");

		mStaticPluginLoader.install(mRoot);

		Ogre::RenderSystem* rs = mRoot->getRenderSystemByName("OpenGL 3+ Rendering Subsystem");
		if (!rs)
			OGRE_EXCEPT(Ogre::Exception::ERR_INTERNAL_ERROR, "Wut", "");
		mRoot->setRenderSystem(rs);
		mRoot->getRenderSystem()->setConfigOption("Full Screen", "Yes");
		mRoot->getRenderSystem()->setConfigOption("Video Mode",
		                                          "1920 x 1080 @ 32-bit colour");
		mRoot->getRenderSystem()->setConfigOption("VSync", "Yes");

		mRoot->getRenderSystem()->setConfigOption("sRGB Gamma Conversion", "Yes");
		mRoot->initialise(false);

		Ogre::ConfigOptionMap& cfgOpts = mRoot->getRenderSystem()->getConfigOptions();

		int width  = 1920;
		int height = 1080;

		Ogre::ConfigOptionMap::iterator opt = cfgOpts.find("Video Mode");
		if (opt != cfgOpts.end()) {
			const Ogre::String::size_type start     = opt->second.currentValue.find_first_of("012356789");
			Ogre::String::size_type       widthEnd  = opt->second.currentValue.find(' ', start);
			Ogre::String::size_type       heightEnd = opt->second.currentValue.find(' ', widthEnd + 3);
			width                                   = Ogre::StringConverter::parseInt(
			        opt->second.currentValue.substr(0, widthEnd));
			height = Ogre::StringConverter::parseInt(
			        opt->second.currentValue.substr(widthEnd + 3, heightEnd));
		}

		Ogre::NameValuePairList params;
		bool                    fullscreen = Ogre::StringConverter::parseBool(cfgOpts["Full Screen"].currentValue);
		int                     screen     = 0;
		int                     posX       = SDL_WINDOWPOS_CENTERED_DISPLAY(screen);
		int                     posY       = SDL_WINDOWPOS_CENTERED_DISPLAY(screen);

		if (fullscreen) {
			posX = SDL_WINDOWPOS_UNDEFINED_DISPLAY(screen);
			posY = SDL_WINDOWPOS_UNDEFINED_DISPLAY(screen);
		}

		mSdlWindow = SDL_CreateWindow(windowTitle.c_str(), // window title
		                              posX,                // initial x position
		                              posY,                // initial y position
		                              width,               // width, in pixels
		                              height,              // height, in pixels
		                              SDL_WINDOW_SHOWN | (fullscreen ? SDL_WINDOW_FULLSCREEN : 0) | SDL_WINDOW_RESIZABLE);

		SDL_SysWMinfo wmInfo;
		SDL_VERSION(&wmInfo.version);

		if (SDL_GetWindowWMInfo(mSdlWindow, &wmInfo) == SDL_FALSE) {
			OGRE_EXCEPT(Ogre::Exception::ERR_INTERNAL_ERROR,
			            "Couldn't get WM Info! (SDL2)",
			            "GraphicsSystem::initialize");
		}

		Ogre::String winHandle;
		switch (wmInfo.subsystem) {
#ifdef WIN32
		case SDL_SYSWM_WINDOWS:
			winHandle = Ogre::StringConverter::toString((uintptr_t) wmInfo.info.win.window);
			break;
#else
		case SDL_SYSWM_X11:
			winHandle = Ogre::StringConverter::toString((uintptr_t) wmInfo.info.x11.window);
			break;
#endif
		default:
			OGRE_EXCEPT(Ogre::Exception::ERR_NOT_IMPLEMENTED,
			            "Unexpected WM! (SDL2)",
			            "GraphicsSystem::initialize");
			break;
		}

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
		params.insert(std::make_pair("externalWindowHandle", winHandle));
#else
		params.insert(std::make_pair("parentWindowHandle", winHandle));
#endif

		params.insert(std::make_pair("title", windowTitle));
		params.insert(std::make_pair("gamma", "true"));
		params.insert(std::make_pair("FSAA", cfgOpts["FSAA"].currentValue));
		params.insert(std::make_pair("vsync", cfgOpts["VSync"].currentValue));

		mRenderWindow = Ogre::Root::getSingleton().createRenderWindow(
		        windowTitle, width, height, fullscreen, &params);

		setupResources();
		loadResources();
		chooseSceneManager();
		createCamera();
		mWorkspace = setupCompositor();

		mInputHandler = new SdlInputHandler(
		        mSdlWindow, mCurrentGameState, mCurrentGameState, mCurrentGameState);

		BaseSystem::initialize();
	}
	//------------------------------------------------------------------------------------------------
	void GraphicsSystem::deinitialize(void) {
		BaseSystem::deinitialize();

		delete mInputHandler;
		mInputHandler = 0;

		OGRE_DELETE mRoot;
		mRoot = 0;

		if (mSdlWindow) {
			// Restore desktop resolution on exit
			SDL_SetWindowFullscreen(mSdlWindow, 0);
			SDL_DestroyWindow(mSdlWindow);
			mSdlWindow = 0;
		}

		SDL_Quit();
	}
	//------------------------------------------------------------------------------------------------
	void GraphicsSystem::update(float timeSinceLast) {
		Ogre::WindowEventUtilities::messagePump();

		SDL_Event evt;
		while (SDL_PollEvent(&evt)) {
			switch (evt.type) {
			case SDL_WINDOWEVENT: handleWindowEvent(evt); break;
			case SDL_QUIT: mQuit = true; break;
			default: break;
			}

			mInputHandler->_handleSdlEvents(evt);
		}

		BaseSystem::update(timeSinceLast);

		if (mRenderWindow->isVisible())
			mQuit |= !mRoot->renderOneFrame();

		mAccumTimeSinceLastLogicFrame += timeSinceLast;
	}
	//------------------------------------------------------------------------------------------------
	void GraphicsSystem::handleWindowEvent(const SDL_Event& evt) {
		switch (evt.window.event) {
		case SDL_WINDOWEVENT_SIZE_CHANGED: int w, h; SDL_GetWindowSize(mSdlWindow, &w, &h);
#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
			mRenderWindow->resize(w, h);
#else
			mRenderWindow->windowMovedOrResized();
#endif
			break;
		case SDL_WINDOWEVENT_RESIZED:
#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
			mRenderWindow->resize(evt.window.data1, evt.window.data2);
#else
			mRenderWindow->windowMovedOrResized();
#endif
			break;
		case SDL_WINDOWEVENT_CLOSE: break;
		case SDL_WINDOWEVENT_SHOWN: mRenderWindow->setVisible(true); break;
		case SDL_WINDOWEVENT_HIDDEN: mRenderWindow->setVisible(false); break;
		}
	}
	//------------------------------------------------------------------------------------------------
	void GraphicsSystem::processIncomingMessage(Mq::MessageId messageId, const void* data) {
		switch (messageId) {
		case Mq::LOGICFRAME_FINISHED: {
			Ogre::uint32 newIdx = *reinterpret_cast<const Ogre::uint32*>(data);

			if (newIdx != std::numeric_limits<Ogre::uint32>::max()) {
				mAccumTimeSinceLastLogicFrame = 0;
				this->queueSendMessage(
				        mLogicSystem,
				        Mq::LOGICFRAME_FINISHED,
				        (mCurrentTransformIdx + NUM_GAME_ENTITY_BUFFERS - 1) % NUM_GAME_ENTITY_BUFFERS);

				assert((mCurrentTransformIdx + 1) % NUM_GAME_ENTITY_BUFFERS == newIdx && "Graphics is receiving indices out of order!!!");

				mCurrentTransformIdx = newIdx;
			}
		} break;
		case Mq::GAME_ENTITY_ADDED:
			gameEntityAdded(
			        reinterpret_cast<const GameEntityManager::CreatedGameEntity*>(data));
			break;
		case Mq::GAME_ENTITY_REMOVED:
			gameEntityRemoved(*reinterpret_cast<GameEntity* const*>(data));
			break;
		case Mq::GAME_ENTITY_SCHEDULED_FOR_REMOVAL_SLOT:
			this->queueSendMessage(mLogicSystem,
			                       Mq::GAME_ENTITY_SCHEDULED_FOR_REMOVAL_SLOT,
			                       *reinterpret_cast<const Ogre::uint32*>(data));
			break;
		default: break;
		}
	}
	//------------------------------------------------------------------------------------------------
	void GraphicsSystem::addResourceLocation(const Ogre::String& archName,
	                                         const Ogre::String& typeName,
	                                         const Ogre::String& secName) {
		Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
		        archName, typeName, secName);
	}
	//------------------------------------------------------------------------------------------------
	void GraphicsSystem::setupResources(void) {
		Ogre::ConfigFile cf;
		cf.load(mResourcePath + "resources.cfg");

		Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();

		Ogre::String secName, typeName, archName;
		while (seci.hasMoreElements()) {
			secName                                      = seci.peekNextKey();
			Ogre::ConfigFile::SettingsMultiMap* settings = seci.getNext();

			if (secName != "Hlms") {
				Ogre::ConfigFile::SettingsMultiMap::iterator i;
				for (i = settings->begin(); i != settings->end(); ++i) {
					typeName = i->first;
					archName = i->second;
					addResourceLocation(archName, typeName, secName);
				}
			}
		}
	}
	//------------------------------------------------------------------------------------------------
	void GraphicsSystem::registerHlms(void) {
		Ogre::ConfigFile cf;
		cf.load(mResourcePath + "resources.cfg");

		Ogre::String dataFolder = mResourcePath + cf.getSetting("DoNotUseAsResource", "Hlms", "");

		if (dataFolder.empty())
			dataFolder = "./";
		else if (*(dataFolder.end() - 1) != '/')
			dataFolder += "/";

		Ogre::RenderSystem* renderSystem = mRoot->getRenderSystem();

		Ogre::String shaderSyntax = "GLSL";
		if (renderSystem->getName() == "Direct3D11 Rendering Subsystem")
			shaderSyntax = "HLSL";
		else if (renderSystem->getName() == "Metal Rendering Subsystem")
			shaderSyntax = "Metal";

		Ogre::Archive* archiveLibrary = Ogre::ArchiveManager::getSingletonPtr()->load(
		        dataFolder + "Hlms/Common/" + shaderSyntax, "FileSystem", true);
		Ogre::Archive* archiveLibraryAny = Ogre::ArchiveManager::getSingletonPtr()->load(
		        dataFolder + "Hlms/Common/Any", "FileSystem", true);
		Ogre::Archive* archivePbsLibraryAny = Ogre::ArchiveManager::getSingletonPtr()->load(
		        dataFolder + "Hlms/Pbs/Any", "FileSystem", true);

		Ogre::ArchiveVec library;
		library.push_back(archiveLibrary);
		library.push_back(archiveLibraryAny);

		Ogre::Archive* archiveUnlit = Ogre::ArchiveManager::getSingletonPtr()->load(
		        dataFolder + "Hlms/Unlit/" + shaderSyntax, "FileSystem", true);

		Ogre::HlmsUnlit* hlmsUnlit = OGRE_NEW Ogre::HlmsUnlit(archiveUnlit, &library);
		Ogre::Root::getSingleton().getHlmsManager()->registerHlms(hlmsUnlit);

		Ogre::Archive* archivePbs = Ogre::ArchiveManager::getSingletonPtr()->load(
		        dataFolder + "Hlms/Pbs/" + shaderSyntax, "FileSystem", true);
		library.push_back(archivePbsLibraryAny);
		Ogre::HlmsPbs* hlmsPbs = OGRE_NEW Ogre::HlmsPbs(archivePbs, &library);
		Ogre::Root::getSingleton().getHlmsManager()->registerHlms(hlmsPbs);
		library.pop_back();

		if (renderSystem->getName() == "Direct3D11 Rendering Subsystem") {
			bool supportsNoOverwriteOnTextureBuffers;
			renderSystem->getCustomAttribute("MapNoOverwriteOnDynamicBufferSRV",
			                                 &supportsNoOverwriteOnTextureBuffers);

			if (!supportsNoOverwriteOnTextureBuffers) {
				hlmsPbs->setTextureBufferDefaultSize(512 * 1024);
				hlmsUnlit->setTextureBufferDefaultSize(512 * 1024);
			}
		}
	}
	//------------------------------------------------------------------------------------------------
	void GraphicsSystem::loadResources(void) {
		registerHlms();

		Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups(true);
	}
	//------------------------------------------------------------------------------------------------
	void GraphicsSystem::chooseSceneManager(void) {
		Ogre::InstancingThreadedCullingMethod threadedCullingMethod = Ogre::INSTANCING_CULLING_SINGLETHREAD;
#if OGRE_DEBUG_MODE
		const size_t numThreads = 1;
#else
		const size_t numThreads = std::max<size_t>(1, Ogre::PlatformInformation::getNumLogicalCores());
#endif
		mSceneManager = mRoot->createSceneManager(
		        Ogre::ST_GENERIC, numThreads, threadedCullingMethod, "Ogre::SceneManager");

		mSceneManager->setShadowDirectionalLightExtrusionDistance(500.0f);
		mSceneManager->setShadowFarDistance(500.0f);
		mSceneManager->setAmbientLight(Ogre::ColourValue(1.f, 1.f, 1.f), Ogre::ColourValue(1.f, 1.f, 1.f), Ogre::Vector3::UNIT_Y);
	}
	//------------------------------------------------------------------------------------------------
	void GraphicsSystem::createCamera(void) {
		mCamera = mSceneManager->createCamera("Main Ogre::Camera");

		mCamera->setPosition(Ogre::Vector3(0, 5, 15));
		mCamera->lookAt(Ogre::Vector3(0, 0, 0));
		mCamera->setNearClipDistance(0.2f);
		mCamera->setFarClipDistance(1000.0f);
		mCamera->setAutoAspectRatio(true);
	}
	//------------------------------------------------------------------------------------------------
	Ogre::CompositorWorkspace* GraphicsSystem::setupCompositor(void) {
		Ogre::CompositorManager2* compositorManager = mRoot->getCompositorManager2();

		const Ogre::String workspaceName("Game Workspace");
		if (!compositorManager->hasWorkspaceDefinition(workspaceName)) {
			compositorManager->createBasicWorkspaceDef(
			        workspaceName, mBackgroundColour, Ogre::IdString());
		}

		return compositorManager->addWorkspace(
		        mSceneManager, mRenderWindow, mCamera, workspaceName, true);
	}
	//------------------------------------------------------------------------------------------------
	void GraphicsSystem::stopCompositor(void) {
		if (mWorkspace) {
			Ogre::CompositorManager2* compositorManager = mRoot->getCompositorManager2();
			compositorManager->removeWorkspace(mWorkspace);
			mWorkspace = 0;
		}
	}
	//------------------------------------------------------------------------------------------------
	void GraphicsSystem::restartCompositor(void) {
		stopCompositor();
		mWorkspace = setupCompositor();
	}
	//------------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------------
	struct GameEntityCmp {
		bool operator()(const GameEntity*    _l,
		                const Ogre::Matrix4* RESTRICT_ALIAS _r) const {
			const Ogre::Transform& transform = _l->mSceneNode->_getTransform();
			return &transform.mDerivedTransform[transform.mIndex] < _r;
		}

		bool operator()(const Ogre::Matrix4* RESTRICT_ALIAS _r,
		                const GameEntity*                   _l) const {
			const Ogre::Transform& transform = _l->mSceneNode->_getTransform();
			return _r < &transform.mDerivedTransform[transform.mIndex];
		}

		bool operator()(const GameEntity* _l, const GameEntity* _r) const {
			const Ogre::Transform& lTransform = _l->mSceneNode->_getTransform();
			const Ogre::Transform& rTransform = _r->mSceneNode->_getTransform();
			return &lTransform.mDerivedTransform[lTransform.mIndex] < &rTransform.mDerivedTransform[rTransform.mIndex];
		}
	};
	//------------------------------------------------------------------------------------------------
	void GraphicsSystem::gameEntityAdded(const GameEntityManager::CreatedGameEntity* cge) {
		Ogre::SceneNode* sceneNode = mSceneManager->getRootSceneNode(cge->gameEntity->mType)
		                                     ->createChildSceneNode(cge->gameEntity->mType,
		                                                            cge->initialTransform.vPos,
		                                                            cge->initialTransform.qRot);
		sceneNode->setScale(cge->initialTransform.vScale);
		cge->gameEntity->mSceneNode = sceneNode;

		if (cge->gameEntity->mMoDefinition->moType == MoTypeItem) {
			Ogre::Item* item = mSceneManager->createItem(
			        cge->gameEntity->mMoDefinition->meshName,
			        cge->gameEntity->mMoDefinition->resourceGroup,
			        cge->gameEntity->mType);

			Ogre::StringVector materialNames = cge->gameEntity->mMoDefinition->submeshMaterials;
			size_t             minMaterials  = std::min(materialNames.size(), item->getNumSubItems());

			for (size_t i = 0; i < minMaterials; ++i) {
				item->getSubItem(i)->setDatablockOrMaterialName(
				        materialNames[i],
				        cge->gameEntity->mMoDefinition->resourceGroup);
			}

			cge->gameEntity->mMovableObject = item;
		}

		sceneNode->attachObject(cge->gameEntity->mMovableObject);

		// Linking Graphics and Physics
		if (dynamic_cast<btRigidBody*>(cge->gameEntity->mCollisionObject)) {
			btTransform transform = Collision::Converter::to(cge->initialTransform);
			static_cast<btRigidBody*>(cge->gameEntity->mCollisionObject)->setMotionState(new Collision::ObjectState(transform, cge->gameEntity, (LogicSystem*) mLogicSystem));
		}

		const Ogre::Transform&  transform    = sceneNode->_getTransform();
		GameEntityVec::iterator itGameEntity = std::lower_bound(mGameEntities[cge->gameEntity->mType].begin(),
		                                                        mGameEntities[cge->gameEntity->mType].end(),
		                                                        &transform.mDerivedTransform[transform.mIndex],
		                                                        GameEntityCmp());
		mGameEntities[cge->gameEntity->mType].insert(itGameEntity, cge->gameEntity);
	}
	//------------------------------------------------------------------------------------------------
	void GraphicsSystem::gameEntityRemoved(GameEntity* toRemove) {
		const Ogre::Transform&  transform    = toRemove->mSceneNode->_getTransform();
		GameEntityVec::iterator itGameEntity = std::lower_bound(mGameEntities[toRemove->mType].begin(),
		                                                        mGameEntities[toRemove->mType].end(),
		                                                        &transform.mDerivedTransform[transform.mIndex],
		                                                        GameEntityCmp());

		assert(itGameEntity != mGameEntities[toRemove->mType].end() && *itGameEntity == toRemove);
		mGameEntities[toRemove->mType].erase(itGameEntity);

		toRemove->mSceneNode->getParentSceneNode()->removeAndDestroyChild(
		        toRemove->mSceneNode);
		toRemove->mSceneNode = 0;

		assert(dynamic_cast<Ogre::Item*>(toRemove->mMovableObject));

		mSceneManager->destroyItem(static_cast<Ogre::Item*>(toRemove->mMovableObject));
		toRemove->mMovableObject = 0;
	}
	//------------------------------------------------------------------------------------------------
	void GraphicsSystem::updateGameEntities(const GameEntityVec& gameEntities, float weight) {
		mThreadGameEntityToUpdate = &gameEntities;
		mThreadWeight             = weight;

		mSceneManager->executeUserScalableTask(this, true);
	}
	//------------------------------------------------------------------------------------------------
	void GraphicsSystem::execute(size_t threadId, size_t numThreads) {
		size_t currIdx = mCurrentTransformIdx;
		size_t prevIdx = (mCurrentTransformIdx + NUM_GAME_ENTITY_BUFFERS - 1) % NUM_GAME_ENTITY_BUFFERS;

		const size_t objsPerThread = (mThreadGameEntityToUpdate->size() + (numThreads - 1)) / numThreads;
		const size_t toAdvance     = std::min(threadId * objsPerThread, mThreadGameEntityToUpdate->size());

		GameEntityVec::const_iterator itor = mThreadGameEntityToUpdate->begin() + toAdvance;
		GameEntityVec::const_iterator end  = mThreadGameEntityToUpdate->begin() + std::min(toAdvance + objsPerThread, mThreadGameEntityToUpdate->size());
		while (itor != end) {
			GameEntity*   gEnt      = *itor;
			Ogre::Vector3 interpVec = Ogre::Math::lerp(gEnt->mTransform[prevIdx]->vPos,
			                                           gEnt->mTransform[currIdx]->vPos,
			                                           mThreadWeight);
			gEnt->mSceneNode->setPosition(interpVec);

			interpVec = Ogre::Math::lerp(gEnt->mTransform[prevIdx]->vScale,
			                             gEnt->mTransform[currIdx]->vScale,
			                             mThreadWeight);
			gEnt->mSceneNode->setScale(interpVec);

			Ogre::Quaternion interpQ = Ogre::Math::lerp(gEnt->mTransform[prevIdx]->qRot,
			                                            gEnt->mTransform[currIdx]->qRot,
			                                            mThreadWeight);
			gEnt->mSceneNode->setOrientation(interpQ);

			++itor;
		}
	}
}
