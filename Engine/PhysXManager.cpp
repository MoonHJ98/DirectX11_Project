
#include "pch.h"
#include "PhysXManager.h"

HRESULT PhysXManager::Initialize()
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

HRESULT PhysXManager::CreateSimulation()
{
	material = physics->createMaterial(0.5f, 0.5f, 0.6f);
	auto groundPlane = PxCreatePlane(*physics, PxPlane(0, 1, 0, 1), *material);
	scene->addActor(*groundPlane);

	//float halfExtent = 0.5f;
	//PxU32 size = 20;
	//
	//const PxTransform t(PxVec3(0));
	//
	//PxShape* shape = physics->createShape(PxBoxGeometry(1.f, 1.f, 1.f), *material);
	//for (PxU32 i = 0; i < size; i++)
	//{
	//	for (PxU32 j = 0; j < size - i; j++)
	//	{
	//		PxTransform localTm(PxVec3(PxReal(j * 2) - PxReal(size - i), PxReal(i * 2 + 1) + 50, 0));
	//		PxRigidDynamic* body = physics->createRigidDynamic(t.transform(localTm));
	//		body->attachShape(*shape);
	//		PxRigidBodyExt::updateMassAndInertia(*body, 10.f);
	//		scene->addActor(*body);
	//	}
	//}
	//shape->release();

	return S_OK;
}

PxRigidBody* PhysXManager::AddDynamicRigidbody(Vector3 _pos)
{
	PxTransform localTm(_pos.x, _pos.y, _pos.z);
	PxTransform transform(PxVec3(0));
	PxRigidBody* body = physics->createRigidDynamic(transform.transform(localTm));
	PxRigidBodyExt::updateMassAndInertia(*body, 10.f);
	scene->addActor(*body);

	return body;
}


PxShape* PhysXManager::AddCollider(PxGeometryType::Enum _type, ColliderDesc _desc)
{
	PxShape* shape = nullptr;
	
	switch (_type)
	{
	case physx::PxGeometryType::eSPHERE:
		shape = physics->createShape(PxSphereGeometry(PxReal(_desc.radius)), *material);
		break;
	case physx::PxGeometryType::ePLANE:
		break;
	case physx::PxGeometryType::eCAPSULE:
		shape = physics->createShape(PxCapsuleGeometry(PxReal(_desc.radius), PxReal(_desc.halfHeight)), *material);
		break;
	case physx::PxGeometryType::eBOX:
		shape = physics->createShape(PxBoxGeometry(_desc.scale.x, _desc.scale.y, _desc.scale.z), *material);
		break;
	case physx::PxGeometryType::eHEIGHTFIELD:
		break;
	case physx::PxGeometryType::eGEOMETRY_COUNT:
		break;
	case physx::PxGeometryType::eINVALID:
		break;
	default:
		break;
	}

	return shape;
}

void PhysXManager::RunSimulate()
{
	scene->simulate(1.f / 60.f);
	scene->fetchResults(true); 
}
