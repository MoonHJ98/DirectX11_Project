#pragma once

#include "Includes.h"


class PhysXManager
{
	SINGLETON(PhysXManager)

public:
	HRESULT Initialize();
	HRESULT CreateSimulation();
	PxRigidBody* AddDynamicRigidbody(Vector3 _pos);
	PxShape* AddCollider(PxGeometryType::Enum _type, ColliderDesc _desc);

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

