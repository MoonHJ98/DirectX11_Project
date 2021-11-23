#include "pch.h"
#include "PhysX.h"

PhysX::PhysX()
{
}

PhysX::PhysX(const PhysX & Rhs)
{
}

PhysX::~PhysX()
{
}

HRESULT PhysX::Initialize()
{
	foundation = PxCreateFoundation(PX_FOUNDATION_VERSION, allocator, *errorCallBack);
	pvd = PxCreatePvd(*foundation);
	auto transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	auto res = pvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);
	physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, physx::PxTolerancesScale(), true, pvd);
	dispatcher = physx::PxDefaultCpuDispatcherCreate(1);

	return S_OK;
}

shared_ptr<PhysX> PhysX::Create()
{
	shared_ptr<PhysX> Instance(new PhysX());
	if (FAILED(Instance->Initialize()))
	{
		MSG_BOX("Failed to create PhysX.");
		return nullptr;
	}
	return Instance;
}
