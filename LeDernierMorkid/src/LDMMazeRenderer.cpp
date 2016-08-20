#include "LDMMazeRenderer.h"

/* Mark the maze grid as having a wall at the given coordinate,
   and draw that wall on the screen. */
void LDMMazeRenderer::build_wall(int i, int j, int dir){
	/* Put it in the maze. */
	switch(dir) {
	case 0:
		if(maze[i][j] & DOOR_TOP)
			return;				// So not draw wall and don't build it
		maze[i][j] |= WALL_TOP;
		if(j>0)
			maze[i][j-1] |= WALL_BOTTOM;
		break;
	case 1:
		if(maze[i][j] & DOOR_RIGHT)
			return;				// So not draw wall and don't build it
		maze[i][j] |= WALL_RIGHT;
		if(i<maze_size_x-1)
			maze[i+1][j] |= WALL_LEFT;
		break;
	case 2:
		if(maze[i][j] & DOOR_BOTTOM)
			return;				// So not draw wall and don't build it
		maze[i][j] |= WALL_BOTTOM;
		if(j<maze_size_y-1)
			maze[i][j+1] |= WALL_TOP;
		break;
	case 3:
		if(maze[i][j] & DOOR_LEFT)
			return;				// So not draw wall and don't build it
		maze[i][j] |= WALL_LEFT;
		if(i>0)
			maze[i-1][j] |= WALL_RIGHT;
		break;
	}

	draw_wall(i,j,dir);
}
void LDMMazeRenderer::draw_wall(int i, int j, int dir){
	switch (dir) {									// UP WALL
	case 0:
		Morkidios::Utils::drawLine(mImage, Ogre::ColourValue(1,1,1),
			border_x + grid_width * i, 
			border_y + grid_height * j,
			border_x + grid_width * (i+1), 
			border_y + grid_height * j,
			BRUSH_SIZE);
		break;
	case 1:										// RIGHT WALL
		Morkidios::Utils::drawLine(mImage, Ogre::ColourValue(1,1,1),
			border_x + grid_width * (i+1), 
			border_y + grid_height * j,
			border_x + grid_width * (i+1), 
			border_y + grid_height * (j+1),
			BRUSH_SIZE);
		break;
	case 2:										// DOWN WALL
		Morkidios::Utils::drawLine(mImage, Ogre::ColourValue(1,1,1),
			border_x + grid_width * i, 
			border_y + grid_height * (j+1),
			border_x + grid_width * (i+1), 
			border_y + grid_height * (j+1),
			BRUSH_SIZE);
		break;
	case 3:										// LEFT WALL
		Morkidios::Utils::drawLine(mImage, Ogre::ColourValue(1,1,1),
			border_x + grid_width * i, 
			border_y + grid_height * j,
			border_x + grid_width * i, 
			border_y + grid_height * (j+1),
			BRUSH_SIZE);
		break;
	}
}

