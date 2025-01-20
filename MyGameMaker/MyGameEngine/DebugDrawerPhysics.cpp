#include "DebugDrawerPhysics.h"

DebugDrawerPhysics::DebugDrawerPhysics() : mode(DBG_DrawWireframe) {}

void DebugDrawerPhysics::drawLine(const btVector3& from, const btVector3& to, const btVector3& color) {
    glm::vec3 start(from.getX(), from.getY(), from.getZ());
    glm::vec3 end(to.getX(), to.getY(), to.getZ());
    glm::vec3 col(color.getX(), color.getY(), color.getZ());

    drawWiredLine(start, end, col);
}

void DebugDrawerPhysics::drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) {
    glm::vec3 position(PointOnB.getX(), PointOnB.getY(), PointOnB.getZ());
    glm::vec3 col(color.getX(), color.getY(), color.getZ());

    glm::vec3 endPoint = position + glm::vec3(normalOnB.getX(), normalOnB.getY(), normalOnB.getZ()) * distance;
    drawWiredLine(position, endPoint, col);
}

void DebugDrawerPhysics::reportErrorWarning(const char* warningString) {
    std::cerr << "Bullet warning: " << warningString << std::endl;
}

void DebugDrawerPhysics::draw3dText(const btVector3& location, const char* textString) {
    // Placeholder for 3D text drawing if necessary
}

void DebugDrawerPhysics::setDebugMode(int debugMode) {
    mode = static_cast<DebugDrawModes>(debugMode);
}

int DebugDrawerPhysics::getDebugMode() const {
    return mode;
}

void DebugDrawerPhysics::drawWiredLine(const glm::vec3& start, const glm::vec3& end, const glm::vec3& color) {
    glPushAttrib(GL_CURRENT_BIT | GL_LINE_BIT);

    glColor3f(color.r, color.g, color.b);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex3f(start.x, start.y, start.z);
    glVertex3f(end.x, end.y, end.z);
    glEnd();

    glPopAttrib();
}

void DebugDrawerPhysics::drawBoundingBox(const BoundingBox& bbox, const glm::vec3& color) {
    glPushAttrib(GL_CURRENT_BIT | GL_LINE_BIT);

    glColor3f(color.r, color.g, color.b);
    glLineWidth(2.0f);

    // Draw the edges of the BoundingBox
    drawWiredLine(bbox.v000(), bbox.v001(), color);
    drawWiredLine(bbox.v001(), bbox.v011(), color);
    drawWiredLine(bbox.v011(), bbox.v010(), color);
    drawWiredLine(bbox.v010(), bbox.v000(), color);

    drawWiredLine(bbox.v100(), bbox.v101(), color);
    drawWiredLine(bbox.v101(), bbox.v111(), color);
    drawWiredLine(bbox.v111(), bbox.v110(), color);
    drawWiredLine(bbox.v110(), bbox.v100(), color);

    drawWiredLine(bbox.v000(), bbox.v100(), color);
    drawWiredLine(bbox.v001(), bbox.v101(), color);
    drawWiredLine(bbox.v011(), bbox.v111(), color);
    drawWiredLine(bbox.v010(), bbox.v110(), color);

    glPopAttrib();
}
