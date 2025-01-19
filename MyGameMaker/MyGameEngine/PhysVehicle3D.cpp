#include "PhysVehicle3D.h"
#include "../MyGameEditor/Root.h"
#include <Bullet/btBulletDynamicsCommon.h>
#include <glm/glm.hpp>

// ----------------------------------------------------------------------------
VehicleInfo::~VehicleInfo()
{
	//if(wheels != NULL)
		//delete wheels;
}

// ----------------------------------------------------------------------------
PhysVehicle3D::PhysVehicle3D(btRigidBody* body, btRaycastVehicle* vehicle, const VehicleInfo& info) : PhysBody3D(body), vehicle(vehicle), info(info)
{
}

// ----------------------------------------------------------------------------
PhysVehicle3D::~PhysVehicle3D()
{
	delete vehicle;
}

// ----------------------------------------------------------------------------
void PhysVehicle3D::Render()
{
     
         auto wheel1 = Application->root->CreateCylinder("wheel1");
         auto wheel2 = Application->root->CreateCylinder("wheel2");
         auto wheel3 = Application->root->CreateCylinder("wheel3");
         auto wheel4 = Application->root->CreateCylinder("wheel4");

         // Set manual positions for the wheels
         wheel1->GetTransform()->SetPosition(glm::dvec3(1.0, 0.5, 1.0));
         wheel2->GetTransform()->SetPosition(glm::dvec3(-1.0, 0.5, 1.0));
         wheel3->GetTransform()->SetPosition(glm::dvec3(1.0, 0.5, -1.0));
         wheel4->GetTransform()->SetPosition(glm::dvec3(-1.0, 0.5, -1.0));

         for(int i = 0; i < 4; ++i)
         {
             vehicle->updateWheelTransform(i);
         }

         auto chassis = Application->root->CreateCube("chassis");
         btQuaternion q = vehicle->getChassisWorldTransform().getRotation();
         btVector3 offset(info.chassis_offset.x, info.chassis_offset.y, info.chassis_offset.z);
         offset = offset.rotate(q.getAxis(), q.getAngle());

		

		 // Calculate the average position of the wheels
		 btVector3 avgWheelPos(0, 0, 0);
		 for (int i = 0; i < vehicle->getNumWheels(); ++i)
		 {
			 avgWheelPos += vehicle->getWheelInfo(i).m_worldTransform.getOrigin();
		 }
		 avgWheelPos /= vehicle->getNumWheels();


     

	//auto chassis = Application->root->CreateCube("chassis");
	////Cube chassis(info.chassis_size.x, info.chassis_size.y, info.chassis_size.z);
	////vehicle->getChassisWorldTransform().getOpenGLMatrix(&chassis.transform);
	//btQuaternion q = vehicle->getChassisWorldTransform().getRotation();
	//btVector3 offset(info.chassis_offset.x, info.chassis_offset.y, info.chassis_offset.z);
	//offset = offset.rotate(q.getAxis(), q.getAngle());

 //   chassis->GetTransform()->SetPosition(
 //       vehicle->getChassisWorldTransform().getOrigin().getX() + static_cast<float>(offset.getX()), 
 //       vehicle->getChassisWorldTransform().getOrigin().getY() + static_cast<float>(offset.getY()), 
 //       vehicle->getChassisWorldTransform().getOrigin().getZ() + static_cast<float>(offset.getZ())
 //   );


	//chassis.Render();
}

// ----------------------------------------------------------------------------
void PhysVehicle3D::ApplyEngineForce(float force)
{
	for(int i = 0; i < vehicle->getNumWheels(); ++i)
	{
		if(info.wheels[i].drive == true)
		{
			vehicle->applyEngineForce(force, i);
		}
	}
}

// ----------------------------------------------------------------------------
void PhysVehicle3D::Brake(float force)
{
	for(int i = 0; i < vehicle->getNumWheels(); ++i)
	{
		if(info.wheels[i].brake == true)
		{
			vehicle->setBrake(force,i);
		}
	}
}

// ----------------------------------------------------------------------------
void PhysVehicle3D::Turn(float degrees)
{
	for(int i = 0; i < vehicle->getNumWheels(); ++i)
	{
		if(info.wheels[i].steering == true)
		{
			vehicle->setSteeringValue(degrees, i);
		}
	}
}

// ----------------------------------------------------------------------------
float PhysVehicle3D::GetKmh() const
{
	return vehicle->getCurrentSpeedKmHour();
}

void PhysVehicle3D::ResetCarOrientation(float angle)
{
	float orientationMat[16];
	memset(orientationMat, 0.0f, sizeof(orientationMat));

	orientationMat[0] = cos(angle);
	orientationMat[2] = -sin(angle);
	orientationMat[5] = 1;
	orientationMat[8] = sin(angle);
	orientationMat[10] = cos(angle);

	SetTransform(orientationMat);
}