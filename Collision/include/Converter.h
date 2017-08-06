#ifndef _CONVERTER_H_
#define _CONVERTER_H_

namespace Common {
	struct GameEntityTransform;
}

#include <OgreQuaternion.h>
#include <OgreVector3.h>

#include <LinearMath/btQuaternion.h>
#include <LinearMath/btTransform.h>
#include <LinearMath/btVector3.h>
#include <LinearMath/btVector3.h>

namespace Collision {

	/**
	 * @class Converter
	 * @brief
	 *	Converter between Bullet to Ogre
	 *
	 * This class provide functions to convert Ogre Math to Bullet Math (Ogre::Vector3 -> btVector3 ...)
	 * @remarks
	 *	This class should not be used as an object
	 */
	class Converter {
	public:
		/**
		 * Convert btTransform to Common::GameEntityTransform
		 * @param transform
		 *	btTransform to convert
		 * @param scale
		 *	scale to add to transform to create the Common::GameEntityTransform
		 * @return
		 *	the Common::GameEntityTransform created
		 * @remarks
		 *	scale is not needed but should not be forgetten, btTransform does not provide scale but Common::GameEntityTransform does
		 */
		static Common::GameEntityTransform to(const btTransform& transform, btVector3 scale = btVector3(1, 1, 1));
		/**
		 * Convert Common::GameEntityTransform to btTransform
		 * @param transform
		 *	Common::GameEntityTransform to convert
		 * @return
		 *	the btTransform created
		 * @remarks
		 *	As Common::GameEntityTransform provide a scale attribute but btTransform does not, do not forget to manage this case
		 */
		static btTransform to(const Common::GameEntityTransform& transform);

		/**
		 * Convert btQuaternion to Ogre::Quaternion
		 * @param quaternion
		 *	btQuaternion to convert
		 * @return
		 *	the Ogre::Quaternion created
		 */
		static Ogre::Quaternion to(const btQuaternion& quaternion);
		/**
		 * Convert Ogre::Quaternion to btQuaternion
		 * @param quaternion
		 *	Ogre::Quaternion to convert
		 * @return
		 *	the btQuaternion created
		 */
		static btQuaternion to(const Ogre::Quaternion& quaternion);

		/**
		 * Convert btVector3 to Ogre::Vector3
		 * @param vector
		 *	btVector3 to convert
		 * @return
		 *	the Ogre::Vector3 created
		 */
		static Ogre::Vector3 to(const btVector3& vector);
		/**
		 * Convert Ogre::Quaternion to btVector3
		 * @param vector
		 *	Ogre::Vector3 to convert
		 * @return
		 *	the btVector3 created
		 */
		static btVector3 to(const Ogre::Vector3& vector);
	};
}

#endif // _CONVERTER_H_
