#include "Octree.h"
#include "GameObject.h"
#include "BoundingBox.h"


void Octree::Insert(OctreeNode* node, const GameObject& obj, int depth) {
    // If the object's bounding box does not intersect this node, do nothing
    if (!node->bbox.intersects(obj.boundingBox()) || !node->bbox.contains(obj)) {
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

        for (size_t j = 0; j < node->contained_objects.size(); ++j) {
            GameObject& object = node->contained_objects[j];

            auto bb = object.boundingBox();

            //--check if any of the parent's objectsshould be reasigned to the new child
            if (child->bbox.contains(object) || child->bbox.intersects(object.boundingBox())) {

                Insert(child, object, depth + 1); /* Add obj to new child */
                node->contained_objects.erase(node->contained_objects.begin() + j); /*Remove from parent*/
                j--;
            }
            else {
                int h = 8;
            }
        }
        int r = 9;
    }
    int i = 0;
    for (auto* child : node->children){
  /*      if (i == 5 ) {
            int g = 7;
            auto bb1 = node->children[i]->contained_objects[0].boundingBox();
            auto bb2 = obj.boundingBox();
            int u = 5;
        }*/
        if (node->children[i]->bbox.contains(obj) || node->children[i]->bbox.intersects(obj.boundingBox())) {
            Insert(child, obj, depth + 1);


            return;
        }
        i++;
    }
}


void Octree::Subdivide(OctreeNode* node) {
   auto subBounds = node->bbox.subdivide();
    for (size_t i = 0; i < 8; ++i) {
        node->children[i] = new OctreeNode(subBounds[i]);

    }

    int h = 0;
}
