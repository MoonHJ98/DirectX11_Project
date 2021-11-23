#pragma once

#include <Windows.h>
#include <memory>
#include "PxPhysics.h"
#include "PxPhysicsAPI.h"

#pragma comment(lib, "PxFoundationDEBUG_x86.lib")
#pragma comment(lib, "PhysX3ExtensionsDEBUG.lib")
#pragma comment(lib, "PhysX3DEBUG_x86.lib")
#pragma comment(lib, "PxPvdSDKDEBUG_x86.lib")
#define MSG_BOX(MESSAGE) MessageBox(0, TEXT(MESSAGE), TEXT("System Message"), MB_OK)

using namespace std;
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

public:
	static shared_ptr<PhysX> Create();

private:
	PxDefaultAllocator allocator;
	PxDefaultErrorCallback* errorCallBack = nullptr;
	PxFoundation* foundation = nullptr;
	PxPhysics* physics = nullptr;
	PxDefaultCpuDispatcher* dispatcher = nullptr;
	PxPvd* pvd = nullptr;

};