void LDMMazeRenderer::set_maze_sizes (int width, int height){
	maze_size_x = width / grid_width;
	maze_size_y = height / grid_height;

	if (maze_size_x < 4) maze_size_x = 4;
	if (maze_size_y < 4) maze_size_y = 4;
}
/* Initialise the sets. */
void LDMMazeRenderer::init_sets(){
	int i, t, r;

	if(sets)
		delete sets;
	sets = (int *)malloc(maze_size_x*maze_size_y*sizeof(int));


	if(!sets)
	abort();
	for(i = 0; i < maze_size_x*maze_size_y; i++){
		sets[i] = i;
	}
  
	if(hedges)
		free(hedges);
	hedges = (int *)malloc(maze_size_x*maze_size_y*2*sizeof(int));

	if(!hedges)
		abort();
	for(i = 0; i < maze_size_x*maze_size_y*2; i++){
		hedges[i] = i;
	}
	/* Mask out outside walls. */
	for(i = 0; i < maze_size_y; i++){
		hedges[2*((maze_size_x)*i+maze_size_x-1)+1] = -1;
	}
	for(i = 0; i < maze_size_x; i++){
		hedges[2*((maze_size_y-1)*maze_size_x+i)] = -1;
	}

	/* Mask out rooms*/
	for(int i = 0; i < mRooms.size(); i++)
		mask_out_set_rect(mRooms[i].x,mRooms[i].y,mRooms[i].xx-mRooms[i].x,mRooms[i].yy-mRooms[i].y,-1);

	for(i = 0; i < maze_size_x*maze_size_y*2; i++){
		t = hedges[i];
		r = random()%(maze_size_x*maze_size_y*2);
		hedges[i] = hedges[r];
		hedges[r] = t;
	}
}
/* Get the representative of a set. */
int LDMMazeRenderer::get_set(int num){
	int s;

	if(sets[num]==num)
		return num;
	else {
		s = get_set(sets[num]);
		sets[num] = s;
		return s;
	}
}
/* Join two sets together. */
void LDMMazeRenderer::join_sets(int num1, int num2) {
	int s1, s2;

	s1 = get_set(num1);
	s2 = get_set(num2);
  
	if(s1<s2)
		sets[s2] = s1;
	else
		sets[s1] = s2;
}
/* Exitialise the sets. */
void LDMMazeRenderer::exit_sets(){
	if(hedges)
		free(hedges);
	hedges = 0;
	if(sets)
		free(sets);
	sets = 0;
}
void LDMMazeRenderer::set_create_maze(){
	int i, h, xx, yy, dir, v, w;
	
	/* Do almo all the setup. */
	init_sets();

	/* art running through the hedges. */
	for(i = 0; i < 2*maze_size_x*maze_size_y; i++) { 
		h = hedges[i];

		/* This one is in the logo or outside border. */
		if(h==-1)
			continue;

		dir = h%2?1:2;
		xx = (h>>1)%maze_size_x;
		yy = (h>>1)/maze_size_x;

		v = xx;
		w = yy;
		switch(dir){
		case 1:
			v++;
			break;
		case 2:
			w++;
			break;
		}

		if(get_set(xx+yy*maze_size_x)!=get_set(v+w*maze_size_x)){
			join_sets(xx+yy*maze_size_x, v+w*maze_size_x);
			/* Don't draw the wall. */
		}
		else {
			/* Don't join the sets. */
			build_wall(xx, yy, dir); 
		}
	}

	build_maze_border();
	build_all();
	

	/* Free some memory. */
	exit_sets();
}
/* mark a rectangle as being unavailable for usage in the maze */
void LDMMazeRenderer::mask_out_set_rect(int x, int y, int w, int h, int value){
	int xx, yy;
	for(xx = x; xx < x+w; xx++)
		for(yy = y; yy < y+h; yy++){
			hedges[2*(xx+maze_size_x*yy)+1] = value;
			hedges[2*(xx+maze_size_x*yy)] = value;
		}
	for(xx = x; xx < x+w; xx++){
		build_wall(xx, y, 0);
		build_wall(xx, y+h, 0);
		hedges[2*(xx+maze_size_x*(y-1))] = value;
	}
	for(yy = y; yy < y+h; yy++){
		build_wall(x, yy, 3);
		build_wall(x+w, yy, 3);
		hedges[2*(x-1+maze_size_x*yy)+1] = value;
	}
}
/* draws the maze outline */
void LDMMazeRenderer::build_maze_border (){
	for(int i = 0; i < maze_size_x; i++)
		build_wall(i, 0, 0);
	for(int i = 0; i < maze_size_x; i++)
		build_wall(i, maze_size_y, 0);
	for(int i = 0; i < maze_size_y; i++)
		build_wall(0, i, 3);
	for(int i = 0; i < maze_size_y; i++)
		build_wall(maze_size_x, i, 3);
}
void LDMMazeRenderer::build_all(){
	build_3D();
	build_collisions();
}

