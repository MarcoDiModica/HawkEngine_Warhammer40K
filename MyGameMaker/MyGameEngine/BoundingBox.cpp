#include "BoundingBox.h"


BoundingBox::BoundingBox(const vec3* vertices, size_t num_verts) {
	min = max = vertices[0];
	for (size_t i = 1; i < num_verts; ++i) {
		min = glm::min(min, vertices[i]);
		max = glm::max(max, vertices[i]);
	}
}

BoundingBox BoundingBox::operator+(const BoundingBox& other) const {
	BoundingBox result;
	result.min = glm::min(min, other.min);
	result.max = glm::max(max, other.max);
	return result;
}

BoundingBox operator*(const mat4& mat, const BoundingBox& bbox) {
	auto vertices = bbox.vertices();
	for (auto& v : vertices) v = mat * vec4(v, 1);
	return BoundingBox(vertices.data(), vertices.size());
}


