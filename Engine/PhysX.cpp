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
	foundation = PxCreateFoundation(PX_FOUNDATION_VERSION, allocator, errorCallBack);
	pvd = PxCreatePvd(*foundation);
	auto transport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	auto res = pvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

	PxTolerancesScale tolerancesScale;
	tolerancesScale.length = 100;
	tolerancesScale.speed = 981;
	physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, tolerancesScale, true, pvd);

	PxSceneDesc sceneDesc(physics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.f, -9.81f, 0.f);
	dispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = dispatcher;
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	scene = physics->createScene(sceneDesc);

	auto pvdClient = scene->getScenePvdClient();

	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);

	}

	return CreateSimulation();
}

HRESULT PhysX::CreateSimulation()
{
	material = physics->createMaterial(0.5f, 0.5f, 0.6f);
	auto groundPlane = PxCreatePlane(*physics, PxPlane(0, 1, 0, 1), *material);
	scene->addActor(*groundPlane);

	float halfExtent = 0.5f;
	PxU32 size = 20;

	const PxTransform t(PxVec3(0));

	PxShape* shape = physics->createShape(PxBoxGeometry(halfExtent, halfExtent, halfExtent), *material);
	for (PxU32 i = 0; i < size; i++)
	{
		for (PxU32 j = 0; j < size - i; j++)
		{
			PxTransform localTm(PxVec3(PxReal(j * 2) - PxReal(size - i), PxReal(i * 2 + 1), 0) * halfExtent);
			PxRigidDynamic* body = physics->createRigidDynamic(t.transform(localTm));
			body->attachShape(*shape);
			PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
			scene->addActor(*body);
		}
	}
	shape->release();

	return S_OK;
}

void PhysX::RunSimulate()
{
	scene->simulate(1.f / 60.f);
	scene->fetchResults(true); 
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
