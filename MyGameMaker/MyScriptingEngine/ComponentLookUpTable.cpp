#include "ComponentLookUpTable.h"
#include <iostream>
#include "../MyGameEditor/Log.h" // ilegal

struct A
{
    virtual ~A() {}
};
struct B : A {};
struct C : A {};

void SharpBinder::InitializeLookUpTable() {
	/*PlusLookUpTable["HawkEngine.Transform"] = typeid( Transform_Component );
	PlusLookUpTable["HawkEngine.Mesh"] = typeid(Mesh);*/
	//// Add the rest of the components

	//// SharpLookUP 
    const auto pair = std::pair<std::type_index, std::string >(std::type_index(typeid(Transform_Component)), std::string("HawkEngine.Transform"));
    SharpLookUpTable.insert(pair );
	/*SharpLookUpTable[std::type_index(typeid(Transform_Component))] = "HawkEngine.Transform";
	SharpLookUpTable[std::type_index(typeid(MeshRenderer))] = "HawkEngine.Mesh";*/

    const auto pair2 = std::pair<std::type_index, int >(std::type_index(typeid(Transform_Component)), 2);
    SharpLookUpTable2.insert(pair2);

	auto t = std::type_index(typeid(Transform_Component));
    return;
    std::unordered_map<std::type_index, std::string> type_names;

    type_names[std::type_index(typeid(int))] = "int";
    type_names[std::type_index(typeid(double))] = "double";
    type_names[std::type_index(typeid(A))] = "A";
    type_names[std::type_index(typeid(B))] = "B";
    type_names[std::type_index(typeid(C))] = "C";

    type_names[std::type_index(typeid(Transform_Component))] = "transform";

    int i;
    double d;
    A a;

    // note that we're storing pointer to type A
    std::unique_ptr<A> b(new B);
    std::unique_ptr<A> c(new C);

    LOG(LogType::LOG_INFO ,  type_names[std::type_index(typeid(i))].c_str() );
    LOG(LogType::LOG_INFO ,  type_names[std::type_index(typeid(d))].c_str());
    LOG(LogType::LOG_INFO ,  type_names[std::type_index(typeid(A))].c_str());
    LOG(LogType::LOG_INFO, type_names[std::type_index(typeid(Transform_Component))].c_str());
    LOG(LogType::LOG_INFO, SharpLookUpTable[std::type_index(typeid(Transform_Component))].c_str());
    std::string s = SharpLookUpTable[std::type_index(typeid(Transform_Component))];
    int u = 3;
   /* LOG(LogType::LOG_INFO , type_names[std::type_index(typeid(*b))].c_str());
    LOG(LogType::LOG_INFO , type_names[std::type_index(typeid(*c))].c_str());
    LOG(LogType::LOG_INFO, type_names[std::type_index(typeid(*c))].c_str());*/
}