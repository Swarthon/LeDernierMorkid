#include "GraphicsGameState.h"
#include "GraphicsSystem.h"
#include "Terrain/HlmsTerrain.h"
#include "Terrain/HlmsTerrainDatablock.h"

#include "CollisionCameraController.h"

#include <OgreSceneManager.h>

#include <OgreCamera.h>
#include <OgreFrameStats.h>
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

#include "LogicSystem.h"

extern const double cFrametime;

GraphicsGameState::GraphicsGameState()
                : mGraphicsSystem(0), mEnableInterpolation(true), mCameraController(0) {
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
void GraphicsGameState::createScene(void) {
	Ogre::Root*         root         = mGraphicsSystem->getRoot();
	Ogre::SceneManager* sceneManager = mGraphicsSystem->getSceneManager();
	mCameraController                = new Collision::CollisionCameraController(mGraphicsSystem, ((Common::LogicSystem*) mGraphicsSystem->mLogicSystem)->getWorld());

	mGraphicsSystem->getCamera()->setPosition(-10.0f, 80.0f, 10.0f);

	mTerrain = new TerrainGraphics(Ogre::Id::generateNewId<Ogre::MovableObject>(),
	                               &sceneManager->_getEntityMemoryManager(Ogre::SCENE_STATIC),
	                               sceneManager,
	                               0,
	                               root->getCompositorManager2(),
	                               mGraphicsSystem->getCamera());
	mTerrain->setCastShadows(false);
	mTerrain->buildGraphics("terrain.png",
	                        Ogre::Vector3::ZERO,
	                        Ogre::Vector3(2048.0f, 100.0f, 2048.0f));

	Ogre::SceneNode* rootNode  = sceneManager->getRootSceneNode(Ogre::SCENE_STATIC);
	Ogre::SceneNode* sceneNode = rootNode->createChildSceneNode(Ogre::SCENE_STATIC);
	sceneNode->attachObject(mTerrain);

	Ogre::HlmsManager*        hlmsManager        = root->getHlmsManager();
	Ogre::HlmsTextureManager* hlmsTextureManager = hlmsManager->getTextureManager();
	HlmsTerrainDatablock*     datablock          = static_cast<HlmsTerrainDatablock*>(
	        hlmsManager->getHlms(Ogre::HLMS_USER3)->getDefaultDatablock());

	assert(dynamic_cast<HlmsTerrainDatablock*>(datablock));

	Ogre::HlmsTextureManager::TextureLocation texLocation;
	texLocation = hlmsTextureManager->createOrRetrieveTexture(
	        "terrain_texture.png", Ogre::HlmsTextureManager::TEXTURE_TYPE_ENV_MAP);
	datablock->setRoughness(0, 0.1);
	datablock->setTexture(TERRAIN_DETAIL0, texLocation.xIdx, texLocation.texture);
	datablock->setTexture(TERRAIN_DETAIL_ROUGHNESS0, texLocation.xIdx, texLocation.texture);
	datablock->setTexture(TERRAIN_DETAIL_METALNESS0, texLocation.xIdx, texLocation.texture);
	datablock->setDetailMapOffsetScale(0, Ogre::Vector4(0, 0, 1, 1));
	texLocation = hlmsTextureManager->createOrRetrieveTexture(
	        "terrain_texture.png", Ogre::HlmsTextureManager::TEXTURE_TYPE_ENV_MAP);
	datablock->setRoughness(1, 0.1);
	datablock->setTexture(TERRAIN_DETAIL1, texLocation.xIdx, texLocation.texture);
	datablock->setTexture(TERRAIN_DETAIL_ROUGHNESS1, texLocation.xIdx, texLocation.texture);
	datablock->setTexture(TERRAIN_DETAIL_METALNESS1, texLocation.xIdx, texLocation.texture);
	datablock->setDetailMapOffsetScale(1, Ogre::Vector4(0, 0, 1, 1));
	texLocation = hlmsTextureManager->createOrRetrieveTexture(
	        "terrain_texture.png", Ogre::HlmsTextureManager::TEXTURE_TYPE_ENV_MAP);
	datablock->setRoughness(2, 0.1);
	datablock->setTexture(TERRAIN_DETAIL2, texLocation.xIdx, texLocation.texture);
	datablock->setTexture(TERRAIN_DETAIL_ROUGHNESS2, texLocation.xIdx, texLocation.texture);
	datablock->setTexture(TERRAIN_DETAIL_METALNESS2, texLocation.xIdx, texLocation.texture);
	datablock->setDetailMapOffsetScale(2, Ogre::Vector4(0, 0, 1, 1));
	texLocation = hlmsTextureManager->createOrRetrieveTexture(
	        "terrain_texture.png", Ogre::HlmsTextureManager::TEXTURE_TYPE_ENV_MAP);
	datablock->setRoughness(3, 0.1);
	datablock->setTexture(TERRAIN_DETAIL3, texLocation.xIdx, texLocation.texture);
	datablock->setTexture(TERRAIN_DETAIL_ROUGHNESS3, texLocation.xIdx, texLocation.texture);
	datablock->setTexture(TERRAIN_DETAIL_METALNESS3, texLocation.xIdx, texLocation.texture);
	datablock->setDetailMapOffsetScale(3, Ogre::Vector4(0, 0, 1, 1));
	datablock->setDiffuse(Ogre::Vector3(1, 1, 1));

	mTerrain->setDatablock(static_cast<Ogre::HlmsDatablock*>(datablock));

	/*	mSunLight                  = sceneManager->createLight();
	Ogre::SceneNode* lightNode = rootNode->createChildSceneNode();
	lightNode->attachObject(mSunLight);
	mSunLight->setPowerScale(Ogre::Math::PI);
	mSunLight->setType(Ogre::Light::LT_DIRECTIONAL);
	mSunLight->setDirection(Ogre::Vector3(-1, -1, -1).normalisedCopy());
	mSunLight->setDirection(
	        Ogre::Quaternion(Ogre::Radian(0), Ogre::Vector3::UNIT_Y) * Ogre::Vector3(cosf(Ogre::Math::PI * 0.1f), -sinf(Ogre::Math::PI * 0.1f), 0.0).normalisedCopy());*/
}
//------------------------------------------------------------------------------------------------
void GraphicsGameState::update(float timeSinceLast) {
	float weight = mGraphicsSystem->getAccumTimeSinceLastLogicFrame() / cFrametime;
	weight       = std::min(1.0f, weight);

	if (!mEnableInterpolation)
		weight = 0;

	mTerrain->update(Ogre::Vector3::ZERO); // Now lightDir isn't used

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
