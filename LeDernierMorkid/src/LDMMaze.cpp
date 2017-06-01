#include "LDMMaze.h"

// MazeNavigationAlgorithm
void MazeNavigationAlgorithm::navigate(Morkidios::AI* ai){
	Ogre::Vector2 pos = ai->getMemories()->oldPos[ai->getMemories()->oldPos.size()-1];

	// Check if he is in a room
	LDMMazeRenderer::Room r;
	if(mMaze->getMazeRenderer()->isInRoom(pos, &r)){
		int i = rand() % r.doors.size();
		Morkidios::Task t;
		t.action = Morkidios::Task::Moving;
		t.position = mMaze->getMazeRenderer()->getIn3D(Ogre::Vector2(r.doors[i].first, r.doors[i].second));
		ai->getTasks()->push_back(t);
		return;
	}

	// Else choose next case
	int x = mMaze->getMazeRenderer()->getIn2D(pos).x, y = mMaze->getMazeRenderer()->getIn2D(pos).y;
	int walls = mMaze->getMazeRenderer()->getMazeArray()[x][y];
	Ogre::Vector2 newPos = Ogre::Vector2::ZERO;
	Ogre::Vector2 oldPos = Ogre::Vector2::ZERO;
	if(ai->getMemories()->oldTasks.size() > 1)
		if(ai->getMemories()->oldTasks[1].action == Morkidios::Task::Moving)
			oldPos = ai->getMemories()->oldTasks[1].position;

	std::vector<Ogre::Vector2> availableWays;
	if(!(walls & WALL_TOP) && oldPos != mMaze->getMazeRenderer()->getIn3D(Ogre::Vector2(x+0.5, y-0.5)))
		availableWays.push_back(mMaze->getMazeRenderer()->getIn3D(Ogre::Vector2(x+0.5, y-0.5)));
	if(!(walls & WALL_BOTTOM) && oldPos != mMaze->getMazeRenderer()->getIn3D(Ogre::Vector2(x+0.5, y+1.5)))
		availableWays.push_back(mMaze->getMazeRenderer()->getIn3D(Ogre::Vector2(x+0.5, y+1.5)));
	if(!(walls & WALL_LEFT) && oldPos != mMaze->getMazeRenderer()->getIn3D(Ogre::Vector2(x-0.5, y+0.5)))
		availableWays.push_back(mMaze->getMazeRenderer()->getIn3D(Ogre::Vector2(x-0.5, y+0.5)));
	if(!(walls & WALL_RIGHT) && oldPos != mMaze->getMazeRenderer()->getIn3D(Ogre::Vector2(x+1.5, y+0.5)))
		availableWays.push_back(mMaze->getMazeRenderer()->getIn3D(Ogre::Vector2(x+1.5, y+0.5)));

	if(availableWays.size() != 0)
		newPos = availableWays[rand() % availableWays.size()];
	else			// If no ways, get the ancient way
		newPos = oldPos;

	Morkidios::Task t;
	t.action = Morkidios::Task::Moving;
	t.position = newPos;
	ai->getTasks()->push_back(t);
}
void MazeNavigationAlgorithm::searchForEnnemy(Morkidios::AI* ai){
	// First, check if it sees an enemy or if he is chasing
	if(ai->getTasks()->front().action == Morkidios::Task::Chasing){
//		ai->getTasks()->front().position = Ogre::Vector2(ai->getTasks()->front().target->getPosition().x,ai->getTasks()->front().target->getPosition().z);
		return;
	}
	for(size_t i = 0; i < ai->getEnemy().size(); i++){
		btVector3 start = ai->getGhostObject()->getWorldTransform().getOrigin();
		btVector3 end = ai->getEnemy()[i]->getGhostObject()->getWorldTransform().getOrigin();
		Morkidios::Utils::ClosestNotMeRayResultCallback rayCallback(ai->getGhostObject());
		mMaze->getWorld()->rayTest(start, end, rayCallback);
		if(rayCallback.hasHit()){
			if(rayCallback.m_collisionObject == ai->getEnemy()[i]->getGhostObject()){
				Morkidios::Task t;
				t.action = Morkidios::Task::Chasing;
				t.position = Ogre::Vector2(ai->getEnemy()[i]->getSceneNode()->getPosition().x, ai->getEnemy()[i]->getSceneNode()->getPosition().z);
				t.target = ai->getEnemy()[i];
				ai->getTasks()->push_back(t);
				return;
			}
		}
	}
}
// ------------------------

// Construction methodes
LDMMaze::LDMMaze(){
	// Ogre
	mGeometry = NULL;

	// Bullet
	mWorld = NULL;
	mDispatcher = NULL;
	mBroadphase = NULL;
	mSolver = NULL;

	// AI
	mAIForEach = 0;
	mNavigationAlgorithm = new MazeNavigationAlgorithm(this);
}
LDMMaze::~LDMMaze(){
	Ogre::MeshManager::getSingleton().remove("plane");
}
void LDMMaze::init(std::string name, Ogre::SceneManager* smgr){
	Morkidios::Terrain::init(smgr, MAZE_SIZE, MAZE_HEIGHT, name);

	Ogre::Plane plane;
	plane.normal = Ogre::Vector3::UNIT_Y;
	plane.d = 0;

	Ogre::MeshManager::getSingleton().createPlane("plane", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, MAZE_SIZE, MAZE_SIZE, 50, 50, true, 1, 35, 35, Ogre::Vector3::UNIT_Z);

	mGeometry->setRegionDimensions(Ogre::Vector3(MAZE_SIZE, MAZE_SIZE, MAZE_SIZE));
	mGeometry->setOrigin(Ogre::Vector3(-MAZE_SIZE/2, 0, -MAZE_SIZE/2));

	Ogre::Entity* mGround = smgr->createEntity("plane");
	mGround->setMaterialName("GroundMaterialNormalMappedSpecular");
	mGround->setCastShadows(true);
	mGeometry->addEntity(mGround, Ogre::Vector3::ZERO);

	mGroundShape = new btStaticPlaneShape(btVector3(0,1,0), 0);
	mGroundBody = new btRigidBody(0, new btDefaultMotionState(), mGroundShape);

	mGroundBody->setRestitution(0);
	mGroundBody->setFriction(1);
	mWorld->addRigidBody(mGroundBody);

	Ogre::Entity* mRoof = smgr->createEntity("plane");
	mRoof->setMaterialName("GroundMaterialNormalMappedSpecular");
	mRoof->setCastShadows(true);
	mGeometry->addEntity(mRoof, Ogre::Vector3(0,MAZE_HEIGHT,0), Ogre::Quaternion(Ogre::Degree(180),Ogre::Vector3(1,0,0)));

	mMaze = new LDMMazeRenderer();
	mMaze->init(mWorld, mGeometry, smgr, Ogre::Vector3(MAZE_SIZE, MAZE_HEIGHT, MAZE_SIZE));
	mMaze->createRoom(7,7,12,12,2);
	mMaze->run();
	mMaze->getImage().save("Map.bmp");

	mGeometry->setCastShadows(true);
	mGeometry->build();
}

// Return value methodes
Ogre::Image LDMMaze::getImage(){
	return mMaze->getImage();
}
LDMMazeRenderer* LDMMaze::getMazeRenderer(){
	return mMaze;
}