void LDMMazeRenderer::build_3D(){
	Ogre::Vector3 halfSize = Ogre::Vector3(mSize.x / 2, 0, mSize.z / 2);
	Ogre::Vector3 scale = mSize / Ogre::Vector3(maze_size_x, mSize.y, maze_size_y);
	int numWall = 0;

	Ogre::Entity* corner = mSceneManager->createEntity("CornerWall", "Corner.mesh");
	corner->setMaterialName("WallMaterial");
	Ogre::Entity* wall = mSceneManager->createEntity("Wall", "Wall.mesh");
	wall->setMaterialName("WallMaterial");
	Ogre::Entity* cross = mSceneManager->createEntity("CrossWall", "Cross.mesh");
	cross->setMaterialName("WallMaterial");
	Ogre::Entity* halfWall = mSceneManager->createEntity("HalfWall", "HalfWall.mesh");
	halfWall->setMaterialName("WallMaterial");
	Ogre::Entity* t = mSceneManager->createEntity("TWall", "T.mesh");
	t->setMaterialName("WallMaterial");

	for(int x = 0; x <= maze_size_x; x++){
		for(int y = 0; y <= maze_size_y; y++){
			int intersec = 0;
			if(x < maze_size_x && y < maze_size_y)
				if(maze[x][y] & WALL_TOP)
					intersec |= WALL_RIGHT;
			if(x < maze_size_x && y < maze_size_y)
				if(maze[x][y] & WALL_LEFT)
					intersec |= WALL_BOTTOM;

			if(x == maze_size_x && y < maze_size_y)
				if(maze[x-1][y] & WALL_RIGHT)
					intersec |= WALL_BOTTOM;
			if(y == maze_size_y && x < maze_size_x)
				if(maze[x][y-1] & WALL_BOTTOM)
					intersec |= WALL_RIGHT;
			if(x == maze_size_x && y == maze_size_y){
				if(maze[x-1][y-1] & WALL_RIGHT)
					intersec |= WALL_TOP;				
				if(maze[x-1][y-1] & WALL_BOTTOM)
					intersec |= WALL_LEFT;
			}

			if(x > 0)
				if(maze[x-1][y] & WALL_TOP)
					intersec |= WALL_LEFT;
			if(y > 0)
				if(maze[x][y-1] & WALL_LEFT)
					intersec |= WALL_TOP;

			if(intersec  == (WALL_RIGHT | WALL_BOTTOM))
				mGeometry->addEntity(corner->clone(std::string("Wall #")+Morkidios::Utils::convertIntToString(numWall++)), Ogre::Vector3(x,mSize.y/2,y) * scale - halfSize, Ogre::Quaternion(Ogre::Degree(-90),Ogre::Vector3(0,1,0)), Ogre::Vector3(1,5,1));
			if(intersec == (WALL_LEFT | WALL_BOTTOM))
				mGeometry->addEntity(corner->clone(std::string("Wall #")+Morkidios::Utils::convertIntToString(numWall++)), Ogre::Vector3(x,mSize.y/2,y) * scale - halfSize, Ogre::Quaternion(Ogre::Degree(-180),Ogre::Vector3(0,1,0)), Ogre::Vector3(1,5,1));
			if(intersec  == (WALL_RIGHT | WALL_TOP))
				mGeometry->addEntity(corner->clone(std::string("Wall #")+Morkidios::Utils::convertIntToString(numWall++)), Ogre::Vector3(x,mSize.y/2,y) * scale - halfSize, Ogre::Quaternion(Ogre::Degree(0),Ogre::Vector3(0,1,0)), Ogre::Vector3(1,5,1));
			if(intersec == (WALL_LEFT | WALL_TOP))
				mGeometry->addEntity(corner->clone(std::string("Wall #")+Morkidios::Utils::convertIntToString(numWall++)), Ogre::Vector3(x,mSize.y/2,y) * scale - halfSize, Ogre::Quaternion(Ogre::Degree(90),Ogre::Vector3(0,1,0)), Ogre::Vector3(1,5,1));

			if(intersec == (WALL_LEFT | WALL_RIGHT))
				mGeometry->addEntity(wall->clone(std::string("Wall #")+Morkidios::Utils::convertIntToString(numWall++)), Ogre::Vector3(x,mSize.y/2,y) * scale - halfSize, Ogre::Quaternion(Ogre::Degree(90),Ogre::Vector3(0,1,0)), Ogre::Vector3(1,5,1));
			if(intersec == (WALL_BOTTOM | WALL_TOP))
				mGeometry->addEntity(wall->clone(std::string("Wall #")+Morkidios::Utils::convertIntToString(numWall++)), Ogre::Vector3(x,mSize.y/2,y) * scale - halfSize, Ogre::Quaternion(Ogre::Degree(0),Ogre::Vector3(0,1,0)), Ogre::Vector3(1,5,1));

			if(intersec == (WALL_BOTTOM | WALL_RIGHT | WALL_LEFT))
				mGeometry->addEntity(t->clone(std::string("Wall #")+Morkidios::Utils::convertIntToString(numWall++)), Ogre::Vector3(x,mSize.y/2,y) * scale - halfSize, Ogre::Quaternion(Ogre::Degree(-90),Ogre::Vector3(0,1,0)), Ogre::Vector3(1,5,1));
			if(intersec == (WALL_TOP | WALL_RIGHT | WALL_LEFT))
				mGeometry->addEntity(t->clone(std::string("Wall #")+Morkidios::Utils::convertIntToString(numWall++)), Ogre::Vector3(x,mSize.y/2,y) * scale - halfSize, Ogre::Quaternion(Ogre::Degree(90),Ogre::Vector3(0,1,0)), Ogre::Vector3(1,5,1));
			if(intersec == (WALL_BOTTOM | WALL_TOP | WALL_LEFT))
				mGeometry->addEntity(t->clone(std::string("Wall #")+Morkidios::Utils::convertIntToString(numWall++)), Ogre::Vector3(x,mSize.y/2,y) * scale - halfSize, Ogre::Quaternion(Ogre::Degree(180),Ogre::Vector3(0,1,0)), Ogre::Vector3(1,5,1));
			if(intersec == (WALL_BOTTOM | WALL_TOP | WALL_RIGHT))
				mGeometry->addEntity(t->clone(std::string("Wall #")+Morkidios::Utils::convertIntToString(numWall++)), Ogre::Vector3(x,mSize.y/2,y) * scale - halfSize, Ogre::Quaternion(Ogre::Degree(0),Ogre::Vector3(0,1,0)), Ogre::Vector3(1,5,1));

			if(intersec == (WALL_LEFT | WALL_RIGHT | WALL_TOP | WALL_BOTTOM))
				mGeometry->addEntity(cross->clone(std::string("Wall #")+Morkidios::Utils::convertIntToString(numWall++)), Ogre::Vector3(x,mSize.y/2,y) * scale - halfSize, Ogre::Quaternion(Ogre::Degree(0),Ogre::Vector3(0,1,0)), Ogre::Vector3(1,5,1));

			if(intersec == WALL_LEFT)
				mGeometry->addEntity(halfWall->clone(std::string("Wall #")+Morkidios::Utils::convertIntToString(numWall++)), Ogre::Vector3(x,mSize.y/2,y) * scale - halfSize, Ogre::Quaternion(Ogre::Degree(180),Ogre::Vector3(0,1,0)), Ogre::Vector3(1,5,1));
			if(intersec == WALL_RIGHT)
				mGeometry->addEntity(halfWall->clone(std::string("Wall #")+Morkidios::Utils::convertIntToString(numWall++)), Ogre::Vector3(x,mSize.y/2,y) * scale - halfSize, Ogre::Quaternion(Ogre::Degree(0),Ogre::Vector3(0,1,0)), Ogre::Vector3(1,5,1));
			if(intersec == WALL_TOP)
				mGeometry->addEntity(halfWall->clone(std::string("Wall #")+Morkidios::Utils::convertIntToString(numWall++)), Ogre::Vector3(x,mSize.y/2,y) * scale - halfSize, Ogre::Quaternion(Ogre::Degree(90),Ogre::Vector3(0,1,0)), Ogre::Vector3(1,5,1));
			if(intersec == WALL_BOTTOM)
				mGeometry->addEntity(halfWall->clone(std::string("Wall #")+Morkidios::Utils::convertIntToString(numWall++)), Ogre::Vector3(x,mSize.y/2,y) * scale - halfSize, Ogre::Quaternion(Ogre::Degree(-90),Ogre::Vector3(0,1,0)), Ogre::Vector3(1,5,1));
		}
	}
}
void LDMMazeRenderer::build_collisions(){
	double height = mSize.y;
	btVector3 boxHalfExtents = btVector3(2,height,19.5) / 2;
	btBoxShape* box = new btBoxShape(boxHalfExtents);
	Ogre::Vector3 halfSize = Ogre::Vector3(mSize.x / 2, 0, mSize.z / 2);
	Ogre::Vector3 scale = mSize / Ogre::Vector3(maze_size_x, mSize.y, maze_size_y);

	for(int x = 0; x < maze_size_x; x++){
		for(int y = 0; y < maze_size_y; y++){
			if(maze[x][y] & WALL_TOP){
				btRigidBody* body = new btRigidBody(0, new btDefaultMotionState(), box);
				btTransform transform = btTransform::getIdentity();
				transform.setOrigin(OgreBulletCollisions::OgreBtConverter::to(Ogre::Vector3(x+0.5,height/2,y)*scale - halfSize));
				transform.setRotation(btQuaternion(btVector3(0,1,0),Ogre::Degree(90).valueRadians()));
				body->setWorldTransform(transform);
				mWorld->addRigidBody(body);
			}
			if(maze[x][y] & WALL_LEFT){
				btRigidBody* body = new btRigidBody(0, new btDefaultMotionState(), box);
				btTransform transform = btTransform::getIdentity();
				transform.setOrigin(OgreBulletCollisions::OgreBtConverter::to(Ogre::Vector3(x,height/2,y+0.5)*scale - halfSize));
				transform.setRotation(btQuaternion(btVector3(0,1,0),Ogre::Degree(0).valueRadians()));
				body->setWorldTransform(transform);
				mWorld->addRigidBody(body);
			}
			if(x == maze_size_x-1){
				if(maze[x][y] & WALL_RIGHT){
					btRigidBody* body = new btRigidBody(0, new btDefaultMotionState(), box);
					btTransform transform = btTransform::getIdentity();
					transform.setOrigin(OgreBulletCollisions::OgreBtConverter::to(Ogre::Vector3(x+1,height/2,y+0.5)*scale - halfSize));
					transform.setRotation(btQuaternion(btVector3(0,1,0),Ogre::Degree(0).valueRadians()));
					body->setWorldTransform(transform);
					mWorld->addRigidBody(body);
				}
			}
			if(y == maze_size_y-1){
				if(maze[x][y] & WALL_BOTTOM){
					btRigidBody* body = new btRigidBody(0, new btDefaultMotionState(), box);
					btTransform transform = btTransform::getIdentity();
					transform.setOrigin(OgreBulletCollisions::OgreBtConverter::to(Ogre::Vector3(x,height/2,y+1)*scale - halfSize));
					transform.setRotation(btQuaternion(btVector3(0,1,0),Ogre::Degree(90).valueRadians()));
					body->setWorldTransform(transform);
					mWorld->addRigidBody(body);
				}
			}
		}
	}
}


