#include "TransformComponent.h"
#include "GameObject.h"
#include "../MyGameEditor/Log.h" // Esto es ilegal verdad?
#include "types.h"
#include "Component.h"
#include "../MyScriptingEngine/MonoManager.h"
#include "../MyScriptingEngine/EngineBinds.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/jit/jit.h>

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

        local_matrix = other.local_matrix;
    }

   // mono_add_internal_call("aaa", (const void*) &Transform_Component::SetPosition);

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
    clone->local_matrix = this->local_matrix;
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
		//LOG(LogType::LOG_ERROR, "Owner is not null");
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
        HandleLocalUpdate();
    }
}

void Transform_Component::SetRotationQuat(const glm::dquat& rotation)
{
    glm::dvec3 position = matrix[3];
	matrix = glm::mat4_cast(rotation);
	HandleLocalUpdate();
    SetPosition(position);
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

void Transform_Component::SetForward(const glm::dvec3& forward)
{
	glm::dvec3 newForward = glm::normalize(forward);
	glm::dvec3 newRight = glm::normalize(glm::cross(up, newForward));
	glm::dvec3 newUp = glm::cross(newForward, newRight);

	left = newRight;
	this->up = newUp;
	this->forward = newForward;

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

    MonoObject* Transform_Component::GetSharp() {
        if (CsharpReference) {
            return CsharpReference;
        }

        // 1. Obtener la clase de C#
        MonoClass* klass = MonoManager::GetInstance().GetClass("HawkEngine", "Transform");
        if (!klass) {
            // Manejar el error si la clase no se encuentra
            return nullptr;
        }

        // 2. Crear una instancia del objeto de C#
        MonoObject* monoObject = mono_object_new(MonoManager::GetInstance().GetDomain(), klass);
        if (!monoObject) {
            // Manejar el error si la instancia no se puede crear
            return nullptr;
        }

        // 3. Obtener el constructor correcto
        MonoMethodDesc* constructorDesc = mono_method_desc_new("HawkEngine.Transform:.ctor(uintptr,HawkEngine.GameObject)", true);
        MonoMethod* method = mono_method_desc_search_in_class(constructorDesc, klass);
        if (!method)
        {
            //Manejar error si el constructor no se encuentra
            return nullptr;
        }

        // 4. Preparar los argumentos para el constructor
        uintptr_t componentPtr = reinterpret_cast<uintptr_t>(this); // Puntero a la instancia de Transform_Component
        MonoObject* ownerGo = owner->GetSharp(); // Obtenemos la instancia del propietario de C#
        if (!ownerGo)
        {
            //Manejar error si el propietario no tiene instancia de C#
            return nullptr;
        }

        void* args[2];
        args[0] = &componentPtr;
        args[1] = ownerGo;

        // 5. Invocar al constructor
        mono_runtime_invoke(method, monoObject, args, NULL);

        // 6. Guardar la referencia y devolverla
        CsharpReference = monoObject;
        return CsharpReference;
    }