#ifndef _GAMEENTITY_H_
#define _GAMEENTITY_H_

#include <OgreQuaternion.h>
#include <OgreStringVector.h>
#include <OgreVector3.h>

#include <BulletCollision/CollisionDispatch/btCollisionObject.h>

namespace Common {
#define NUM_GAME_ENTITY_BUFFERS 4

	enum MovableObjectType {
		MoTypeItem,
		MoTypeEntity,
		NumMovableObjectType
	};
	enum CollisionObjectType {
		CoRigidBody,
		CoGhostObject,
		CoSoftBody,
		NumCollisionObjectType
	};

	struct MovableObjectDefinition {
		Ogre::String       meshName;
		Ogre::String       resourceGroup;
		Ogre::StringVector submeshMaterials;
		MovableObjectType  moType;
	};
	struct CollisionObjectDefinition {
		unsigned int        mass;
		unsigned int        restitution;
		unsigned int        friction;
		CollisionObjectType coType;
		btCollisionShape*   shape;
	};

	struct GameEntityTransform {
		Ogre::Vector3    vPos;
		Ogre::Quaternion qRot;
		Ogre::Vector3    vScale;
	};

	struct GameEntity {
	private:
		Ogre::uint32 mId;

	public:
		//----------------------------------------
		// Only used by Graphics thread
		//----------------------------------------
		Ogre::SceneNode*     mSceneNode;
		Ogre::MovableObject* mMovableObject; // Could be Entity, InstancedEntity, Item.

		//----------------------------------------
		// Used by Logic thread
		//----------------------------------------
		btCollisionObject* mCollisionObject;

		//----------------------------------------
		// Used by both Logic and Graphics threads
		//----------------------------------------
		GameEntityTransform*      mTransform[NUM_GAME_ENTITY_BUFFERS];
		Ogre::SceneMemoryMgrTypes mType;

		//----------------------------------------
		// Read-only
		//----------------------------------------
		MovableObjectDefinition const*   mMoDefinition;
		CollisionObjectDefinition const* mCoDefinition;
		size_t                           mTransformBufferIdx;

		GameEntity(Ogre::uint32                     id,
		           const MovableObjectDefinition*   moDefinition,
		           const CollisionObjectDefinition* coDefinition,
		           Ogre::SceneMemoryMgrTypes        type)
		                : mId(id),
		                  mSceneNode(0),
		                  mMovableObject(0),
		                  mType(type),
		                  mMoDefinition(moDefinition),
		                  mCoDefinition(coDefinition),
		                  mTransformBufferIdx(0) {
			for (int i            = 0; i < NUM_GAME_ENTITY_BUFFERS; ++i)
				mTransform[i] = 0;
		}

		Ogre::uint32 getId(void) const { return mId; }
		bool operator<(const GameEntity* _r) const { return mId < _r->mId; }
	};

	typedef std::vector<GameEntity*> GameEntityVec;
}

#endif // _GAMEENTITY_H_
