#ifndef _BASESYSTEM_H_
#define _BASESYSTEM_H_

#include "Threading/MessageQueueSystem.h"

namespace Common {
	class GameState;

	class BaseSystem : public Mq::MessageQueueSystem {
	protected:
		GameState* mCurrentGameState;

	public:
		BaseSystem(GameState* gameState);
		virtual ~BaseSystem();

		virtual void initialize(void);
		virtual void deinitialize(void);

		virtual void createScene(void);

		virtual void destroyScene(void);

		void beginFrameParallel(void);
		void update(float timeSinceLast);
		void finishFrameParallel(void);
		void finishFrame(void);
	};
}

#endif // _BASESYSTEM_H_
