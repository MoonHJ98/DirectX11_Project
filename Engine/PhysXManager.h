#pragma once

#include "Includes.h"


class PhysXManager
{
	SINGLETON(PhysXManager)

public:
	HRESULT Initialize();
	HRESULT CreateSimulation();
	PxRigidBody* AddRigidbody(Vector3 _pos);
	PxShape* AddCollider(PxGeometryType::Enum _type, Vector3 _scale);

public:
	void RunSimulate();

private:
	PxDefaultAllocator allocator;
	PxDefaultErrorCallback errorCallBack;
	PxFoundation* foundation = nullptr;
	PxPhysics* physics = nullptr;
	PxDefaultCpuDispatcher* dispatcher = nullptr;
	PxPvd* pvd = nullptr;
	PxScene* scene = nullptr;
	PxMaterial* material = nullptr;
	

};

