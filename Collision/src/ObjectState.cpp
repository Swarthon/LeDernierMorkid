#include "ObjectState.h"
#include "Converter.h"

#include "GameEntity.h"
#include "LogicSystem.h"

namespace Collision {
	ObjectState::ObjectState(btTransform& i, Common::GameEntity* ge, const Common::LogicSystem* ls) {
		mInitialPosition = i;
		mGameEntity      = ge;
		mLogicSystem	 = ls;
	}
	ObjectState::~ObjectState() {
	}

	void ObjectState::getWorldTransform(btTransform& worldTrans) const {
		worldTrans = mInitialPosition;
	}
	void ObjectState::setWorldTransform(const btTransform& worldTrans) {
		if (mGameEntity == NULL)
			return;

		size_t currIdx = mLogicSystem->getCurrentTransformIdx();
		mGameEntity->mTransform[currIdx]->vPos = Converter::to(worldTrans.getOrigin());
		mGameEntity->mTransform[currIdx]->qRot = Converter::to(worldTrans.getRotation());
	}
}
