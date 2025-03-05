#define GLM_ENABLE_EXPERIMENTAL
#include "TransformComponent.h"
#include "GameObject.h"
#include "../MyGameEditor/Log.h"  // Check licensing if needed.
#include "types.h"
#include "Component.h"
#include "../MyScriptingEngine/MonoManager.h"
#include "../MyScriptingEngine/EngineBinds.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/jit/jit.h>

// YAML helpers (omitted for brevity)
static bool decodeMat(const YAML::Node& node, glm::dmat4& rhs);
static YAML::Node encodeMat(const glm::dmat4& rhs);

Transform_Component::Transform_Component(GameObject* owner) : Component(owner)
{
    name = "Transform_Component";
    matrix = glm::dmat4(1.0);
    local_matrix = glm::dmat4(1.0);
    position = glm::dvec3(0.0);
    forward = glm::dvec3(0, 0, 1);
    up = glm::dvec3(0, 1, 0);
    left = glm::dvec3(1, 0, 0);
}

Transform_Component::Transform_Component(const Transform_Component& other) : Component(other)
{
    matrix = other.matrix;
    local_matrix = other.local_matrix;
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
        local_matrix = other.local_matrix;
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
    local_matrix = std::move(other.local_matrix);
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
        local_matrix = std::move(other.local_matrix);
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
    clone->owner = owner;
    return clone;
}

//------------------------------------------------------------------
// Helper functions for matrix updates.
// These implement:
//   world = parentWorld * local_matrix  
//   local = inverse(parentWorld) * world
//------------------------------------------------------------------
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

void Transform_Component::UpdateWorldMatrix(const glm::dmat4& parentMatrix) {
    matrix = parentMatrix * local_matrix;
    position = glm::dvec3(matrix[3]);
}

void Transform_Component::UpdateLocalMatrix(const glm::dmat4& parentMatrix) {
    local_matrix = glm::inverse(parentMatrix) * matrix;
}

// Use parent's world matrix if available; otherwise identity.
void Transform_Component::HandleWorldUpdate() {
    glm::dmat4 parentMatrix = (owner && owner->GetParent()) ?
        owner->GetParent()->GetTransform()->matrix : glm::dmat4(1.0);
    UpdateWorldMatrix(parentMatrix);
}

void Transform_Component::HandleLocalUpdate() {
    glm::dmat4 parentMatrix = (owner && owner->GetParent()) ?
        owner->GetParent()->GetTransform()->matrix : glm::dmat4(1.0);
    UpdateLocalMatrix(parentMatrix);
}

//------------------------------------------------------------------
// Inspector API functions: these modify local_matrix and then recalc world matrix.
//------------------------------------------------------------------

// Sets the absolute position (world space) by computing the corresponding local position.
void Transform_Component::SetPosition(const glm::dvec3& newPos)
{
    if (owner && owner->GetParent()) {
        glm::dmat4 parentWorld = owner->GetParent()->GetTransform()->matrix;
        glm::dmat4 invParent = glm::inverse(parentWorld);
        glm::dvec4 localPos = invParent * glm::dvec4(newPos, 1.0);
        local_matrix[3] = localPos;
        matrix = parentWorld * local_matrix;
    }
    else {
        matrix[3] = glm::dvec4(newPos, 1.0);
        local_matrix = matrix;
    }
    position = newPos;
}

// Translates in local space.
void Transform_Component::Translate(const glm::dvec3& translation)
{
    if (owner && owner->GetParent()) {
        local_matrix = glm::translate(local_matrix, translation);
        glm::dmat4 parentWorld = owner->GetParent()->GetTransform()->matrix;
        matrix = parentWorld * local_matrix;
    }
    else {
        matrix = glm::translate(matrix, translation);
        local_matrix = matrix;
    }
    position = glm::dvec3(matrix[3]);
}

