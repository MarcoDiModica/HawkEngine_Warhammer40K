#include "Model.h"

void Model::calculateBoundingBox()
{
    if (modelData.vertexData.empty()) {
        m_BoundingBox = BoundingBox(); // Bounding box vacía si no hay vértices
        return;
    }
    // Calcula la bounding box a partir de los vértices
    m_BoundingBox = BoundingBox(modelData.vertexData.data(), modelData.vertexData.size());
}
