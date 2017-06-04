#ifndef _LOGICSYSTEM_H_
#define _LOGICSYSTEM_H_

#include "BaseSystem.h"
#include <OgrePrerequisites.h>

namespace Common {
	class GameEntityManager;

	class LogicSystem : public BaseSystem {
	protected:
		BaseSystem          *mGraphicsSystem;
		GameEntityManager   *mGameEntityManager;

		Ogre::uint32                mCurrentTransformIdx;
		std::deque<Ogre::uint32>    mAvailableTransformIdx;

		virtual void processIncomingMessage( Mq::MessageId messageId, const void *data );

	public:
		LogicSystem( GameState *gameState );
		virtual ~LogicSystem();

		void _notifyGraphicsSystem( BaseSystem *graphicsSystem )    { mGraphicsSystem = graphicsSystem; }
		void _notifyGameEntityManager( GameEntityManager *mgr )     { mGameEntityManager = mgr; }

		void finishFrameParallel(void);

		GameEntityManager* getGameEntityManager(void)               { return mGameEntityManager; }
		Ogre::uint32 getCurrentTransformIdx(void) const             { return mCurrentTransformIdx; }
	};
}

#endif // _LOGICSYSTEM_H_
