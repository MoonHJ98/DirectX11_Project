#pragma once

#include "Includes.h"

class TerrainBuffer;

class ContactCallBack : public PxSimulationEventCallback
{
	// PxSimulationEventCallback을(를) 통해 상속됨
	virtual void onConstraintBreak(PxConstraintInfo * constraints, PxU32 count) override;
	virtual void onWake(PxActor ** actors, PxU32 count) override;
	virtual void onSleep(PxActor ** actors, PxU32 count) override;
	virtual void onContact(const PxContactPairHeader & pairHeader, const PxContactPair * pairs, PxU32 nbPairs) override;
	virtual void onTrigger(PxTriggerPair * pairs, PxU32 count) override;
	virtual void onAdvance(const PxRigidBody * const * bodyBuffer, const PxTransform * poseBuffer, const PxU32 count) override;
};


class PhysXManager
{
	SINGLETON(PhysXManager)

public:
	HRESULT Initialize();
	HRESULT CreateSimulation();
	PxRigidDynamic* AddRigidbody(Vector3 _pos);
	PxShape* AddCollider(PxGeometryType::Enum _type, ColliderDesc _desc);
	void SetupFiltering(PxRigidActor* actor, PxU32 filterGroup, PxU32 filterMask);
	void CreateHeightField(shared_ptr<TerrainBuffer> _terrainBuffer);

public:
	void RunSimulate(float _timeDelta);


private:
	PxDefaultAllocator allocator;
	PxDefaultErrorCallback errorCallBack;
	PxFoundation* foundation = nullptr;
	PxPhysics* physics = nullptr;
	PxDefaultCpuDispatcher* dispatcher = nullptr;
	PxPvd* pvd = nullptr;
	PxScene* scene = nullptr;
	PxMaterial* material = nullptr;
	PxCooking* cooking = nullptr;
};

