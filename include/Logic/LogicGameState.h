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

/**
 * @class LogicGameState
 * @brief
 *	LogicGameState describing how logics work
 */
class LogicGameState : public Common::GameState {
	/// LogicSystem containing logics data
	Common::LogicSystem* mLogicSystem;
	/// Collision part of Terrain
	CollisionTerrain*    mTerrain;

public:
	/// Simple Constructor
	LogicGameState();
	/// Simple Destructor
	~LogicGameState();

	/**
	 * Set mLogicSystem value
	 * @param logicSystem
	 *	LogicSystem to set
	 */
	void _notifyLogicSystem(Common::LogicSystem* logicSystem) { mLogicSystem = logicSystem; }
	/// Create the scene
	virtual void                                 createScene(void);
	/**
	 * Update the State. Method to call once per frame
	 * @param timeSinceLast
	 *	Time past since last frame
	 */
	virtual void update(float timeSinceLast);
};

#endif // _LOGICGAMESTATE_H_
