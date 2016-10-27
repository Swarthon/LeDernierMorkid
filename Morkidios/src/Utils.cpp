#include "Utils.h"

namespace Morkidios {

	// C++
	std::string Utils::convertIntToString(int i){
		std::ostringstream os;
		os << i;
		return os.str();
	}

	// Ogre
	bool Utils::onLeft(Ogre::Vector3 start, Ogre::Vector3 end){
		return Ogre::Vector3(-start.z, 0, start.x).distance(end) < Ogre::Vector3(start.z, 0, -start.x).distance(end);
	}
	bool Utils::onRight(Ogre::Vector3 start, Ogre::Vector3 end){
		return Ogre::Vector3(-start.z, 0, start.x).distance(end) > Ogre::Vector3(start.z, 0, -start.x).distance(end);
	}
	Ogre::Vector3 Utils::getLeft(Ogre::Vector3 start){
		return Ogre::Vector3(-start.z, 0, start.x);
	}
	Ogre::Vector3 Utils::getRight(Ogre::Vector3 start){
		return Ogre::Vector3(start.z, 0, -start.x);
	}
	void Utils::destroyAllAttachedMovableObjects(Ogre::SceneNode* node){
		if(!node) return;

		// Destroy all the attached objects
		Ogre::SceneNode::ObjectIterator itObject = node->getAttachedObjectIterator();

		while (itObject.hasMoreElements())
			node->getCreator()->destroyMovableObject(itObject.getNext());

		// Recurse to child SceneNodes
		Ogre::SceneNode::ChildNodeIterator itChild = node->getChildIterator();

		while (itChild.hasMoreElements()){
			Ogre::SceneNode* pChildNode = static_cast<Ogre::SceneNode*>(itChild.getNext());
			destroyAllAttachedMovableObjects(pChildNode);
		}
	}

	// Image
	void Utils::drawLine(Ogre::Image& img, Ogre::ColourValue c, int x, int y, int xx, int yy){
		// Horizontal line
		if(y == yy)
			for(int x_ = x; x_ <= xx; x_++)
				img.setColourAt(c, x_, y, 0);

		// Vertical line
		if(x == xx)
			for(int y_ = y; y_ <= yy; y_++)
				img.setColourAt(c, x, y_, 0);
	}
	void Utils::drawLine(Ogre::Image& img, Ogre::ColourValue c, int x, int y, int xx, int yy, int pixelSize){
		if(y == yy)
			for(int i = 0; i < pixelSize; i++)
				drawLine(img, c, x, y + i, xx + pixelSize - 1, yy + i);
		if(x == xx)
			for(int i = 0; i < pixelSize; i++)
				drawLine(img, c, x + i, y, xx + i, yy + pixelSize - 1);
	}

