#include "Scene.h"

#include "MeshRendererComponent.h"
#include "Mesh.h"
//#include "../MyScriptingEngine/MonoEnvironment.h"

void Scene::Start()
{
	int u = 5;

	//MonoEnvironment* env = new MonoEnvironment();

	//for (auto& child : children())
	//{
	//	child->Start();
	//}
}

void Scene::Update(float deltaTime)
{

	for (auto& child : children())
	{
		child->Update(deltaTime);
	}



}

void Scene::Destroy()
{
	for (auto& child : _children) {
        RemoveGameObject(child);
    }
    _children.clear();
}

void Scene::OnEnable()
{
	for (auto& child : children())
	{
		child->OnEnable();
	}
}

void Scene::OnDisable()
{
	for (auto& child : children())
	{
		child->OnDisable();
	}
}

void Scene::RemoveGameObject(std::shared_ptr<GameObject> gameObject)
{
	_children.erase(std::remove(_children.begin(), _children.end(), gameObject), _children.end());

	gameObject->Destroy();
}

void Scene::AddGameObject(std::shared_ptr<GameObject> gameObject)
{
	gameObject->parent = nullptr;
	gameObject->scene = this;
	_children.push_back(gameObject);

	if (tree) {
		tree->Insert(tree->root, *_children[_children.size() - 1], 0);
	}

}

std::string Scene::GetName() const
{
	return name;
}

void Scene::SetName(const std::string& name)
{
	this->name = name;
}
void display() {



	BoundingBox box(vec3(-10, -1, -1), vec3(10, 1, 1));
	box.draw();


}
void Scene::DebugDrawTree() {
	//display();
	if (tree != nullptr) {
		tree->DebugDraw(tree->root);
	}
}