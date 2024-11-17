//#include "Scene.h"
//
//#include "MeshRendererComponent.h"
//#include "Mesh.h"
//
//void Scene::Start()
//{
//	for (auto& child : children())
//	{
//		child.Start();
//	}
//}
//
//void Scene::Update(float deltaTime)
//{
//	for (auto& child : children())
//	{
//		child.Update(deltaTime);
//	}
//}
//
//void Scene::Destroy()
//{
//	for (auto& child : children())
//	{
//		child.Destroy();
//	}
//}
//
//void Scene::OnEnable()
//{
//	for (auto& child : children())
//	{
//		child.OnEnable();
//	}
//}
//
//void Scene::OnDisable()
//{
//	for (auto& child : children())
//	{
//		child.OnDisable();
//	}
//}
//
//void Scene::RemoveGameObject(std::shared_ptr<GameObject> gameObject)
//{
//	//removeChild(*gameObject);
//}
//
//void Scene::AddGameObject(std::shared_ptr<GameObject> gameObject)
//{
//	emplaceChild(*gameObject);
//}
//
//std::shared_ptr<GameObject> Scene::FindGameObjectbyName(const std::string& name) 
//{
//	for (auto& child : children())
//	{
//		if (child.GetName() == name)
//		{
//			return std::make_shared<GameObject>(child);
//		}
//	}
//	return nullptr;
//}
//
//std::shared_ptr<GameObject> Scene::FindGameObjectbyTag(const std::string& tag)
//{
//	for (auto& child : children())
//	{
//		if (child.CompareTag(tag))
//		{
//			return std::make_shared<GameObject>(child);
//		}
//	}
//	return nullptr;
//}
//
//std::string Scene::GetName() const
//{
//	return name;
//}
//
//void Scene::SetName(const std::string& name)
//{
//	this->name = name;
//}
//
////std::shared_ptr<GameObject> Scene::CreateEmptyGameObject(const std::string& name)
////{
////	auto object = std::make_shared<GameObject>(name);
////	return object;
////}
////
////std::shared_ptr<GameObject> Scene::CreateCube(const std::string& name)
////{
////	auto object = std::make_shared<GameObject>(name);
////	object->AddComponent<MeshRenderer>();
////	auto meshRenderer = object->GetComponent<MeshRenderer>();
////	meshRenderer->SetMesh(Mesh::CreateCube());
////	return object;
////}
////
////std::shared_ptr<GameObject> Scene::CreateSphere(const std::string& name)
////{
////	auto object = std::make_shared<GameObject>(name);
////	object->AddComponent<MeshRenderer>();
////	auto meshRenderer = object->GetComponent<MeshRenderer>();
////	meshRenderer->SetMesh(Mesh::CreateSphere());
////	return object;
////}
////
////std::shared_ptr<GameObject> Scene::CreatePlane(const std::string& name)
////{
////	auto object = std::make_shared<GameObject>(name);
////	object->AddComponent<MeshRenderer>();
////	auto meshRenderer = object->GetComponent<MeshRenderer>();
////	meshRenderer->SetMesh(Mesh::CreatePlane());
////	return object;
////}