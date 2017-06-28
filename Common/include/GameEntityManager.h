#ifndef _GAMEENTITYMANAGER_H_
#define _GAMEENTITYMANAGER_H_

#include "GameEntity.h"
#include "Threading/MessageQueueSystem.h"

namespace Common {
	class GraphicsSystem;
	class LogicSystem;

	class GameEntityManager {
	public:
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

		Ogre::uint32  mCurrentId;
		GameEntityVec mGameEntities[Ogre::NUM_SCENE_MEMORY_MANAGER_TYPES];

		std::vector<GameEntityTransform*> mTransformBuffers;
		std::vector<Region>               mAvailableTransforms;

		GameEntityVecVec    mScheduledForRemoval;
		size_t              mScheduledForRemovalCurrentSlot;
		std::vector<size_t> mScheduledForRemovalAvailableSlots;

		Mq::MessageQueueSystem* mGraphicsSystem;
		LogicSystem*            mLogicSystem;

		Ogre::uint32 getScheduledForRemovalAvailableSlot(void);
		void destroyAllGameEntitiesIn(GameEntityVec& container);

		void aquireTransformSlot(size_t& outSlot, size_t& outBufferIdx);
		void releaseTransformSlot(size_t bufferIdx, GameEntityTransform* transform);

	public:
		GameEntityManager(Mq::MessageQueueSystem* graphicsSystem, LogicSystem* logicSystem);
		~GameEntityManager();

		GameEntity* addGameEntity(Ogre::SceneMemoryMgrTypes      type,
		                          const MovableObjectDefinition* moDefinition,
		                          const Ogre::Vector3&           initialPos,
		                          const Ogre::Quaternion&        initialRot,
		                          const Ogre::Vector3&           initialScale);

		void removeGameEntity(GameEntity* toRemove);
		void _notifyGameEntitiesRemoved(size_t slot);
		void finishFrameParallel(void);
	};
}

#endif // _GAMEENTITYMANAGER_H_
