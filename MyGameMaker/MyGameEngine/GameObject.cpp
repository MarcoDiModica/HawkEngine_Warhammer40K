#include "GameObject.h"
#include "Component.h"

GameObject::GameObject(const std::string& name, const std::string& tag, bool active)
	: name(name), tag(tag), active(active), destroyed(false)
{
}