#include "TransformComponent.h"
#include "GameObject.h"
#include "../MyGameEditor/Log.h"
#include "types.h"
#include <yaml-cpp/yaml.h>
#include "Component.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>

static  bool decodeMat(const YAML::Node& node, glm::dmat4& rhs);
static  YAML::Node encodeMat(const glm::dmat4& rhs);

Transform_Component::Transform_Component(GameObject* owner) : Component(owner) { name = "Transform_Component"; }

Transform_Component::Transform_Component(const Transform_Component& other) : Component(other)
{
    matrix = other.matrix;
    position = other.position;
    left = other.left;
    up = other.up;
    forward = other.forward;
}

Transform_Component& Transform_Component::operator=(const Transform_Component& other)
{
    if (this != &other)
    {
        Component::operator=(other);
        matrix = other.matrix;
        position = other.position;
        left = other.left;
        up = other.up;
        forward = other.forward;
    }
    return *this;
}

Transform_Component::Transform_Component(Transform_Component&& other) noexcept : Component(std::move(other))
{
	matrix = std::move(other.matrix);
	position = std::move(other.position);
	left = std::move(other.left);
	up = std::move(other.up);
	forward = std::move(other.forward);
}

Transform_Component& Transform_Component::operator=(Transform_Component&& other) noexcept
{
	if (this != &other)
	{
		Component::operator=(std::move(other));
		matrix = std::move(other.matrix);
		position = std::move(other.position);
		left = std::move(other.left);
		up = std::move(other.up);
		forward = std::move(other.forward);
	}
	return *this;
}

std::unique_ptr<Component> Transform_Component::Clone(GameObject* owner)
{
    auto clone = std::make_unique<Transform_Component>(*this);
    clone->matrix = this->matrix;
    clone->left = this->left;
    clone->up = this->up;
    clone->forward = this->forward;
    clone->position = this->position;
    clone->owner = owner;
    return clone;
}

void Transform_Component::Translate(const glm::dvec3& translation)
{

    matrix = glm::translate(matrix, translation);

    /* After modifying WORLD , recalculate  LOCAL */

    HandleLocalUpdate();

}

void Transform_Component::TranslateLocal(const glm::dvec3& translation) {

    local_matrix = glm::translate(local_matrix, translation);

    HandleWorldUpdate();
}

void Transform_Component::Update(float deltaTime) 
{
    if (owner) {

        if (!owner->GetParent())/*No owner means owner is the scene*/ {
           UpdateWorldMatrix(glm::dmat4(1.0));
        }
        for (auto& child : owner->GetChildren()) {
            child->GetTransform()->UpdateWorldMatrix(matrix);
        }
    }
    else
    {
        UpdateWorldMatrix(glm::dmat4(1.0));
    }

    //log if owner is null or not
    bool isOwnerNull = owner == nullptr;
    if (isOwnerNull)
    {
        LOG(LogType::LOG_ERROR, "Owner is null");
    }
    else
    {
		LOG(LogType::LOG_ERROR, "Owner is not null");
	}
}

void Transform_Component::SetPosition(const glm::dvec3& position)
{
    matrix[3] = glm::dvec4(position, 1);
    HandleLocalUpdate();
}



void Transform_Component::Rotate(double rads, const glm::dvec3& axis)
{

    matrix = glm::rotate(matrix, rads, axis);
    HandleLocalUpdate();

}

void Transform_Component::RotateLocal(double rads, const glm::dvec3& axis) {

    local_matrix = glm::rotate(local_matrix, rads, axis);
    HandleWorldUpdate();
}

void Transform_Component::SetRotation(const glm::dvec3& eulerAngles)
{
    glm::dmat4 rotationMatrix = glm::identity<glm::dmat4>();
    rotationMatrix = glm::rotate(rotationMatrix, eulerAngles.x, glm::dvec3(1, 0, 0));
    rotationMatrix = glm::rotate(rotationMatrix, eulerAngles.y, glm::dvec3(0, 1, 0));
    rotationMatrix = glm::rotate(rotationMatrix, eulerAngles.z, glm::dvec3(0, 0, 1));

    // Preserve the position
    glm::dvec3 position = glm::dvec3(matrix[3]);
    if (rotationMatrix != matrix) {
        matrix = rotationMatrix;
        matrix[3] = glm::dvec4(position, 1);
        LOG(LogType::LOG_ERROR, "Attempting to rotate");
        HandleLocalUpdate();
    }
}


