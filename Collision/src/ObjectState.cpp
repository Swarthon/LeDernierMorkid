#include "ObjectState.h"
#include "Converter.h"

#include "GameEntity.h"
#include "LogicSystem.h"

namespace Collision {
	ObjectState::ObjectState(btTransform& initial, Common::GameEntity* gameEntity, const Common::LogicSystem* logicSystem) {
		mWorldTransform = initial;
		mGameEntity     = gameEntity;
		mLogicSystem    = logicSystem;
	}
	ObjectState::~ObjectState() {
	}

	void ObjectState::getWorldTransform(btTransform& worldTrans) const {
		worldTrans = mWorldTransform;
	}
	void ObjectState::setWorldTransform(const btTransform& worldTrans) {
		assert(mGameEntity);

		size_t currIdx                         = mLogicSystem->getCurrentTransformIdx();
		mGameEntity->mTransform[currIdx]->vPos = Converter::to(worldTrans.getOrigin());
		mGameEntity->mTransform[currIdx]->qRot = Converter::to(worldTrans.getRotation());

		mWorldTransform == worldTrans;
	}
}
