#ifndef LDMMAZE_RENDERER_H
#define LDMMAZE_RENDERER_H

// C++ include
#include <stdlib.h>
#include <sstream>

// Morkidios include
#include <Morkidios.h>

// Bullet includes
#include <btBulletDynamicsCommon.h>
#include <BulletDynamics/Dynamics/btRigidBody.h>

#define MAX_MAZE_SIZE_X	10000
#define MAX_MAZE_SIZE_Y	10000

#define START_SQUARE	0x2000
#define END_SQUARE	0x1000

#define DOOR_TOP	0b10000000
#define DOOR_RIGHT	0b1000000
#define DOOR_BOTTOM	0b100000
#define DOOR_LEFT	0b10000

#define WALL_TOP	0b1000
#define WALL_RIGHT	0b100
#define WALL_BOTTOM	0b10
#define WALL_LEFT	0b1
#define WALL_ANY	0xF

#define	border_x        (0)
#define	border_y        (0)

#define	get_random(x)	(random() % (x))

#define BRUSH_SIZE 10

class LDMMazeRenderer {
private:
	struct Room {
		int x, y, xx, yy;
		int doorX, doorY;
	};

	unsigned short** maze;

	int maze_size_x, maze_size_y;
	int start_x, start_y, start_dir, end_x, end_y, end_dir;
	int grid_width, grid_height;
	int bw;

	int x, y;

	int *sets;		/* The sets that our squares are in. */
	int *hedges;		/* The `list' of hedges. */

	int mMazeSizeX;
	int mMazeSizeY;

	// Ogre
	Ogre::Image mImage;
	Ogre::StaticGeometry* mGeometry;
	Ogre::SceneManager* mSceneManager;
	Ogre::Vector3 mSize;

	// Bullet
	btDynamicsWorld* mWorld;

	bool mIsInitialized;
	std::vector<Room> mRooms;

	// Fonctions privates
	void draw_wall (int, int, int);
	void draw_solid_square (int, int, int);
	void build_wall (int, int, int);
	void set_maze_sizes (int, int);
	void mask_out_set_rect(int, int, int, int, int);
	void init_sets();
	int get_set(int);
	void join_sets(int, int);
	void exit_sets();
	void set_create_maze();
	void build_maze_border();
	void build_all();
	void build_3D();
	void build_collisions();
public:
	// Construction methodes
	LDMMazeRenderer();
	LDMMazeRenderer(int xs, int ys);
	void init(btDynamicsWorld* world, Ogre::StaticGeometry* sg, Ogre::SceneManager* smgr, Ogre::Vector3 size = Ogre::Vector3(10000, 500, 10000));
	void run();

	// Return value methodes
	Ogre::Image getImage();
	Ogre::Vector2 getMazeSize();

	// Various methodes
	void createRoom(int x, int y, int xx, int yy, int numDoors = 1);
};

#endif // LDMMAZE_RENDERER_H