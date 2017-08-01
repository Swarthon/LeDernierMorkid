#include "Terrain/CollisionTerrain.h"

#include "Converter.h"

#include <OgreImage.h>

#include <BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h>
#include <BulletDynamics/Dynamics/btDynamicsWorld.h>
#include <BulletDynamics/Dynamics/btRigidBody.h>
#include <LinearMath/btDefaultMotionState.h>

//-----------------------------------------------------------------------------
CollisionTerrain::CollisionTerrain(btDynamicsWorld* world)
                : mWorld(world) {
}
//-------------------------------------------------------------------------------------------------
void CollisionTerrain::load(const Ogre::String&  texName,
                            const Ogre::Vector3  center,
                            const Ogre::Vector3& dimensions) {
	Ogre::Image image = loadImage(texName);
	Terrain::load(image, center, dimensions);
	createShape();
}
//-------------------------------------------------------------------------------------------------
void CollisionTerrain::createShape() {
	btScalar  heightScale = 1.f;
	btVector3 localScaling(1, heightScale, 1);

	btHeightfieldTerrainShape* terrainShape = new btHeightfieldTerrainShape(mWidth, mDepth, mHeightMap.data(), 1, 0, mHeight, 1, PHY_FLOAT, true);
	double                     scale        = mXZDimensions.x / (mWidth - 1);
	terrainShape->setLocalScaling(btVector3(scale, 1, scale));

	btRigidBody* body = new btRigidBody(0, new btDefaultMotionState(), terrainShape);
	body->setFriction(0.8f);
	body->setHitFraction(0.8f);
	body->setRestitution(0.6f);
	body->getWorldTransform().setOrigin(btVector3(0, mTerrainOrigin.y, 0));
	//	body->getWorldTransform().setRotation(btQuaternion(btVector3(0,1,0),Ogre::Degree(90).valueRadians()));
	body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);

	mWorld->addRigidBody(body);
}
