#ifndef OBJECTSTATE_H_
#define OBJECTSTATE_H_

#include <LinearMath/btMotionState.h>
#include <LinearMath/btTransform.h>

namespace Common {
	class GameEntity;
	class LogicSystem;
}

namespace Collision {
	class ObjectState : public btMotionState {
	public:
		ObjectState(btTransform& i, Common::GameEntity* ge, const Common::LogicSystem* ls);
		virtual ~ObjectState();

		virtual void getWorldTransform(btTransform& worldTrans) const;
		virtual void setWorldTransform(const btTransform& worldTrans);

	private:
		Common::GameEntity*        mGameEntity;
		const Common::LogicSystem* mLogicSystem;
		btTransform                mInitialPosition;
	};
}

#endif // OBJECTSTATE_H_
