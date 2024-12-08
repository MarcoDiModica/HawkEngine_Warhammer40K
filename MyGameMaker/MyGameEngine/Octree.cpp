#include "Octree.h"
#include "GameObject.h"
#include "BoundingBox.h"


void Octree::Insert(OctreeNode* node, const GameObject& obj, int depth) {
    // If the object's bounding box does not intersect this node, do nothing
    if (!node->bbox.intersects(obj.boundingBox()) && !node->bbox.contains(obj)) {
        return;
    }

    // If this is a leaf node and has room for more objects, add the object here
    if (node->isLeaf() && (node->contained_objects.size() < max_points_per_node || depth >= max_depth)) {
        node->contained_objects.push_back(obj);
        return;
    }

    // Since the node exceeds capacity split it into 8 children
    if (node->isLeaf()) {
        Subdivide(node);
    }
    for (auto* child : node->children) /* Insert the object to the children that contains it*/ {
        if (child && child->bbox.contains(obj.boundingBox()) || node->bbox.contains(obj)) {
            Insert(child, obj, depth + 1);

            //--check if any of the parent's objectsshould be reasigned to the new child

            for (size_t j = 0; j < node->contained_objects.size(); ++j) {
                GameObject& object = node->contained_objects[j];

                if (child->bbox.contains(object) || child->bbox.intersects(object.boundingBox())) {
                    
                    Insert(child, object, depth + 1); /* Add obj to new child */
                    node->contained_objects.erase(node->contained_objects.begin() + j); /*Remove from parent*/
                }
            }

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
