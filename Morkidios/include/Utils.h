#ifndef OUTILS_H
#define OUTILS_H

#include <MorkidiosPrerequisites.h>

namespace Morkidios {

	class _MorkidiosExport Utils{
	public:
		// C++
		static std::string convertIntToString(int i);

		// Ogre
		static bool onLeft(Ogre::Vector3 start, Ogre::Vector3 end);
		static bool onRight(Ogre::Vector3 start, Ogre::Vector3 end);
		static Ogre::Vector3 getLeft(Ogre::Vector3 start);
		static Ogre::Vector3 getRight(Ogre::Vector3 start);
		static void destroyAllAttachedMovableObjects(Ogre::SceneNode* node);
		static Ogre::Entity* merge(std::vector<Ogre::MeshPtr> meshes, Ogre::SceneManager* smgr);

		// Image
		static void drawLine(Ogre::Image& img, Ogre::ColourValue c, int x, int y, int xx, int yy);
		static void drawLine(Ogre::Image& img, Ogre::ColourValue c, int x, int y, int xx, int yy, int pixelSize);

		// 3D
		static Ogre::Entity* drawCube(Ogre::SceneManager* smgr, Ogre::Vector3 size, std::string materialName, double widthTextureBegin, double widthTextureEnd, double heightTextureBegin, double heightTextureEnd, double deepTextureBegin, double deepTextureEnd, std::string name = std::string());
		static Ogre::Entity* drawLine(Ogre::SceneManager* smgr, Ogre::Vector3 start, Ogre::Vector3 end, std::string materialName, std::string name = std::string());

#ifndef NO_COLLISION
		// Collision
		static btVector3 getBoundingBox(btCollisionShape* shape);
		static btRigidBody* addCube(Ogre::Vector3 pos, Ogre::Vector3 size);
#endif // NO_COLLISION

		// OIS & CEGUI
		static CEGUI::MouseButton convertButton(OIS::MouseButtonID buttonID);

		// CEGUI
		static CEGUI::BasicImage* addImageToImageset(CEGUI::Texture& tex, std::string name);

		// Various
			// Screen
#ifdef _WIN32
		static void winGetScreenSize(int& horizontal, int& vertical);
#endif
	private:
		Utils();
	};

}

#endif // OUTILS_H
