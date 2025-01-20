#pragma once

#include <bullet/btBulletDynamicsCommon.h>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <iostream>
#include "BoundingBox.h"

class DebugDrawerPhysics : public btIDebugDraw {
public:
    DebugDrawerPhysics();

    void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override;
    void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) override;
    void reportErrorWarning(const char* warningString) override;
    void draw3dText(const btVector3& location, const char* textString) override;
    void setDebugMode(int debugMode) override;
    int getDebugMode() const override;

    void drawBoundingBox(const BoundingBox& bbox, const glm::vec3& color);

private:
    void drawWiredLine(const glm::vec3& start, const glm::vec3& end, const glm::vec3& color);
    DebugDrawModes mode;
};
