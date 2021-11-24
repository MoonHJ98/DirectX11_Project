#pragma once

#include "Includes.h"

#include "PxPhysics.h"
#include "PxPhysicsAPI.h"


using namespace physx;

class PhysX
{
private:
	PhysX();
	PhysX(const PhysX& Rhs);

public:
	~PhysX();

private:
	HRESULT Initialize();
	HRESULT CreateSimulation();

public:
	void RunSimulate();

public:
	static shared_ptr<PhysX> Create();

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

