#ifndef _GAMEENTITY_H_
#define _GAMEENTITY_H_

#include <OgreQuaternion.h>
#include <OgreStringVector.h>
#include <OgreVector3.h>

#include <BulletCollision/CollisionDispatch/btCollisionObject.h>

namespace Common {
#define NUM_GAME_ENTITY_BUFFERS 4

	/**
	 * @enum MovableObjectType
	 * @ingroup Common
	 * @brief
	 *	Type of Movable Object (Graphics)
	 */
	enum MovableObjectType {
		MoTypeItem,
		MoTypeEntity,
		NumMovableObjectType
	};
	/**
	 * @enum CollisionObjectType
	 * @ingroup Common
	 * @brief
	 *	Type of Collision Object (Collision)
	 */
	enum CollisionObjectType {
		CoRigidBody,
		CoGhostObject,
		CoSoftBody,
		NumCollisionObjectType
	};

	/**
	 * @struct MovableObjectDefinition
	 * @ingroup Common
	 * @brief
	 *	Struct defining Movable Object
	 */
	struct MovableObjectDefinition {
		Ogre::String       meshName;
		Ogre::String       resourceGroup;
		Ogre::StringVector submeshMaterials;
		MovableObjectType  moType;
	};
	/**
	 * @struct CollisionObjectDefinition
	 * @ingroup Common
	 * @brief
	 *	Struct defining Collision Object
	 */
	struct CollisionObjectDefinition {
		unsigned int        mass;
		unsigned int        restitution;
		unsigned int        friction;
		CollisionObjectType coType;
		btCollisionShape*   shape;
	};

	/**
	 * @struct GameEntityTransform
	 * @ingroup Common
	 * @brief
	 *	Struct defining the transform available for GameEntity
	 */
	struct GameEntityTransform {
		Ogre::Vector3    vPos;
		Ogre::Quaternion qRot;
		Ogre::Vector3    vScale;
	};

	/**
	 * @struct GameEntity
	 * @ingroup Common
	 * @brief
	 *	Struct defining Game Entity
	 */
	struct GameEntity {
	private:
		/// Unique ID
		Ogre::uint32 mId;

	public:
		//----------------------------------------
		// Only used by Graphics thread
		//----------------------------------------
		/// SceneNode
		Ogre::SceneNode*     mSceneNode;
		/// MovableObject
		Ogre::MovableObject* mMovableObject; // Could be Entity, InstancedEntity, Item.

		//----------------------------------------
		// Used by Logic thread
		//----------------------------------------
		/// CollisionObject
		btCollisionObject* mCollisionObject;

		//----------------------------------------
		// Used by both Logic and Graphics threads
		//----------------------------------------
		/// Actual transform of the GameEntity
		GameEntityTransform*      mTransform[NUM_GAME_ENTITY_BUFFERS];
		/// Type of SceneManager used
		Ogre::SceneMemoryMgrTypes mType;

		//----------------------------------------
		// Read-only
		//----------------------------------------
		/// Definition of the MovableObject
		MovableObjectDefinition const*   mMoDefinition;
		/// Definition of the CollisionObject
		CollisionObjectDefinition const* mCoDefinition;
		/// ID of the transform's buffer
		size_t                           mTransformBufferIdx;

		/**
		 * Constructor
		 * @param id
 		 *	Unique ID to provide
		 * @param moDefinition
 		 *	MovableObjectDefinition describing Graphics items
		 * @param coDefinition
 		 *	CollisionObjectDefinition describing Collision item
		 * @param type
 		 *	Type of SceneManager's Memory used (as Ogre::SCENE_DYNAMIC)
		 */
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

		/// Get mId
		Ogre::uint32 getId(void) const { return mId; }
		/**
		 * Compare two GameEntity
		 * @param _r
 		 *	Second GameEntity to compare
		 * @return
 		 *	Returns whether _r->mId is lower than this->mId
		 */
		bool operator<(const GameEntity* _r) const { return mId < _r->mId; }
	};

	typedef std::vector<GameEntity*> GameEntityVec;
}

#endif // _GAMEENTITY_H_