// Construction methodes
LDMMazeRenderer::LDMMazeRenderer() : mMazeSizeX(1025), mMazeSizeY(1025){
	maze_size_x = 0;
	maze_size_y = 0;
	start_x = 0;
	start_y = 0;
	start_dir = 0;
	end_x = mMazeSizeX;
	end_y = mMazeSizeX;
	end_dir = 0;
	grid_width = 100;
	grid_height = 100;
	bw = 0;

	x = 0;
	y = 0;
	
	sets = NULL;
	hedges = NULL;
	
	maze = new unsigned short*[MAX_MAZE_SIZE_X];
	for(int i = 0; i < MAX_MAZE_SIZE_X; i++){
		maze[i] = new unsigned short[MAX_MAZE_SIZE_Y];
		for(int j = 0; j < MAX_MAZE_SIZE_Y; j++)
			maze[i][j] = 0;
	}

	mIsInitialized = false;

	mImage = Ogre::Image();
}
LDMMazeRenderer::LDMMazeRenderer(int xs, int ys) : mMazeSizeX(xs), mMazeSizeY(ys){
	maze_size_x = 0;
	maze_size_y = 0;
	start_x = 0;
	start_y = 0;
	start_dir = 0;
	end_x = mMazeSizeX;
	end_y = mMazeSizeX;
	end_dir = 0;
	grid_width = 100;
	grid_height = 100;
	bw = 0;

	x = 0;
	y = 0;
	
	sets = NULL;
	hedges = NULL;
	
	maze = new unsigned short*[MAX_MAZE_SIZE_X];
	for(int i = 0; i < MAX_MAZE_SIZE_X; i++)
		maze[i] = new unsigned short[MAX_MAZE_SIZE_Y];

	mIsInitialized = false;

	mImage = Ogre::Image();
}
void LDMMazeRenderer::init(btDynamicsWorld* world, Ogre::StaticGeometry* sg, Ogre::SceneManager* smgr, Ogre::Vector3 size) {
	mSize = size;

	mGeometry = sg;
	mWorld = world;
	mSceneManager = smgr;

	grid_width = grid_height = 50;
	
	x = 0;
	y = 0;

	set_maze_sizes (mMazeSizeX, mMazeSizeY);

	Ogre::uchar* pixels = new Ogre::uchar[mMazeSizeX * mMazeSizeX * 4];
	// set R=0, G=0, B=0, A=255
	for(size_t p = 0; p < (mMazeSizeX * mMazeSizeX * 4); p ++){
		pixels[p] = (p % 4 == 0 && p > 0) ? 255 : 0;
	}
	mImage.loadDynamicImage(pixels, mMazeSizeX, mMazeSizeX, 1, Ogre::PF_R8G8B8A8);
}
void LDMMazeRenderer::run(){
	set_create_maze();
	mIsInitialized = true;
}

