#include "Transform.h"
#include <glm/gtc/matrix_transform.hpp>

void Transform::translate(const vec3& v) {
	_mat = glm::translate(_mat, v);
}

void Transform::rotate(double rads, const vec3& v) {
	_mat = glm::rotate(_mat, rads, v);
}