void Transform_Component::TranslateLocal(const glm::dvec3& translation)
{
    local_matrix = glm::translate(local_matrix, translation);
    HandleWorldUpdate();
    position = glm::dvec3(matrix[3]);
}

// Rotates the object by rads around the given axis by modifying local_matrix.
void Transform_Component::Rotate(double rads, const glm::dvec3& axis)
{
    if (owner && owner->GetParent()) {
        local_matrix = glm::rotate(local_matrix, rads, axis);
        glm::dmat4 parentWorld = owner->GetParent()->GetTransform()->matrix;
        matrix = parentWorld * local_matrix;
    }
    else {
        matrix = glm::rotate(matrix, rads, axis);
        local_matrix = matrix;
    }
    // (Optionally update your basis vectors from matrix)
}

void Transform_Component::RotateLocal(double rads, const glm::dvec3& axis)
{
    local_matrix = glm::rotate(local_matrix, rads, axis);
    HandleWorldUpdate();
}

// Sets rotation using Euler angles (in radians).
void Transform_Component::SetRotation(const glm::dvec3& eulerAngles)
{
    // Decompose the current local matrix into translation, rotation, and scale.
    glm::dvec3 currentScale;
    glm::dquat currentRotation;
    glm::dvec3 currentTranslation;
    glm::dvec3 skew;
    glm::dvec4 perspective;
    glm::decompose(local_matrix, currentScale, currentRotation, currentTranslation, skew, perspective);

    // Compute the new rotation quaternion from the given Euler angles.
    // (Assuming the Euler angles are in radians and the order is X, then Y, then Z.)
    glm::dquat qx = glm::angleAxis(eulerAngles.x, glm::dvec3(1, 0, 0));
    glm::dquat qy = glm::angleAxis(eulerAngles.y, glm::dvec3(0, 1, 0));
    glm::dquat qz = glm::angleAxis(eulerAngles.z, glm::dvec3(0, 0, 1));
    glm::dquat newRotationQuat = qx * qy * qz;

    // Reassemble the local matrix as T * R * S.
    local_matrix = glm::translate(glm::dmat4(1.0), currentTranslation)
        * glm::toMat4(newRotationQuat)
        * glm::scale(glm::dmat4(1.0), currentScale);

    // Update the world matrix based on the parent's transform if available.
    if (owner && owner->GetParent()) {
        glm::dmat4 parentWorld = owner->GetParent()->GetTransform()->matrix;
        matrix = parentWorld * local_matrix;
    }
    else {
        matrix = local_matrix;
    }
}

void Transform_Component::SetRotationQuat(const glm::dquat& rotation)
{
    glm::dmat4 rotMat = glm::mat4_cast(rotation);
    glm::dvec4 localTranslation = local_matrix[3];
    local_matrix = rotMat;
    local_matrix[3] = localTranslation;
    if (owner && owner->GetParent()) {
        glm::dmat4 parentWorld = owner->GetParent()->GetTransform()->matrix;
        matrix = parentWorld * local_matrix;
    }
    else {
        matrix = local_matrix;
    }
    // Optionally update forward/up/left from rotation.
}

// Sets absolute scale by updating the local_matrix.
void Transform_Component::SetScale(const glm::dvec3& scale)
{
    // 1. Extract the translation from the current local matrix.
    glm::dvec4 localTranslation = local_matrix[3];

    // 2. Extract current scale from the local_matrix.
    glm::dvec3 currentScale;
    currentScale.x = glm::length(glm::dvec3(local_matrix[0]));
    currentScale.y = glm::length(glm::dvec3(local_matrix[1]));
    currentScale.z = glm::length(glm::dvec3(local_matrix[2]));

    // 3. Reconstruct the rotation matrix by normalizing each axis.
    glm::dmat4 rotationMatrix(1.0);
    if (currentScale.x > 1e-6)
        rotationMatrix[0] = local_matrix[0] / currentScale.x;
    if (currentScale.y > 1e-6)
        rotationMatrix[1] = local_matrix[1] / currentScale.y;
    if (currentScale.z > 1e-6)
        rotationMatrix[2] = local_matrix[2] / currentScale.z;

    // 4. Build the new scale matrix with the desired scale.
    glm::dmat4 scaleMatrix = glm::scale(glm::dmat4(1.0), scale);

    // 5. Reassemble the local matrix: rotation * new scale, and reapply translation.
    local_matrix = rotationMatrix * scaleMatrix;
    local_matrix[3] = localTranslation;

    // Update world matrix using parent's transform if available.
    if (owner && owner->GetParent()) {
        glm::dmat4 parentWorld = owner->GetParent()->GetTransform()->matrix;
        matrix = parentWorld * local_matrix;
    }
    else {
        matrix = local_matrix;
    }
}

