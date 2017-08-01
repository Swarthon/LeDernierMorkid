#ifndef _LOGICGAMESTATE_H_
#define _LOGICGAMESTATE_H_

#include <OgrePrerequisites.h>

#include "GameState.h"
#include "Terrain/CollisionTerrain.h"

namespace Common {
	class LogicSystem;
	struct GameEntity;
	struct MovableObjectDefinition;
	struct CollisionObjectDefinition;
} // namespace Common

class LogicGameState : public Common::GameState {
	Common::LogicSystem* mLogicSystem;
	CollisionTerrain* mTerrain;

public:
	LogicGameState();
	~LogicGameState();

	void _notifyLogicSystem(Common::LogicSystem* logicSystem) { mLogicSystem = logicSystem; }
	virtual void                                 createScene(void);
	virtual void update(float timeSinceLast);
};

#endif // _LOGICGAMESTATE_H_
