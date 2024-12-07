#include "Octree.h"
#include "GameObject.h"
#include "BoundingBox.h"


void Octree::Insert(OctreeNode* node, const GameObject& obj, int depth) {
    // If the object's bounding box does not intersect this node, do nothing
    if (!node->bbox.intersects(obj.boundingBox())) return;

    // If this is a leaf node and has room for more objects, add the object here
    if (node->isLeaf() && (node->contained_objects.size() < max_points_per_node || depth >= max_depth)) {
        node->contained_objects.push_back(obj);
        return;
    }

    // Subdivide the node if it's a leaf but exceeds capacity
    if (node->isLeaf()) {
        Subdivide(node);
    }

    // Recurse into the appropriate child based on the object's bounding box
    for (auto* child : node->children) {
        if (child && child->bbox.intersects(obj.boundingBox())) {
            Insert(child, obj, depth + 1);
            return;
        }
    }
}


void Octree::Subdivide(OctreeNode* node) {
   auto subBounds = node->bbox.subdivide();
    for (size_t i = 0; i < 8; ++i) {
        node->children[i] = new OctreeNode(subBounds[i]);
    }

    int h = 0;
}
