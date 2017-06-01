#ifndef LDMMAZE_H
#define LDMMAZE_H

// Morkidios
#include "Morkidios.h"

// My includes
#include "LDMMazeRenderer.h"

#define MAZE_SIZE 350
#define MAZE_HEIGHT 10

class LDMMaze;

class MazeNavigationAlgorithm : public Morkidios::NavigationAlgorithm {
public:
	MazeNavigationAlgorithm();
	MazeNavigationAlgorithm(LDMMaze* m){
		mMaze = m;
	}

	void navigate(Morkidios::AI*);
	void searchForEnnemy(Morkidios::AI*);
private:
	LDMMaze* mMaze;
};

class LDMMaze : public Morkidios::Terrain {
public:
	// Construction methodes
	LDMMaze();
	~LDMMaze();
	void init(std::string name, Ogre::SceneManager* smgr);

	// Return value methodes
	Ogre::Image getImage();
	LDMMazeRenderer* getMazeRenderer();
private:
	LDMMazeRenderer* mMaze;

	Ogre::Entity* mRoof;
	Ogre::Entity* mGround;
	btCollisionShape* mGroundShape;
	btRigidBody* mGroundBody;
};

#endif // LDMMAZE_H
