#include "Model.h"

void Model::calculateBoundingBox()
{
    if (modelData.vertexData.empty()) {
        m_BoundingBox = BoundingBox(); // Bounding box vac�a si no hay v�rtices
        return;
    }
    // Calcula la bounding box a partir de los v�rtices
    m_BoundingBox = BoundingBox(modelData.vertexData.data(), modelData.vertexData.size());
}
