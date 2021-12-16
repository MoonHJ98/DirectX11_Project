#pragma once

#ifdef max
#undef max
#undef min
#endif

#define VSYNC_ENABLE true
#define FULLSCREEN false
#define SCREENSIZEX 800
#define SCREENSIZEY 600

#define SCREENDEPTH 1000.f
#define SCREENNEAR 0.1f
#define FoV 3.141592654f / 4.f

#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <d3dtypes.h>
#include <d3dcompiler.h>
#include <memory>
#include <wrl.h>
#include <map>
#include <unordered_map>
#include <vector>
#include <list>
#include <fstream>
#include <iostream>
#include <stdio.h>

#include <DirectXTK/BufferHelpers.h>
#include <DirectXTK/SimpleMath.h>
#include <DirectXTK/VertexTypes.h>
#include <DirectXTK/GeometricPrimitive.h>
#include "DirectXTex.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "ImGuiFileDialog.h"

#include "PxPhysics.h"
#include "PxPhysicsAPI.h"




#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#pragma comment(lib, "assimp-vc140-mt.lib")
#include <d3dcompiler.h>


#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "DirectXTK.lib")
#pragma comment(lib, "DirectXTex.lib")
#pragma warning( disable : 4996)

#pragma comment(lib, "PxFoundationDEBUG_x86.lib")
#pragma comment(lib, "PhysX3ExtensionsDEBUG.lib")
#pragma comment(lib, "PhysX3DEBUG_x86.lib")
#pragma comment(lib, "LowLevelAABBDEBUG.lib")
#pragma comment(lib, "LowLevelClothDEBUG.lib")
#pragma comment(lib, "LowLevelDEBUG.lib")
#pragma comment(lib, "LowLevelDynamicsDEBUG.lib")
#pragma comment(lib, "LowLevelParticlesDEBUG.lib")
#pragma comment(lib, "PhysX3CharacterKinematicDEBUG_x86.lib")
#pragma comment(lib, "PhysX3CommonDEBUG_x86.lib")
#pragma comment(lib, "PhysX3CookingDEBUG_x86.lib")
#pragma comment(lib, "PhysX3VehicleDEBUG.lib")
#pragma comment(lib, "PsFastXmlDEBUG_x86.lib")
#pragma comment(lib, "PxPvdSDKDEBUG_x86.lib")
#pragma comment(lib, "PxTaskDEBUG_x86.lib")
#pragma comment(lib, "SceneQueryDEBUG.lib")
#pragma comment(lib, "SimulationControllerDEBUG.lib")



using namespace std;
using namespace DirectX;
using namespace Microsoft::WRL;
using namespace SimpleMath;
using namespace PackedVector;
using namespace physx;


#define MSG_BOX(MESSAGE) MessageBox(0, TEXT(MESSAGE), TEXT("System Message"), MB_OK)
#define SINGLETON(CLASSNAME)													\
			private:																	\
			CLASSNAME() {}																\
			CLASSNAME(const CLASSNAME& Rhs) = delete;									\
			CLASSNAME& operator=(const CLASSNAME& Rhs) = delete;						\
			public:																		\
			static shared_ptr<CLASSNAME> GetInstance()									\
			{																			\
					static shared_ptr<CLASSNAME> instance{ new CLASSNAME};				\
					return instance;													\
			};																						

#define SAFEDELETE(Instance) { if(Instance) {delete Instance; Instance = nullptr;}}
#define SAFEDELETEARRAY(Instance){ if(Instance){ delete [] Instance; Instance = nullptr; } }

#define MAX_MODEL_TRANSFORMS 250
#define MAX_MODEL_KEYFRAMES 500

#define ALLOC(x)	getSampleAllocator()->allocate(x, 0, __FILE__, __LINE__)
#define	FREE(x)	if(x)	{ getSampleAllocator()->deallocate(x); x = NULL;	}
#define NEW(x)	new(#x, __FILE__, __LINE__) x


#include "Struct.h"