	// 3D
	Ogre::Entity* Utils::drawCube(Ogre::SceneManager* smgr, Ogre::Vector3 size, std::string materialName, double widthTextureBegin, double widthTextureEnd, double heightTextureBegin, double heightTextureEnd, double deepTextureBegin, double deepTextureEnd, std::string name){
		Ogre::ManualObject* mo;
		if(name == std::string()){
			static int numCube = 0;
			std::ostringstream os;
			os << numCube;
			mo = new Ogre::ManualObject(std::string("Cube") + os.str());
			numCube++;
		}
		else
			mo = new Ogre::ManualObject(name);

		mo->begin(materialName,  Ogre::RenderOperation::OT_TRIANGLE_LIST);

		size /= 2;		// To make the center at the midle of the cube

		// Down face
		mo->position(-size);				mo->textureCoord(widthTextureBegin, deepTextureBegin);	// 0
		mo->position(size.x, -size.y, -size.z);		mo->textureCoord(widthTextureEnd,deepTextureBegin);	// 1
		mo->position(-size.x, -size.y, size.z);		mo->textureCoord(widthTextureBegin,deepTextureEnd);	// 2
		mo->position(size.x, -size.y, size.z);		mo->textureCoord(widthTextureEnd,deepTextureEnd);	// 3
		mo->triangle(0,1,3);
		mo->triangle(0,3,2);

		// Up face
		mo->position(-size.x, size.y, -size.z);		mo->textureCoord(widthTextureBegin, deepTextureBegin);	// 4
		mo->position(size.x, size.y, -size.z);		mo->textureCoord(widthTextureEnd,deepTextureBegin);	// 5
		mo->position(-size.x, size.y, size.z);		mo->textureCoord(widthTextureBegin,deepTextureEnd);	// 6
		mo->position(size);				mo->textureCoord(widthTextureEnd,deepTextureEnd);	// 7
		mo->triangle(4,7,5);
		mo->triangle(4,6,7);

		// Left face
		mo->position(-size.x, size.y, -size.z);		mo->textureCoord(deepTextureBegin, heightTextureEnd);	// 8
		mo->position(-size.x, size.y, size.z);		mo->textureCoord(deepTextureEnd,heightTextureEnd);	// 9
		mo->position(-size);				mo->textureCoord(deepTextureBegin,heightTextureBegin);	// 10
		mo->position(-size.x, -size.y, size.z);		mo->textureCoord(deepTextureEnd,heightTextureBegin);	// 11
		mo->triangle(8,11,9);
		mo->triangle(8,10,11);

		// Right face
		mo->position(size.x, size.y, -size.z);		mo->textureCoord(deepTextureBegin, heightTextureEnd);	// 12
		mo->position(size);				mo->textureCoord(deepTextureEnd,heightTextureEnd);	// 13
		mo->position(size.x, -size.y, -size.z);		mo->textureCoord(deepTextureBegin,heightTextureBegin);	// 14
		mo->position(size.x, -size.y, size.z);		mo->textureCoord(deepTextureEnd,heightTextureBegin);	// 15
		mo->triangle(12,13,15);
		mo->triangle(12,15,14);

		// Front face
		mo->position(-size.x, size.y, -size.z);		mo->textureCoord(widthTextureBegin,heightTextureEnd);	// 16
		mo->position(size.x, size.y, -size.z);		mo->textureCoord(widthTextureEnd,heightTextureEnd);	// 17
		mo->position(-size);				mo->textureCoord(widthTextureBegin,heightTextureBegin);	// 18
		mo->position(size.x, -size.y, -size.z);		mo->textureCoord(widthTextureEnd,heightTextureBegin);	// 19
		mo->triangle(16,17,19);
		mo->triangle(16,19,18);

		// Back face
		mo->position(-size.x, size.y, size.z);		mo->textureCoord(widthTextureBegin,heightTextureEnd);	// 22
		mo->position(size);				mo->textureCoord(widthTextureEnd,heightTextureEnd);	// 23
		mo->position(-size.x, -size.y, size.z);		mo->textureCoord(widthTextureBegin,heightTextureBegin);	// 20
		mo->position(size.x, -size.y, size.z);		mo->textureCoord(widthTextureEnd,heightTextureBegin);	// 21
		mo->triangle(20,23,21);
		mo->triangle(20,22,23);

		mo->end();

		mo->convertToMesh(mo->getName());
		return smgr->createEntity(mo->getName());
	}
	Ogre::Entity* Utils::drawLine(Ogre::SceneManager* smgr, Ogre::Vector3 start, Ogre::Vector3 end, std::string materialName, std::string name){
		Ogre::ManualObject* mo;
		if(name == std::string()){
			static int numCube = 0;
			std::ostringstream os;
			os << numCube;
			mo = new Ogre::ManualObject(std::string("Line") + os.str());
			numCube++;
		}
		else
			mo = new Ogre::ManualObject(name);
		Ogre::MaterialPtr ma = Ogre::MaterialManager::getSingleton().create(name,materialName);
		ma->setReceiveShadows(false);
		ma->getTechnique(0)->setLightingEnabled(true);
		ma->getTechnique(0)->getPass(0)->setDiffuse(0,0,1,0);
		ma->getTechnique(0)->getPass(0)->setAmbient(0,0,1);
		ma->getTechnique(0)->getPass(0)->setSelfIllumination(0,0,1);


		mo->begin(name, Ogre::RenderOperation::OT_LINE_LIST);
		mo->position(start);
		mo->position(end);
		mo->end();

		mo->convertToMesh(mo->getName());

		return smgr->createEntity(mo->getName());
	}

	// Collision
	btVector3 Utils::getBoundingBox(btCollisionShape* shape){
		btVector3 min, max;
		shape->getAabb(btTransform::getIdentity(), min, max);
		return max - min;
	}
	btRigidBody* Utils::addCube(Ogre::Vector3 pos, Ogre::Vector3 size){
		btVector3 boxHalfExtents = OgreBulletCollisions::OgreBtConverter::to(size/2);
		btBoxShape* box = new btBoxShape(boxHalfExtents);

		btRigidBody* newBody = new btRigidBody(0, new btDefaultMotionState(), box);
		btTransform transform = newBody->getWorldTransform();
		transform.setOrigin(OgreBulletCollisions::OgreBtConverter::to(pos));
		newBody->setWorldTransform(transform);

		return newBody;
	}

	// OIS & CEGUI
	CEGUI::MouseButton Utils::convertButton(OIS::MouseButtonID buttonID){
		switch (buttonID){
		case OIS::MB_Left:
			return CEGUI::LeftButton;
		case OIS::MB_Right:
			return CEGUI::RightButton;
		case OIS::MB_Middle:
			return CEGUI::MiddleButton;
		default:
			return CEGUI::LeftButton;
		}
	}

	// CEGUI
	CEGUI::BasicImage* Utils::addImageToImageset(CEGUI::Texture& tex, std::string name){
		const CEGUI::Rectf rect(CEGUI::Vector2f(0.0f, 0.0f), tex.getOriginalDataSize());
		CEGUI::BasicImage* image = (CEGUI::BasicImage*)( &CEGUI::ImageManager::getSingleton().create("BasicImage", name));
		image->setTexture(&tex);
		image->setArea(rect);
		image->setAutoScaled(CEGUI::ASM_Both);

		return image;
	}

	// Various
		// Screen
#ifdef _WIN32
	void Utils::winGetScreenSize(int& h, int& v){
		RECT desktop;
		const HWND hDesktop = GetDesktopWindow();
		GetWindowRect(hDesktop, &desktop);
		h = desktop.right;
		v = desktop.bottom;
	}
#endif
}
