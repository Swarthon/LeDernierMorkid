// TinyXML includes
#ifndef TIXML_USE_STL
#define TIXML_USE_STL
#endif
#include <tinyxml.h>

// Ogre includes
#include <OGRE/Ogre.h>
#include <OgrePixelFormat.h>

// CEGUI includes
#include <CEGUI/CEGUI.h>
#include <CEGUI/RendererModules/Ogre/Renderer.h>

// OIS includes
#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

// C++ includes
#include <string>
#include <iostream>
#include <sstream>
#include <ctime>
#include <vector>
#include <cstdlib>
#include <map>

// C includes
#include <libintl.h>
#include <locale.h>

// Bullet includes
#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <BulletDynamics/Character/btKinematicCharacterController.h>

// OgreBullet includes
#include <OgreBulletDynamicsRigidBody.h>
#include <Shapes/OgreBulletCollisionsCapsuleShape.h>
#include <Shapes/OgreBulletCollisionsConvexHullShape.h>
#include <Shapes/OgreBulletCollisionsBoxShape.h>
#include <Shapes/OgreBulletCollisionsGImpactShape.h>
#include <Shapes/OgreBulletCollisionsCompoundShape.h>
#include <Utils/OgreBulletCollisionsMeshToShapeConverter.h>

// Directions defines
#define LEFT 		1
#define RIGHT		2
#define FORWARD		4
#define BACKWARD	8
#define UP		16
#define RUN		32

#define _(STRING) gettext(STRING)
