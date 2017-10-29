#ifndef _LOGICState_H_
#define _LOGICState_H_

#include <OgrePrerequisites.h>

#include "State/State.h"
#include "Terrain/CollisionTerrain.h"

namespace Common {
	class LogicSystem;
	struct GameEntity;
	struct MovableObjectDefinition;
	struct CollisionObjectDefinition;
} // namespace Common

/**
 * @class LogicState
 * @brief
 *	LogicState describing how logics work
 */
class LogicState : public Common::State {
	/// LogicSystem containing logics data
	Common::LogicSystem* mLogicSystem;
	/// Collision part of Terrain
	CollisionTerrain*    mTerrain;

public:
	/// Simple Constructor
	LogicState();
	/// Simple Destructor
	~LogicState();

	/**
	 * Set mLogicSystem value
	 * @param logicSystem
	 *	LogicSystem to set
	 */
	void _notifyLogicSystem(Common::LogicSystem* logicSystem) { mLogicSystem = logicSystem; }
	/// Create the scene
	virtual void enter(void);
        virtual void exit() {}
	/**
	 * Update the State. Method to call once per frame
	 * @param timeSinceLast
	 *	Time past since last frame
	 */
	virtual void update(double timeSinceLast);
};

#endif // _LOGICState_H_
