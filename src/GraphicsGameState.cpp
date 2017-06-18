#include "GraphicsGameState.h"
#include "GraphicsSystem.h"
#include "CameraController.h"
#include "HlmsTerrain.h"
#include "HlmsTerrainDatablock.h"

#include <OgreSceneManager.h>

#include <OgreRoot.h>
#include <OgreCamera.h>
#include <OgreFrameStats.h>

#include <OgreHlmsManager.h>
#include <OgreHlms.h>
#include <OgreHlmsCompute.h>
#include <OgreGpuProgramManager.h>
#include <OgreItem.h>
#include <OgreMeshManager.h>
#include <OgreMeshManager2.h>
#include <OgreMesh2.h>
#include <OgreArchiveManager.h>
#include <OgreHlmsTextureManager.h>
#include <OgreHlmsPbs.h>
#include <OgreHlmsSamplerblock.h>
#include <OgreImage.h>

extern const double cFrametime;

GraphicsGameState::GraphicsGameState() :
	mGraphicsSystem( 0 ),
	mEnableInterpolation( true ),
        mCameraController( 0 ) {
}
//-----------------------------------------------------------------------------------
GraphicsGameState::~GraphicsGameState() {
        delete mCameraController;
        mCameraController = 0;
}
//-----------------------------------------------------------------------------------
void GraphicsGameState::_notifyGraphicsSystem(Common::GraphicsSystem *graphicsSystem) {
	mGraphicsSystem = graphicsSystem;
}
//-----------------------------------------------------------------------------------
void GraphicsGameState::createScene(void) {
	Ogre::Root *root = mGraphicsSystem->getRoot();
        Ogre::SceneManager *sceneManager = mGraphicsSystem->getSceneManager();
	mCameraController = new Common::CameraController(mGraphicsSystem);
	mGraphicsSystem->getCamera()->setPosition( -10.0f, 80.0f, 10.0f );

	mTerrain = new Terrain( Ogre::Id::generateNewId<Ogre::MovableObject>(),
                                  &sceneManager->_getEntityMemoryManager( Ogre::SCENE_STATIC ),
                                  sceneManager, 0, root->getCompositorManager2(),
                                  mGraphicsSystem->getCamera() );
        mTerrain->setCastShadows( false );
        mTerrain->build("terrain.png", Ogre::Vector3( 64.0f, 4096.0f * 0.0f, 64.0f ), Ogre::Vector3( 1024.0f, 100.0f, 1024.0f ));

        Ogre::SceneNode *rootNode = sceneManager->getRootSceneNode( Ogre::SCENE_STATIC );
        Ogre::SceneNode *sceneNode = rootNode->createChildSceneNode( Ogre::SCENE_STATIC );
        sceneNode->attachObject( mTerrain );

        Ogre::HlmsManager *hlmsManager = root->getHlmsManager();
	Ogre::HlmsTextureManager *hlmsTextureManager = hlmsManager->getTextureManager();
	HlmsTerrainDatablock *datablock = static_cast<HlmsTerrainDatablock*>(hlmsManager->getHlms( Ogre::HLMS_USER3 )->getDefaultDatablock());

	assert(dynamic_cast<HlmsTerrainDatablock*>(datablock));

	Ogre::HlmsTextureManager::TextureLocation texLocation;
	texLocation = hlmsTextureManager->createOrRetrieveTexture("terrain_texture.jpg", Ogre::HlmsTextureManager::TEXTURE_TYPE_ENV_MAP);
	datablock->setRoughness(0,1);
	datablock->setTexture( TERRAIN_DETAIL0, texLocation.xIdx, texLocation.texture );
	datablock->setTexture( TERRAIN_DETAIL_ROUGHNESS0, texLocation.xIdx, texLocation.texture );
	datablock->setTexture( TERRAIN_DETAIL_METALNESS0, texLocation.xIdx, texLocation.texture );
	datablock->setDetailMapOffsetScale(0,Ogre::Vector4(0,0,1,1));
	texLocation = hlmsTextureManager->createOrRetrieveTexture("grass_green-01_diffusespecular.dds", Ogre::HlmsTextureManager::TEXTURE_TYPE_ENV_MAP);
	datablock->setRoughness(1,1);
	datablock->setTexture( TERRAIN_DETAIL1, texLocation.xIdx, texLocation.texture );
	datablock->setTexture( TERRAIN_DETAIL_ROUGHNESS1, texLocation.xIdx, texLocation.texture );
	datablock->setTexture( TERRAIN_DETAIL_METALNESS1, texLocation.xIdx, texLocation.texture );
	datablock->setDetailMapOffsetScale(1,Ogre::Vector4(0,0,32,32));
	texLocation = hlmsTextureManager->createOrRetrieveTexture("terr_rock-dirt.jpg", Ogre::HlmsTextureManager::TEXTURE_TYPE_ENV_MAP);
	datablock->setTexture( TERRAIN_DETAIL2, texLocation.xIdx, texLocation.texture );
	datablock->setTexture( TERRAIN_DETAIL_ROUGHNESS2, texLocation.xIdx, texLocation.texture );
	datablock->setTexture( TERRAIN_DETAIL_METALNESS2, texLocation.xIdx, texLocation.texture );
	datablock->setDetailMapOffsetScale(2,Ogre::Vector4(0,0,32,32));
	texLocation = hlmsTextureManager->createOrRetrieveTexture("terr_dirt-grass.jpg", Ogre::HlmsTextureManager::TEXTURE_TYPE_ENV_MAP);
	datablock->setTexture( TERRAIN_DETAIL3, texLocation.xIdx, texLocation.texture );
	datablock->setTexture( TERRAIN_DETAIL_ROUGHNESS3, texLocation.xIdx, texLocation.texture );
	datablock->setTexture( TERRAIN_DETAIL_METALNESS3, texLocation.xIdx, texLocation.texture );
	datablock->setDetailMapOffsetScale(3,Ogre::Vector4(0,0,32,32));
	datablock->setDiffuse(Ogre::Vector3(1,1,1));

        mTerrain->setDatablock(static_cast<Ogre::HlmsDatablock*>(datablock));

	mSunLight = sceneManager->createLight();
        Ogre::SceneNode *lightNode = rootNode->createChildSceneNode();
        lightNode->attachObject( mSunLight );
        mSunLight->setPowerScale( Ogre::Math::PI );
        mSunLight->setType( Ogre::Light::LT_DIRECTIONAL );
        mSunLight->setDirection( Ogre::Vector3( -1, -1, -1 ).normalisedCopy() );
        mSunLight->setDirection( Ogre::Quaternion( Ogre::Radian(0), Ogre::Vector3::UNIT_Y ) * Ogre::Vector3( cosf(  Ogre::Math::PI * 0.1f ), -sinf( Ogre::Math::PI * 0.1f ), 0.0 ).normalisedCopy() );
}
//-----------------------------------------------------------------------------------
void GraphicsGameState::update(float timeSinceLast) {
	float weight = mGraphicsSystem->getAccumTimeSinceLastLogicFrame() / cFrametime;
	weight = std::min( 1.0f, weight );

	if( !mEnableInterpolation )
		weight = 0;

	mTerrain->update(mSunLight->getDerivedDirectionUpdated());

	mGraphicsSystem->updateGameEntities(mGraphicsSystem->getGameEntities( Ogre::SCENE_DYNAMIC ), weight);
	if(mCameraController)
		mCameraController->update(timeSinceLast);

	Ogre::Camera *camera = mGraphicsSystem->getCamera();
	Ogre::Vector3 camPos = camera->getPosition();
//	if( mTerrain->getHeightAt( camPos ) )
//		camera->setPosition( camPos + Ogre::Vector3::UNIT_Y * 10.0f );
}
//-----------------------------------------------------------------------------------
void GraphicsGameState::keyPressed(const SDL_KeyboardEvent &arg) {
        if(mCameraController)
        	mCameraController->keyPressed(arg);
	GameState::keyPressed(arg);
}
//-----------------------------------------------------------------------------------
void GraphicsGameState::keyReleased(const SDL_KeyboardEvent &arg) {
        if(mCameraController)
        	mCameraController->keyReleased(arg);
	GameState::keyReleased(arg);
}
//-----------------------------------------------------------------------------------
void GraphicsGameState::mouseMoved(const SDL_Event &arg) {
	if(mCameraController)
        	mCameraController->mouseMoved(arg);

        GameState::mouseMoved( arg );
}
