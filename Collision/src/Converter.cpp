#include "Converter.h"

#include "GameEntity.h"

namespace Collision {
	Common::GameEntityTransform Converter::to(const btTransform& transform, btVector3 scale) {
		Common::GameEntityTransform gameTransform;
		gameTransform.vPos   = to(transform.getOrigin());
		gameTransform.qRot   = to(transform.getRotation());
		gameTransform.vScale = to(scale);

		return gameTransform;
	}
	// ------------------------------------------------------------------------------------------------
	btTransform Converter::to(const Common::GameEntityTransform& transform) {
		btTransform bulletTransform;
		bulletTransform.setOrigin(to(transform.vPos));
		bulletTransform.setRotation(to(transform.qRot));

		return bulletTransform;
	}
	// ------------------------------------------------------------------------------------------------
	Ogre::Quaternion Converter::to(const btQuaternion& q) {
		return Ogre::Quaternion(q.getW(),
		                        q.getX(),
		                        q.getY(),
		                        q.getZ());
	}
	// ------------------------------------------------------------------------------------------------
	btQuaternion Converter::to(const Ogre::Quaternion& q) {
		return btQuaternion(q.x,
		                    q.y,
		                    q.z,
		                    q.w);
	}
	//------------------------------------------------------------------------------------------------
	Ogre::Vector3 Converter::to(const btVector3& v) {
		return Ogre::Vector3(v.getX(),
		                     v.getY(),
		                     v.getZ());
	}
	// ------------------------------------------------------------------------------------------------
	btVector3 Converter::to(const Ogre::Vector3& v) {
		return btVector3(v.x,
		                 v.y,
		                 v.z);
	}
	// ------------------------------------------------------------------------------------------------
}
