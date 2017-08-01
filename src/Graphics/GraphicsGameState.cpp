#include "Graphics/GraphicsGameState.h"

#include "GraphicsSystem.h"
#include "LogicSystem.h"
#include "Terrain/Hlms/HlmsTerrain.h"
#include "Terrain/Hlms/HlmsTerrainDatablock.h"
#include "Terrain/Hlms/PbsListener/HlmsPbsTerrainShadows.h"
#include "Terrain/TerrainShadowMapper.h"

#include "CollisionCameraController.h"

#include <OgreSceneManager.h>

#include <OgreCamera.h>
#include <OgreFrameStats.h>
#include <OgreRenderWindow.h>
#include <OgreRoot.h>

#include <OgreArchiveManager.h>
#include <OgreGpuProgramManager.h>
#include <OgreHlms.h>
#include <OgreHlmsCompute.h>
#include <OgreHlmsManager.h>
#include <OgreHlmsPbs.h>
#include <OgreHlmsSamplerblock.h>
#include <OgreHlmsTextureManager.h>
#include <OgreImage.h>
#include <OgreItem.h>
#include <OgreMesh2.h>
#include <OgreMeshManager.h>
#include <OgreMeshManager2.h>

#include <OgreHardwarePixelBuffer.h>
#include <OgreRenderTexture.h>
#include <OgreTextureManager.h>

#include <Compositor/OgreCompositorManager2.h>
#include <Compositor/OgreCompositorWorkspace.h>

extern const double cFrametime;

