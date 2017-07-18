#ifndef _COLLISIONCAMERACONTROLLER_H_
#define _COLLISIONCAMERACONTROLLER_H_

#include "CameraController.h"

namespace Collision {

        class CollisionCameraController : public Common::CameraController {
        private:
                btDynamicsWorld*                mWorld;
                btPairCachingGhostObject*       mGhostObject;
                btKinematicCharacterController* mCharacter;

        public:
                CollisionCameraController(Common::GraphicsSystem* graphicsSystem, btDynamicsWorld* world);

                virtual void update(float timeSinceLast);
        };

}

#endif // _CAMERACONTROLLER_H_
