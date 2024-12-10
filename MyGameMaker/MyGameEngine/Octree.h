#ifndef OCTREE_H
#define OCTREE_H

#pragma once
#include "BoundingBox.h"
#include "GameObject.h"
#include <vector>
#include <array>

class Scene;

struct OctreeNode {

	OctreeNode(const BoundingBox& bbox) {
		this->bbox = bbox;
	}

	~OctreeNode() {
		/* Recursively delete all child nodes */
		for (int i = 0; i < 8; ++i) {
			delete children[i]; 
		}

	}


	BoundingBox bbox;
	std::vector<std::weak_ptr<GameObject>> contained_objects; // objects contained in the bbox
	OctreeNode* children [8] = { nullptr };

	bool isLeaf() const {

		if (children[0] == nullptr) { return true; }

		return false;
	}

	void removeObject(GameObject& obj) {

		for (int i = 0; i < contained_objects.size(); ++i) {

			if (contained_objects[i].lock() == nullptr) {
				contained_objects.erase(contained_objects.begin() + i);
				return;
			}

			if (obj == *contained_objects[i].lock()) {
				
				contained_objects.erase(contained_objects.begin() + i);
				return;
			}
		}

		return;

	}


};

class Octree
{
public:
	Octree(const BoundingBox& bounds, int max_depth, int max_points_per_node) {
		this->max_depth = max_depth;
		this->max_points_per_node = max_points_per_node;
		root = new OctreeNode(bounds);
	}

	~Octree() { delete root; }

	/* Find gameObjects contained in specificed region */
	std::vector<GameObject> query(const BoundingBox& region) const {
		return query(root, region);
	}

	void DebugDraw(OctreeNode* root_node) {

		for ( auto& node : root_node->children  ){
			
			if (node == NULL) { continue; }

			if (! node->isLeaf()) {

				DebugDraw(node);
			}
			
			node->bbox.draw();
		
		}

		root_node->bbox.draw();

	}

private:
	friend class Scene;
	friend class GameObject;
	friend class Root;

	OctreeNode* root;
	// limit as to how much the octree can subdivide
	int max_depth;
	// how many objects can a node contain before it splits into 4
	int max_points_per_node;

	void Subdivide(OctreeNode* node);
	void Insert(OctreeNode* node,  GameObject& go, int depth = 0);


	std::vector<GameObject> query(OctreeNode* node, const BoundingBox& region) const {
		std::vector<GameObject> result;

		if (!node || !node->bbox.intersects(region)) return result;

		for (const auto obj : node->contained_objects) {
			std::shared_ptr<GameObject> object = obj.lock();
			if (region.contains(*object)) result.push_back(*object);
		}

		// Recurse into children
		for (auto* child : node->children) {
			auto childResult = query(child, region);
			result.insert(result.end(), childResult.begin(), childResult.end());
		}

		return result;
	}


};

#endif