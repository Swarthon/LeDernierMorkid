#include "LDMMaze.h"

// Construction methodes
LDMMaze::LDMMaze(){
	// Ogre
	mGeometry = NULL;

	// Bullet
	mWorld = NULL;
}
LDMMaze::~LDMMaze(){
}
void LDMMaze::init(std::string name, Ogre::SceneManager* smgr){
	Morkidios::Terrain::init(smgr, MAZE_SIZE, MAZE_HEIGHT, name);

	Ogre::Plane plane;
	plane.normal = Ogre::Vector3::UNIT_Y;
	plane.d = 0;

	Ogre::MeshManager::getSingleton().createPlane("plane", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, MAZE_SIZE, MAZE_SIZE, 10, 10, true, 1, 50.0, 50.0, Ogre::Vector3::UNIT_Z);

	Ogre::Entity* mGround = smgr->createEntity("plane");
	mGround->setMaterialName("Examples/GrassFloor");
	mGround->setCastShadows(false);
	mGeometry->addEntity(mGround, Ogre::Vector3::ZERO);

	mGroundShape = new btStaticPlaneShape(btVector3(0,1,0), 0);
	mGroundBody = new btRigidBody(0, new btDefaultMotionState(), mGroundShape);

	mGroundBody->setRestitution(0);
	mGroundBody->setFriction(1);
	mWorld->addRigidBody(mGroundBody);

	Ogre::Entity* mRoof = smgr->createEntity("plane");
	mRoof->setMaterialName("Examples/GrassFloor");
	mRoof->setCastShadows(false);
	mGeometry->addEntity(mRoof, Ogre::Vector3(0,MAZE_HEIGHT,0), Ogre::Quaternion(Ogre::Degree(180),Ogre::Vector3(1,0,0)));

	mMaze.init(mWorld, mGeometry, smgr, Ogre::Vector3(MAZE_SIZE, MAZE_HEIGHT, MAZE_SIZE));
	mMaze.createRoom(7,7,12,12,2);
	mMaze.run();
	mMaze.getImage().save("Map.bmp");

	mGeometry->setRegionDimensions(Ogre::Vector3(MAZE_SIZE, MAZE_SIZE, MAZE_SIZE));
	mGeometry->setOrigin(Ogre::Vector3(-MAZE_SIZE/2, 0, -MAZE_SIZE/2));
	mGeometry->build();
}

// Return value methodes
Ogre::Image LDMMaze::getImage(){
	return mMaze.getImage();
}