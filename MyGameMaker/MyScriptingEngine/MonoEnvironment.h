#ifndef MONO_ENVIRONMENT
#define MONOENVIRONMENT
#pragma once

#include <vector>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/jit/jit.h>

class MonoEnvironment {

public:

	MonoEnvironment();

	void LinkEngineMethods();

	// Test methods

	void CreateGO();

	void DestroyGo();

	inline static MonoDomain * m_ptr_MonoDomain;
	inline static MonoAssembly* m_ptr_GameAssembly;
	inline static MonoImage* m_ptr_GameAssemblyImg;

	std::vector<MonoClass*> user_classes;

};


#endif