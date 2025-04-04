

#pragma once
#include <bullet/btBulletDynamicsCommon.h>



class CustomCollisionDispatcher : public btCollisionDispatcher {
public:
    CustomCollisionDispatcher(btCollisionConfiguration* config) : btCollisionDispatcher(config) {}

    virtual bool needsCollision(const btCollisionObject* body0, const btCollisionObject* body1) override {
        if (body0->isKinematicObject() || body1->isKinematicObject()) {
            return true;  
        }
        return btCollisionDispatcher::needsCollision(body0, body1);
    }
};