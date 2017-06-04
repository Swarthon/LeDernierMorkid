#ifndef _LOGICGAMESTATE_H_
#define _LOGICGAMESTATE_H_

#include <OgrePrerequisites.h>
#include <GameState.h>

namespace Common {
	class LogicSystem;
	struct GameEntity;
	struct MovableObjectDefinition;

	class LogicGameState : public GameState {
		float               mDisplacement;
		GameEntity              *mCubeEntity;
		MovableObjectDefinition *mCubeMoDef;

		LogicSystem         *mLogicSystem;

	public:
		LogicGameState();
		~LogicGameState();

		void _notifyLogicSystem( LogicSystem *logicSystem )     { mLogicSystem = logicSystem; }

		virtual void createScene(void);
		virtual void update( float timeSinceLast );
	};
}

#endif // _LOGICGAMESTATE_H_