// Return value methodes
Ogre::Image LDMMazeRenderer::getImage(){
	if(mIsInitialized)
		return mImage;
	else {
		std::cerr << "Maze not initialized for Maze::init()";
		exit(1);
	}
}
Ogre::Vector2 LDMMazeRenderer::getMazeSize(){
	return Ogre::Vector2(maze_size_x, maze_size_y);
}

// Variouse methodes
void LDMMazeRenderer::createRoom(int x, int y, int xx, int yy, int numDoors){
	if(mIsInitialized){
		std::cout << "Warning : You create a room but maze is initialized\n";
		return;
	}
	if(x < 0 || x >= maze_size_x || xx < 0 || xx>=maze_size_x || y < 0 || y >= maze_size_y || yy < 0 || yy >= maze_size_y){
		std::cerr << "Parameters for MazerRenderer::createRoom are not valid. x : " << x << "  y : " << y << "  xx : " << xx << "  yy : " << yy << "  maze_size_x : " << maze_size_x << "  maze_size_y : " << maze_size_y <<  std::endl;
		exit(1);
	}

	Room r;
	r.x = x;
	r.y = y;
	r.xx = xx;
	r.yy = yy;

	for(int i = 0; i < numDoors; i++){
		srand(clock());
		int side = rand() % 4;

		int posx, posy;

		switch(side){
		case 0:		// UP
			posx = x + (rand()%(xx-x));

			if((maze[posx][y] & DOOR_TOP) != 0)
				i--;				// If there is arlready a door, do it again

			maze[posx][y] |= DOOR_TOP;
			if(y - 1 != 0)
				maze[posx][y-1] |= DOOR_BOTTOM;

			break;
		case 1:		// DOWN			
			posx = x + (rand()%(xx-x));

			if((maze[posx][yy-1] & DOOR_BOTTOM) != 0)
				i--;				// If there is arlready a door, do it again

			maze[posx][yy-1] |= DOOR_BOTTOM;
			maze[posx][yy] |= DOOR_TOP;

			break;
		case 2:		// LEFT
			posy = y + (rand()%(yy-y));

			if((maze[x][posy] & DOOR_LEFT) != 0)
				i--;				// If there is arlready a door, do it again

			maze[x][posy] |= DOOR_LEFT;
			if(x - 1 != 0)
				maze[x-1][posy] |= DOOR_RIGHT;

			break;
		case 3:		// RIGHT
			posy = y + (rand()%(yy-y));

			if((maze[xx-1][posy] & DOOR_RIGHT) != 0)
				i--;				// If there is arlready a door, do it again

			maze[xx-1][posy] |= DOOR_RIGHT;
			maze[xx][posy] |= DOOR_LEFT;

			break;
		}
	}

	mRooms.push_back(r);
}
