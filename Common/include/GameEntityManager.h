#ifndef _GAMEENTITYMANAGER_H_
#define _GAMEENTITYMANAGER_H_

#include "GameEntity.h"
#include "Threading/MessageQueueSystem.h"

namespace Common {
	class GraphicsSystem;
	class LogicSystem;

	/**
	 * @class GameEntityManager
	 * @ingroup Common
	 * @brief
	 * 	Class managing GameEntity
	 */
	class GameEntityManager {
	public:
		/**
		 * @struct CreatedGameEntity
		 * @ingroup Common
		 * @brief
		 *	Struct describing a GameEntity and its GameEntityTransform
		 */
		struct CreatedGameEntity {
			GameEntity*         gameEntity;
			GameEntityTransform initialTransform;
		};

		typedef std::vector<GameEntityVec> GameEntityVecVec;

	private:
		struct Region {
			size_t slotOffset;
			size_t count;
			size_t bufferIdx;

			Region(size_t _slotOffset, size_t _count, size_t _bufferIdx)
			                : slotOffset(_slotOffset), count(_count), bufferIdx(_bufferIdx) {}
		};

		/// Current Id for GameEntity creating
		Ogre::uint32  mCurrentId;
		/// Created GameEntity
		GameEntityVec mGameEntities[Ogre::NUM_SCENE_MEMORY_MANAGER_TYPES];

		std::vector<GameEntityTransform*> mTransformBuffers;
		std::vector<Region>               mAvailableTransforms;

		/// GameEntities waiting for removal
		GameEntityVecVec    mScheduledForRemoval;
		size_t              mScheduledForRemovalCurrentSlot;
		std::vector<size_t> mScheduledForRemovalAvailableSlots;

		/// GraphicsSystem managing Graphics
		Mq::MessageQueueSystem* mGraphicsSystem;
		/// LogicSystem creating GameEntity
		LogicSystem*            mLogicSystem;

		Ogre::uint32 getScheduledForRemovalAvailableSlot(void);
		void destroyAllGameEntitiesIn(GameEntityVec& container);

		void aquireTransformSlot(size_t& outSlot, size_t& outBufferIdx);
		void releaseTransformSlot(size_t bufferIdx, GameEntityTransform* transform);

	public:
		/**
		 * Constructor
		 * @param graphicsSystem
		 * 	GraphicsSystem managing Graphics
		 * @param logicSystem
		 *	LogicSystem which create the GameEntity
		 */
		GameEntityManager(Mq::MessageQueueSystem* graphicsSystem, LogicSystem* logicSystem);
		/**
		 * Destructor
		 */
		~GameEntityManager();

		/**
		 * Creates a GameEntity, adding it to the world, and scheduling for the Graphics thread to create the appropiate SceneNode and Item pointers. MUST BE CALLED FROM LOGIC THREAD.
	    	 * @param type
		 *	Whether this GameEntity is dynamic (going to change transform frequently), or static (will move/rotate/scale very, very infrequently)
		 * @param moDefinition
		 *	Definition of the MovableObject (Graphics)
		 * @param coDefinition
		 *	Definition of the CollisionObject (Collisions)
	    	 * @param initialPos
		 *	Starting position of the GameEntity
	    	 * @param initialRot
		 *	Starting orientation of the GameEntity
        	 * @param initialScale
		 *	Starting scale of the GameEntity
        	 * @return
		 *	Pointer of GameEntity ready to be used by the Logic thread. Take in mind not all of its pointers may filled yet (the ones that are not meant to be used by the logic thread)
	    	 */
		GameEntity* addGameEntity(Ogre::SceneMemoryMgrTypes        type,
		                          const MovableObjectDefinition*   moDefinition,
		                          const CollisionObjectDefinition* coDefinition,
		                          const Ogre::Vector3&             initialPos,
		                          const Ogre::Quaternion&          initialRot,
		                          const Ogre::Vector3&             initialScale);

		/**
		 * Removes the GameEntity from the world. The pointer is not immediately destroyed, we first need to release data in other threads (i.e. Graphics).
		 * It will be destroyed after the Render thread confirms it is done with it
		 * (via a Mq::GAME_ENTITY_SCHEDULED_FOR_REMOVAL_SLOT message)
		 * @param toRemove
		 *	GameEntity to remove
		 */
		void removeGameEntity( GameEntity *toRemove );

		/**
		 * Notify the GameSystem that a GameEntity has been removed
		 * @remarks
		 *	Must be called by LogicSystem when Mq::GAME_ENTITY_SCHEDULED_FOR_REMOVAL_SLOT message arrives
		 * @param
		 * 	Slot of the GameEntity to remove
		 */
		void _notifyGameEntitiesRemoved( size_t slot );

		/// Must be called every frame from the LOGIC THREAD.
		void finishFrameParallel(void);
	};
}

#endif // _GAMEENTITYMANAGER_H_
