#ifndef OUTILS_H
#define OUTILS_H

// Ogre includes
#include <Ogre.h>
#include <OgrePixelFormat.h>

// Bullet includes
#include <btBulletCollisionCommon.h>
#include <BulletCollision/CollisionShapes/btBoxShape.h>
#include <BulletDynamics/Dynamics/btDynamicsWorld.h>
#include <BulletDynamics/Dynamics/btRigidBody.h>

// OgreBullet includes
#include <OgreBulletCollisions.h>
#include <Utils/OgreBulletConverter.h>

// CEGUI includes
#include <CEGUI/CEGUI.h>
#include <CEGUI/RendererModules/Ogre/Renderer.h>

// OIS includes
#include <OIS/OIS.h>

// C++ includes
#include <string>
#include <sstream>


// Defines
#define LEFT 		0b1
#define RIGHT		0b10
#define FORWARD		0b100
#define BACKWARD	0b1000
#define UP		0b10000



namespace Morkidios {

	class Utils{
	public:
		// Structs
		struct CubeConstructionInfo{
			Ogre::SceneManager* smgr = NULL;
			Ogre::StaticGeometry* geometry = NULL;
			btDynamicsWorld* world = NULL;
			std::string materialName;
			std::string name;
			Ogre::Vector3 size = Ogre::Vector3(1,1,1);
			Ogre::Vector3 pos = Ogre::Vector3(0,0,0);
			double widthTextureBegin = 0, widthTextureEnd = 1;
			double heightTextureBegin = 0, heightTextureEnd = 1;
			double deepTextureBegin = 0, deepTextureEnd = 1;
		};

		// C++
		static std::string convertIntToString(int i);

		// Ogre
		static void destroyAllAttachedMovableObjects(Ogre::SceneNode* node);
		static Ogre::Entity* merge(std::vector<Ogre::MeshPtr> meshes, Ogre::SceneManager* smgr);

		// Image		
		static void drawLine(Ogre::Image& img, Ogre::ColourValue c, int x, int y, int xx, int yy);
		static void drawLine(Ogre::Image& img, Ogre::ColourValue c, int x, int y, int xx, int yy, int pixelSize);

		// 3D
		static Ogre::Entity* drawCube(Ogre::SceneManager* smgr, Ogre::Vector3 size, std::string materialName, double widthTextureBegin, double widthTextureEnd, double heightTextureBegin, double heightTextureEnd, double deepTextureBegin, double deepTextureEnd, std::string name = std::string());
		static Ogre::Entity* drawLine(Ogre::SceneManager* smgr, Ogre::Vector3 start, Ogre::Vector3 end, std::string materialName, std::string name = std::string());
		static void addCubeToStaticGeometry(Ogre::SceneManager* smgr, btDynamicsWorld* world, Ogre::StaticGeometry* sg, Ogre::Vector3 pos, Ogre::Vector3 size, std::string materialName, double widthTextureBegin, double widthTextureEnd, double heightTextureBegin, double heightTextureEnd, double deepTextureBegin, double deepTextureEnd, std::string name = std::string());
		static void addCubeToStaticGeometry(CubeConstructionInfo c);

		// Collision
		static btVector3 getBoundingBox(btCollisionShape* shape);
		static btRigidBody* addCube(Ogre::Vector3 pos, Ogre::Vector3 size);

		// OIS & CEGUI		
		static CEGUI::MouseButton convertButton(OIS::MouseButtonID buttonID);

		// CEGUI
		static CEGUI::BasicImage* addImageToImageset(CEGUI::Texture& tex, std::string name);
	private:
		Utils();
	};

}

#endif // OUTILS_H