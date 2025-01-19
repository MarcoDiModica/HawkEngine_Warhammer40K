#ifndef __PhysBody3D_H__
#define __PhysBody3D_H__

#include "p2List.h"
#include <Bullet/LinearMath/btVector3.h>
class btRigidBody;
class Module;

// =================================================
struct PhysBody3D
{
	friend class ModulePhysics3D;
public:
	PhysBody3D(btRigidBody* body);
	~PhysBody3D();

	void Push(float x, float y, float z);
	void GetTransform(float* matrix) const;
	void SetTransform(const float* matrix) const;
	void SetPos(float x, float y, float z);
	btVector3 GetPos();
	void SetAsSensor(bool is_sensor);
	void SetId(int id);
private:
	
public:
	btRigidBody* body = nullptr;

	p2List<Module*> collision_listeners;
	bool isSensor;
	int id;
};

#endif // __PhysBody3D_H__