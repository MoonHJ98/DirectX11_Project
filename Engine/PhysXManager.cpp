#include "pch.h"
#include "PhysXManager.h"
#include "GameObject.h"
#include "TerrainBuffer.h"

PxFilterFlags FilterShader(
	PxFilterObjectAttributes attributes0, PxFilterData filterData0,
	PxFilterObjectAttributes attributes1, PxFilterData filterData1,
	PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{
	// let triggers through
	if (PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1))
	{
		pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
		return PxFilterFlag::eDEFAULT;
	}
	// generate contacts for all that were not filtered above
	pairFlags = PxPairFlag::eCONTACT_DEFAULT;

	// trigger the contact callback for pairs (A,B) where 
	// the filtermask of A contains the ID of B and vice versa.
	if ((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1))
		pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;


	return PxFilterFlag::eDEFAULT;
}

HRESULT PhysXManager::Initialize()
{
	foundation = PxCreateFoundation(PX_FOUNDATION_VERSION, allocator, errorCallBack);
	pvd = PxCreatePvd(*foundation);
	auto transport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	auto res = pvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

	PxTolerancesScale scale;

	physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, scale, true, pvd);

	PxSceneDesc sceneDesc(physics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.f, -9.81f, 0.f);
	dispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = dispatcher;
	sceneDesc.filterShader = FilterShader;
	sceneDesc.simulationEventCallback = new ContactCallBack();

	scene = physics->createScene(sceneDesc);

	PxCookingParams params(scale);
	params.meshWeldTolerance = 0.001f;
	params.meshPreprocessParams = PxMeshPreprocessingFlags(PxMeshPreprocessingFlag::eWELD_VERTICES);
	params.buildGPUData = true; //Enable GRB data being produced in cooking.
	cooking = PxCreateCooking(PX_PHYSICS_VERSION, *foundation, params);

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
	material = physics->createMaterial(0.5f, 0.5f, 0.1f);
	auto groundPlane = PxCreatePlane(*physics, PxPlane(0, 1, 0, 0), *material);
	SetupFiltering(groundPlane, FilterGroup::eHEIGHTFIELD, FilterGroup::eSUBMARINE);
	
	scene->addActor(*groundPlane);


	return S_OK;
}

PxRigidDynamic* PhysXManager::AddRigidbody(Vector3 _pos)
{
	PxTransform localTm(_pos.x, _pos.y, _pos.z);
	PxTransform transform(PxVec3(0));
	PxRigidDynamic* body = physics->createRigidDynamic(transform.transform(localTm));
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
		shape = physics->createShape(PxSphereGeometry(PxReal(_desc.radius)), *material, true);
		break;
	case physx::PxGeometryType::ePLANE:
		break;
	case physx::PxGeometryType::eCAPSULE:
		shape = physics->createShape(PxCapsuleGeometry(PxReal(_desc.radius), PxReal(_desc.halfHeight)), *material, true);
		break;
	case physx::PxGeometryType::eBOX:
		shape = physics->createShape(PxBoxGeometry(_desc.scale.x, _desc.scale.y, _desc.scale.z), *material, true);
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

	shape->setFlag(PxShapeFlag::eVISUALIZATION, true);
	shape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, true);
	return shape;
}

void PhysXManager::SetupFiltering(PxRigidActor * actor, PxU32 filterGroup, PxU32 filterMask)
{

	PxFilterData filterData;
	filterData.word0 = filterGroup; // word0 = own ID
	filterData.word1 = filterMask;	// word1 = ID mask to filter pairs that trigger a contact callback;
	const PxU32 numShapes = actor->getNbShapes();
	PxShape** shapes = (PxShape**)allocator.allocate(sizeof(PxShape*)*numShapes, 0, __FILE__, __LINE__);
	actor->getShapes(shapes, numShapes);
	for (PxU32 i = 0; i < numShapes; i++)
	{
		PxShape* shape = shapes[i];
		shape->setSimulationFilterData(filterData);
	}
	allocator.deallocate(shapes);

}

void PhysXManager::UpdateHeightField(shared_ptr<TerrainBuffer> _terrainBuffer)
{

	const PxReal heightScale = 1.f;
	const PxU32 hfSize = _terrainBuffer->GetTerrainWidth(); // some power of 2
	const PxU32 hfNumVerts = _terrainBuffer->GetVertexCount();

	const PxReal hfScale = 1.f; // this is how wide one heightfield square is

	PxHeightFieldSample* heightmap = (PxHeightFieldSample*)allocator.allocate(sizeof(PxHeightFieldSample)*hfNumVerts, 0, __FILE__, __LINE__);
	memset(heightmap, NULL, hfNumVerts * sizeof(PxHeightFieldSample));

	for (PxU32 z = 0; z < hfSize; z++)
	{
		for (PxU32 x = 0; x < hfSize; x++)
		{
			UINT index = hfSize * z + x;
			heightmap[index].height = (PxI16)_terrainBuffer.get()->GetVertices()[index].position.y;
			heightmap[index].setTessFlag();
			heightmap[index].materialIndex0 = 1;
			heightmap[index].materialIndex1 = 1;
		}
	}

	PxHeightFieldDesc hfDesc;
	hfDesc.format = PxHeightFieldFormat::eS16_TM;
	hfDesc.nbColumns = hfSize;
	hfDesc.nbRows = hfSize;
	hfDesc.samples.data = heightmap;
	hfDesc.samples.stride = sizeof(PxHeightFieldSample);

	PxHeightField* heightField = cooking->createHeightField(hfDesc, physics->getPhysicsInsertionCallback());

	PxTransform pose = PxTransform(PxIdentity);
	pose.p = PxVec3(0.f, 0.f, 0.f);

	heightFieldActor = physics->createRigidStatic(pose);

	PxHeightFieldGeometry hfGeom(heightField, PxMeshGeometryFlags(), heightScale, hfScale, hfScale);
	PxShape* hfShape = PxRigidActorExt::createExclusiveShape(*heightFieldActor, hfGeom, *material);
	heightFieldActor->setName("HeightField");
	
	scene->addActor(*heightFieldActor);
	allocator.deallocate(heightmap);
}

void PhysXManager::RunSimulate(float _timeDelta)
{
	scene->simulate(1.f / 60.f);

	scene->fetchResults(true);

}


void ContactCallBack::onConstraintBreak(PxConstraintInfo * constraints, PxU32 count)
{
}

void ContactCallBack::onWake(PxActor ** actors, PxU32 count)
{
}

void ContactCallBack::onSleep(PxActor ** actors, PxU32 count)
{
}

void ContactCallBack::onContact(const PxContactPairHeader & pairHeader, const PxContactPair * pairs, PxU32 nbPairs)
{
	for (PxU32 i = 0; i < nbPairs; i++)
	{
		const PxContactPair& cp = pairs[i];

		if (cp.events == PxPairFlag::eNOTIFY_TOUCH_FOUND)
		{
			
			GameObject* obj = nullptr;

			obj = reinterpret_cast<GameObject*>(pairHeader.actors[0]->userData);
			if(obj != nullptr)
				obj->OnContact();

			obj = reinterpret_cast<GameObject*>(pairHeader.actors[1]->userData);
			if (obj != nullptr)
				obj->OnContact();
	
		}
	}
}

void ContactCallBack::onTrigger(PxTriggerPair * pairs, PxU32 count)
{
}

void ContactCallBack::onAdvance(const PxRigidBody * const * bodyBuffer, const PxTransform * poseBuffer, const PxU32 count)
{
}

