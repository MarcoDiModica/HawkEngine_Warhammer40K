#pragma once

#include <array>
#include "types.h"
//#include "GameObject.h"

class GameObject;

struct BoundingBox {

	vec3 min;
	vec3 max;

	vec3 center() const { return (min + max) * 0.5; }
	vec3 size() const { return max - min; }

	const vec3& v000() const { return min; }
	vec3 v001() const { return vec3(min.x, min.y, max.z); }
	vec3 v010() const { return vec3(min.x, max.y, min.z); }
	vec3 v011() const { return vec3(min.x, max.y, max.z); }
	vec3 v100() const { return vec3(max.x, min.y, min.z); }
	vec3 v101() const { return vec3(max.x, min.y, max.z); }
	vec3 v110() const { return vec3(max.x, max.y, min.z); }
	const vec3& v111() const { return max; }

	std::array<vec3, 8> vertices() const { return { v000(), v001(), v010(), v011(), v100(), v101(), v110(), v111() }; }

	BoundingBox() = default;
	BoundingBox(const vec3& min, const vec3& max) {
		this->min = min;
		this->max = max;
	}

	BoundingBox(const vec3* vertices, size_t num_verts);

	BoundingBox operator+(const BoundingBox& other) const;


	bool intersects(const BoundingBox& other) const;

	bool contains(const vec3& point) const;

	bool contains(const GameObject& go) const;

	bool contains(const BoundingBox& other) const {
		return (other.min.x >= min.x && other.max.x <= max.x) &&
			(other.min.y >= min.y && other.max.y <= max.y) &&
			(other.min.z >= min.z && other.max.z <= max.z);
	}

	// To create octree
	std::array<BoundingBox, 8> subdivide() const;


	void draw();

};
BoundingBox operator*(const mat4& mat, const BoundingBox& bbox);