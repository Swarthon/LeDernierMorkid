#ifndef _CONVERTER_H_
#define _CONVERTER_H_

namespace Common {
        struct GameEntityTransform;
}

#include <OgreVector3.h>
#include <OgreQuaternion.h>

#include <LinearMath/btTransform.h>
#include <LinearMath/btQuaternion.h>
#include <LinearMath/btVector3.h>
#include <LinearMath/btVector3.h>

namespace Collision {
        class Converter {
        public:
                static Common::GameEntityTransform to(const btTransform& transform, btVector3 scale = btVector3(1,1,1));
                static btTransform                 to(const Common::GameEntityTransform& transform);

                static Ogre::Quaternion to(const btQuaternion& quaternion);
                static btQuaternion     to(const Ogre::Quaternion& quaternion);

                static Ogre::Vector3 to(const btVector3& vector);
                static btVector3     to(const Ogre::Vector3& vector);
        };
}

#endif // _CONVERTER_H_
