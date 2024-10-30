#pragma once

#include "MyGameEngine/types.h"
#include <glm/glm.hpp>

class Transform {

	union {
		mat4 _mat = mat4(1.0);
		struct {
			vec3 _left; mat4::value_type _left_w;
			vec3 _up; mat4::value_type _up_w;
			vec3 _fwd; mat4::value_type _fwd_w;
			vec3 _pos; mat4::value_type _pos_w;
		};
	};

public:
	const auto& mat() const { return _mat; }
	const auto& left() const { return _left; }
	const auto& up() const { return _up; }
	const auto& fwd() const { return _fwd; }
	const auto& pos() const { return _pos; }
	auto& pos() { return _pos; }

	const auto* data() const { return &_mat[0][0]; }

	Transform() = default;
	Transform(const mat4& mat) : _mat(mat) {}

	void alignToGlobalUp(const vec3& worldUp = vec3(0.0f, 1.0f, 0.0f));
	void translate(const vec3& v);
	void rotate(double rads, const vec3& v);
	void LookAt(const vec3& target);
	Transform operator*(const mat4& other) { return Transform(_mat * other); }
	Transform operator*(const Transform& other) { return Transform(_mat * other._mat); }
};

inline Transform operator*(const mat4& m, const Transform& t) { return Transform(m * t.mat()); }