GraphicsGameState::GraphicsGameState()
                : mGraphicsSystem(0), mEnableInterpolation(true), mCameraController(0), mTerrain(0) {
}
//------------------------------------------------------------------------------------------------
GraphicsGameState::~GraphicsGameState() {
	delete mCameraController;
	mCameraController = 0;
}
//------------------------------------------------------------------------------------------------
void GraphicsGameState::_notifyGraphicsSystem(Common::GraphicsSystem* graphicsSystem) {
	mGraphicsSystem = graphicsSystem;
}
//------------------------------------------------------------------------------------------------
Ogre::CompositorWorkspace* GraphicsGameState::setupCompositor() {
	Ogre::Root*               root              = mGraphicsSystem->getRoot();
	Ogre::SceneManager*       sceneManager      = mGraphicsSystem->getSceneManager();
	Ogre::RenderWindow*       renderWindow      = mGraphicsSystem->getRenderWindow();
	Ogre::Camera*             camera            = mGraphicsSystem->getCamera();
	Ogre::CompositorManager2* compositorManager = root->getCompositorManager2();

	Ogre::CompositorChannelVec externalChannels(2);
	//Render window
	externalChannels[0].target = renderWindow;

	//Terrain's Shadow texture
	Ogre::ResourceLayoutMap initialLayouts;
	Ogre::ResourceAccessMap initialUavAccess;

	assert(mTerrain);

	//Terrain is initialized
	const ShadowMapper* shadowMapper = mTerrain->getShadowMapper();
	shadowMapper->fillUavDataForCompositorChannel(externalChannels[1], initialLayouts, initialUavAccess);

	return compositorManager->addWorkspace(sceneManager, externalChannels, camera, "TerrainWorkspace", true, -1, (Ogre::UavBufferPackedVec*) 0, &initialLayouts, &initialUavAccess);
}
//------------------------------------------------------------------------------------------------
void GraphicsGameState::createScene(void) {
	Ogre::Root*         root         = mGraphicsSystem->getRoot();
	Ogre::SceneManager* sceneManager = mGraphicsSystem->getSceneManager();
        sceneManager->setAmbientLight(Ogre::ColourValue(0.5f, 0.5f, 0.5f), Ogre::ColourValue(0.5f, 0.5f, 0.5f), Ogre::Vector3::UNIT_Y);
	mCameraController                = new Collision::CollisionCameraController(mGraphicsSystem, ((Common::LogicSystem*) mGraphicsSystem->mLogicSystem)->getWorld());

	mGraphicsSystem->getCamera()->setPosition(-10.0f, 80.0f, 10.0f);

	mTerrain = new GraphicsTerrain(Ogre::Id::generateNewId<Ogre::MovableObject>(),
	                               &sceneManager->_getEntityMemoryManager(Ogre::SCENE_STATIC),
	                               sceneManager,
	                               0,
	                               root->getCompositorManager2(),
	                               mGraphicsSystem->getCamera());
	mTerrain->setCastShadows(false);
	mTerrain->load("terrain.png", Ogre::Vector3(0.0f, 4096.0f * 0.0f, 0.0f), Ogre::Vector3(1024.0f, 100.0f, 1024.0f));

	Ogre::SceneNode* rootNode  = sceneManager->getRootSceneNode(Ogre::SCENE_STATIC);
	Ogre::SceneNode* sceneNode = rootNode->createChildSceneNode(Ogre::SCENE_STATIC);
	sceneNode->attachObject(mTerrain);

	Ogre::HlmsManager*        hlmsManager        = root->getHlmsManager();
	Ogre::HlmsTextureManager* hlmsTextureManager = hlmsManager->getTextureManager();
	HlmsTerrainDatablock*     datablock          = static_cast<HlmsTerrainDatablock*>(
	        hlmsManager->getHlms(Ogre::HLMS_USER3)->getDefaultDatablock());

	assert(dynamic_cast<HlmsTerrainDatablock*>(datablock));

	Ogre::HlmsTextureManager::TextureLocation texLocation = hlmsTextureManager->createOrRetrieveTexture(
	        "terrain_texture.png", Ogre::HlmsTextureManager::TEXTURE_TYPE_ENV_MAP);
        datablock->setTexture(TERRAIN_REFLECTION, texLocation.xIdx, texLocation.texture);
	datablock->setTexture(TERRAIN_DETAIL0, texLocation.xIdx, texLocation.texture);
        datablock->setTexture(TERRAIN_DETAIL1, texLocation.xIdx, texLocation.texture);
        datablock->setTexture(TERRAIN_DETAIL2, texLocation.xIdx, texLocation.texture);
        datablock->setTexture(TERRAIN_DETAIL3, texLocation.xIdx, texLocation.texture);
        datablock->setDiffuse(Ogre::Vector3(1,1,1));

	mTerrain->setDatablock(static_cast<Ogre::HlmsDatablock*>(datablock));

	mSunLight                  = sceneManager->createLight();
	Ogre::SceneNode* lightNode = rootNode->createChildSceneNode();
	lightNode->attachObject(mSunLight);
	mSunLight->setPowerScale(Ogre::Math::PI);
	mSunLight->setType(Ogre::Light::LT_DIRECTIONAL);
	mSunLight->setDirection(Ogre::Quaternion(Ogre::Radian(126.862 / 180.0f * Ogre::Math::PI), Ogre::Vector3::UNIT_Y) *
	                        Ogre::Vector3(cosf(291.245), -sinf(291.245), 0.0).normalisedCopy());
        mSunLight->setSpecularColour(Ogre::ColourValue(0,0,0));

	HlmsPbsTerrainShadows* mHlmsPbsTerrainShadows = new HlmsPbsTerrainShadows();
	mHlmsPbsTerrainShadows->setTerrain(mTerrain);
	Ogre::Hlms* hlmsPbs = root->getHlmsManager()->getHlms(Ogre::HLMS_PBS);
	hlmsPbs->setListener(mHlmsPbsTerrainShadows);

	setupCompositor();
}
//------------------------------------------------------------------------------------------------
void GraphicsGameState::update(float timeSinceLast) {
	float weight = mGraphicsSystem->getAccumTimeSinceLastLogicFrame() / cFrametime;
	weight       = std::min(1.0f, weight);

	if (!mEnableInterpolation)
		weight = 0;

	const float lightEpsilon = 0.0f;
	mTerrain->update(mSunLight->getDerivedDirectionUpdated(), lightEpsilon);

	mGraphicsSystem->updateGameEntities(mGraphicsSystem->getGameEntities(Ogre::SCENE_DYNAMIC),
	                                    weight);
	if (mCameraController)
		mCameraController->update(timeSinceLast);

	Ogre::Camera* camera = mGraphicsSystem->getCamera();
	Ogre::Vector3 camPos = camera->getPosition();
}
//------------------------------------------------------------------------------------------------
void GraphicsGameState::keyPressed(const SDL_KeyboardEvent& arg) {
	if (mCameraController)
		mCameraController->keyPressed(arg);
	GameState::keyPressed(arg);
}
//------------------------------------------------------------------------------------------------
void GraphicsGameState::keyReleased(const SDL_KeyboardEvent& arg) {
	if (mCameraController)
		mCameraController->keyReleased(arg);
	GameState::keyReleased(arg);
}
//------------------------------------------------------------------------------------------------
void GraphicsGameState::mouseMoved(const SDL_Event& arg) {
	if (mCameraController)
		mCameraController->mouseMoved(arg);

	GameState::mouseMoved(arg);
}