void Transform_Component::SetScale(const glm::dvec3& scale)
{

    glm::dmat4 scaleMatrix = glm::identity<glm::dmat4>();
    scaleMatrix = glm::scale(scaleMatrix, scale);

    // Preserve the position
    glm::dvec3 position = glm::dvec3(matrix[3]);
    matrix = scaleMatrix;
    matrix[3] = glm::dvec4(position, 1);

    HandleLocalUpdate();
}

void Transform_Component::Scale(const glm::dvec3& scale)
{
    matrix = glm::scale(matrix, scale);
}

void Transform_Component::LookAt(const glm::dvec3& target)
{
    matrix = glm::lookAt(position, target, up);

    HandleLocalUpdate();
}

void Transform_Component::AlignToGlobalUp(const glm::vec3& worldUp)
{
    glm::vec3 fwd = glm::normalize(forward);
    glm::vec3 right = glm::normalize(glm::cross(worldUp, fwd));
    glm::vec3 up = glm::cross(fwd, right);

    left = right;
    this->up = up;
    forward = fwd;
    position = position;
	
    SetMatrix(glm::dmat4(glm::dvec4(left, 0.0), glm::dvec4(up, 0.0), glm::dvec4(forward, 0.0), glm::dvec4(position, 1.0)));
}

void Transform_Component::HandleWorldUpdate() {

    if (owner) {
        if (!owner->GetParent())/*No owner means owner is the scene*/ {
            UpdateWorldMatrix(glm::dmat4(1.0));
        }
        else {
            UpdateWorldMatrix(owner->GetParent()->GetTransform()->matrix);
        }
    }
    else
    {
		UpdateWorldMatrix(glm::dmat4(1.0));
	}
}

void Transform_Component::HandleLocalUpdate() {

    if (owner) {
		if (!owner->GetParent())/*No owner means owner is the scene*/ {
			UpdateLocalMatrix(glm::dmat4(1.0));
		}
		else {
			UpdateLocalMatrix(owner->GetParent()->GetTransform()->matrix);
		}
	}
    else
    {
		UpdateLocalMatrix(glm::dmat4(1.0));
	}
}

YAML::Node Transform_Component::encode() {
    YAML::Node node = Component::encode();

    node["matrix"] = matrix;

    return node;

    //node["position"] = encodePosition();
    //node["rotation"] = encodeRotation();
    //node["scale"] = encodeScale();
    //return node;
}

bool Transform_Component::decode(const YAML::Node& node) {

    Component::decode(node);
    glm::dmat4 new_matrix;
    decodeMat(node, new_matrix);

    SetMatrix(new_matrix);

    return true;

    if (!node["position"] || !node["rotation"] || !node["scale"])
        return false;
    /*----------Position-------------*/
    glm::dvec3 _position;
    YAML::convert<glm::dvec3>::decode(node["position"], _position);
    SetPosition(_position);
    /*----------Scale-------------*/
    glm::dvec3 new_scale;
    YAML::convert<glm::dvec3>::decode(node["scale"], new_scale);
    SetScale(new_scale);
    /*----------Rotation-------------*/
    glm::dvec3 new_rotation;
    YAML::convert<glm::dvec3>::decode(node["rotation"], new_rotation);
    SetRotation(new_rotation);


    return true;
}

    static  YAML::Node encodeMat(const glm::dmat4& rhs) {
        YAML::Node node;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                node["row" + std::to_string(i)]["col" + std::to_string(j)] = rhs[i][j];
            }
        }
        return node;
    }

    static  bool decodeMat(const YAML::Node& node, glm::dmat4& rhs) {

        YAML::Node matNode = node["matrix"];

        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                std::string rowKey = "row" + std::to_string(i);
                std::string colKey = "col" + std::to_string(j);

                if (!matNode[rowKey] || !matNode[rowKey][colKey]) {
                    return false;
                }
                rhs[i][j] = matNode[rowKey][colKey].as<double>();
            }
        }
        return true;
    }