// Sets the forward vector: updates the basis (right, up, forward) in the local_matrix.
// Here we recalc right and up based on the new forward and the existing up (or default to (0,1,0)).
void Transform_Component::SetForward(const glm::dvec3& newFwd)
{
    glm::dvec3 normFwd = glm::normalize(newFwd);
    // Use current up if valid, else default.
    glm::dvec3 refUp = (glm::length(up) < 1e-6) ? glm::dvec3(0, 1, 0) : up;
    glm::dvec3 newRight = glm::normalize(glm::cross(refUp, normFwd));
    glm::dvec3 newUp = glm::cross(normFwd, newRight);

    // Store the new basis.
    left = newRight;
    up = newUp;
    forward = normFwd;

    // Rebuild the rotational part of local_matrix while preserving translation.
    glm::dvec4 translation = local_matrix[3];
    // Assuming column-major order: first column = right, second = up, third = forward.
    local_matrix = glm::dmat4(
        glm::dvec4(newRight, 0.0),
        glm::dvec4(newUp, 0.0),
        glm::dvec4(normFwd, 0.0),
        translation
    );
    if (owner && owner->GetParent()) {
        glm::dmat4 parentWorld = owner->GetParent()->GetTransform()->matrix;
        matrix = parentWorld * local_matrix;
    }
    else {
        matrix = local_matrix;
    }
    position = glm::dvec3(matrix[3]);
}

// LookAt: orient the object to face a target point.
// Note: glm::lookAt produces a view matrix, so we invert it.
void Transform_Component::LookAt(const glm::dvec3& target)
{
    glm::dmat4 view = glm::lookAt(position, target, up);
    glm::dmat4 world = glm::inverse(view);
    if (owner && owner->GetParent()) {
        glm::dmat4 parentWorld = owner->GetParent()->GetTransform()->matrix;
        local_matrix = glm::inverse(parentWorld) * world;
        matrix = parentWorld * local_matrix;
    }
    else {
        local_matrix = world;
        matrix = world;
    }
    position = glm::dvec3(matrix[3]);
}

// AlignToGlobalUp: reorients the rotation so that the local "up" aligns with a specified world up.
void Transform_Component::AlignToGlobalUp(const glm::vec3& worldUp)
{
    glm::dvec3 fwd = glm::normalize(forward);
    glm::dvec3 right = glm::normalize(glm::cross(glm::dvec3(worldUp), fwd));
    glm::dvec3 newUp = glm::cross(fwd, right);

    left = right;
    up = newUp;
    forward = fwd;

    glm::dvec4 translation = local_matrix[3];
    local_matrix = glm::dmat4(
        glm::dvec4(right, 0.0),
        glm::dvec4(newUp, 0.0),
        glm::dvec4(fwd, 0.0),
        translation
    );
    if (owner && owner->GetParent()) {
        glm::dmat4 parentWorld = owner->GetParent()->GetTransform()->matrix;
        matrix = parentWorld * local_matrix;
    }
    else {
        matrix = local_matrix;
    }
    position = glm::dvec3(matrix[3]);
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