#pragma once
#ifndef __MONOENVIRONMENT_H__
#define __MONOENVIRONMENT_H__

//#include "SharpBinder.h"
////#include "../MyGameEditor/Log.h" // ilegal
//
//
//using namespace SharpBinder;

class MonoEnvironment
{
public:
	// TODO move all of this to cpp
	MonoEnvironment();

	

	~MonoEnvironment();

	void LinkEngineMethods();

	private:
		// Methods to test C#stuff , delete these at the end

		/// <summary>
		/// C# whould call theese but we are testing that C# calls them correctl by in C++ calling C# which calls C++
		/// </summary>
		/*void CreateGO() {
			MonoClass* klass = mono_class_from_name(m_ptr_GameAssemblyImg, "HawkEngine", "EngineCalls");
			MonoMethod* method = mono_class_get_method_from_name(klass, "CreateGameObject", 1);

			MonoString* arg = mono_string_new(mono_domain_get(), "Samson");
			void* args[1] = { arg };

			mono_runtime_invoke(method, nullptr, args, nullptr);

		}*/

public :


	//inline static MonoDomain* m_ptr_MonoDomain;
	//inline static MonoAssembly* m_ptr_GameAssembly;
	//inline static MonoImage* m_ptr_GameAssemblyImg;

	//std::vector<MonoClass*> user_classes; // array containing all the C# types metadata

};



#endif