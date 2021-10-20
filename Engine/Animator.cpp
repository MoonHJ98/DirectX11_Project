#include "pch.h"
#include "Animator.h"
#include "Clip.h"
#include "Model.h"
#include "Bone.h"
#include "BinaryFile.h"
#include "GraphicDevice.h"

Animator::Animator()
{
}

Animator::Animator(const Animator & Rhs)
{
}

Animator::~Animator()
{
}

void Animator::Update(float _timeDelta)
{
	if (texture == nullptr)
		CreateTexture();

	KeyframeDesc& desc = keyframeDesc;
	
	// 현재 애니메이션.
	{
		auto& clip = clips[desc.clip];
		desc.RunningTime += _timeDelta;

		// 시간 비율
		float time = 1.f / clip->GetFrameRate() / desc.Speed;

		// desc.Time이 1보다 크거나 같으면 애니메이션의 다음 프레임
		if (desc.Time >= 1.f)
		{
			desc.RunningTime = 0.f;

			// 루프. 한번만 플레이하고 싶다면 마지막 프레임에서 멈추면 됨
			desc.CurrFrame = (desc.CurrFrame + 1) % clip->GetFrameCount();
		}
		desc.Time = desc.RunningTime / time;

	}
}

void Animator::Render()
{
	keyframeBuffer->SetData(Graphic->GetDeviceContext(), keyframeDesc);
	auto buffer = keyframeBuffer->GetBuffer();
	Graphic->GetDeviceContext()->VSSetConstantBuffers(2, 1, &buffer);
}

shared_ptr<Clip> Animator::ClipByName(wstring name)
{
	for (auto& clip : clips)
	{
		if (clip->GetName() == name)
			return clip;
	}

	return nullptr;
}

void Animator::ReadClip(wstring file)
{
	file = L"../Resources/" + file + L".clip";

	BinaryReader* r = new BinaryReader();
	r->Open(file);


	wstring name = ToWString(r->ReadString());
	float duration = r->ReadFloat();
	float frameRate = r->ReadFloat();
	UINT frameCount = r->ReadUInt();

	auto clip = Clip::Create(name, duration, frameRate, frameCount);


	UINT keyframesCount = r->ReadUInt();
	for (UINT i = 0; i < keyframesCount; i++)
	{
		shared_ptr<ModelKeyframe> keyframe(new ModelKeyframe());
		keyframe->BoneName = ToWString(r->ReadString());


		UINT size = r->ReadUInt();
		if (size > 0)
		{
			keyframe->Transforms.assign(size, ModelKeyframeData());

			void* ptr = (void *)&keyframe->Transforms[0];
			r->ReadByte(&ptr, sizeof(ModelKeyframeData) * size);
		}
		 
		clip->SetKeyframe(keyframe->BoneName, keyframe);
	}

	r->Close();
	SAFEDELETE(r);

	clips.push_back(clip);
}

void Animator::CreateTexture()
{
	/*
	스택메모리의 크기는 윈도우에서 일반적으로 2MB 정도로 할당됨.
	memcpy의 크기도 스택 메모리에 의존해서 2MB이상 사용할 수 없으므로
	VirtualAlloc을 사용함.
	*/
	//Matrix matrix[MAX_MODEL_KEYFRAMES][MAX_MODEL_TRANSFORMS];

	// 2차원 배열을 동작할당해서 3차원 배열로 만든다.
	//shared_ptr<ClipTransform[]> temp(new ClipTransform[clips.size()]);
	//clipTransforms = temp;
	
	clipTransforms = new ClipTransform[clips.size()];

	// 클립별로 순회하면서 처리.
	for (UINT i = 0; i < clips.size(); i++)
		CreateClipTransform(i);


	/*
	행렬 하나의 크기 = 64 Byte
	픽섹 하나에 쓸 수 있는 데이터의 크기 = 16 Byte

	행렬 하나를 4개로 나눠서(16 Byte) 픽셀에 저장하고 쉐이더에서 다시 행렬로 만들 것. 그래서 넓이 = desc.Width = MAX_MODEL_TRANSFORMS * 4;

	*/
	//Create Texture
	{
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
		desc.Width = MAX_MODEL_TRANSFORMS * 4;
		desc.Height = MAX_MODEL_KEYFRAMES;
		desc.ArraySize = clips.size();
		desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; // 픽셀 하나당 16Byte를 쓸 수 있음. 16Byte * 4 = 64Byte
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.MipLevels = 1;
		desc.SampleDesc.Count = 1;

		UINT pageSize = MAX_MODEL_TRANSFORMS * 4 * 16 * MAX_MODEL_KEYFRAMES;
		//void* p = malloc(pageSize * model->ClipCount());  // malloc하면 stackoverflow
		void* p = VirtualAlloc(NULL, pageSize * clips.size(), MEM_RESERVE, PAGE_READWRITE);
		/*
		MEM_RESERVE: 예약하겠다.
		PAGE_READWRITE : 예약한 곳은 읽고 쓸수 있게 열어두겠다.

		
		VirtualQuery(실제로 할당된 크기를 알아낼 수 있음) 리턴값 : MEMORY_BASIC_INFORMATION
		*/ 
		
		for (UINT c = 0; c < clips.size(); c++)
		{
			// 각 클립의 시작 번호
			UINT start = c * pageSize;

			for (UINT k = 0; k < MAX_MODEL_KEYFRAMES; k++)
			{
				// 시작 주소
				void* temp = (BYTE *)p + MAX_MODEL_TRANSFORMS * k * sizeof(Matrix) + start;

				// 위의 VirtualAlloc은 예약. 여기는 실제 할당.
				VirtualAlloc(temp, MAX_MODEL_TRANSFORMS * sizeof(Matrix), MEM_COMMIT, PAGE_READWRITE);
				memcpy(temp, clipTransforms[c].Transform[k], MAX_MODEL_TRANSFORMS * sizeof(Matrix));
			}
		}//for(c)


		D3D11_SUBRESOURCE_DATA* subResources = new D3D11_SUBRESOURCE_DATA[clips.size()];
		for (UINT c = 0; c < clips.size(); c++)
		{
			void* temp = (BYTE *)p + c * pageSize;

			subResources[c].pSysMem = temp; // 한 페이지의 시작주소
			subResources[c].SysMemPitch = MAX_MODEL_TRANSFORMS * sizeof(Matrix); // 가로 줄의 크기
			subResources[c].SysMemSlicePitch = pageSize; // 한 장의 크기. SysMemPitch, SysMemSlicePitch는 텍스쳐가 한 장 일때는 필요없음
		}

		Graphic->GetDevice()->CreateTexture2D(&desc, subResources, texture.GetAddressOf());


		SAFEDELETEARRAY(subResources);
		VirtualFree(p, 0, MEM_RELEASE);
	}


	/*
	셰이더 리소스 뷰
	위에서 만든 데이터를 셰이더에서 어떻게 받아서 어떤 형식으로 받아서 사용할까를 결정해주는 것.
	이 메모리 형식이 어떻게 돼있느냐를 셰이더에게 알려주기 위해 사용하는 것.
	*/
	//Create SRV
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY; // 리소스의 형식
		desc.Texture2DArray.MipLevels = 1; // 디멘션과 맞는 desc의 멤버변수에 값을 넣어줘야함.
		desc.Texture2DArray.ArraySize = clips.size();

		Graphic->GetDevice()->CreateShaderResourceView(texture.Get(), &desc, srv.GetAddressOf());
	}
	auto tempModel = model.lock();
	tempModel->SetTransformsSRV(srv);

}

void Animator::CreateClipTransform(UINT index)
{
	Matrix* bones = new Matrix[MAX_MODEL_TRANSFORMS];

	auto tempModel = model.lock();

	shared_ptr<Clip> clip = clips[index];

	for (UINT f = 0; f < clip->GetFrameCount(); f++)
	{
		for (UINT b = 0; b < tempModel->GetBoneCount(); b++)
		{
			shared_ptr<Bone> bone = tempModel->GetBoneByIndex(b);


			Matrix parent;
			Matrix invGlobal = *bone->GetTransform();
			invGlobal = XMMatrixInverse(nullptr, invGlobal);


			int parentIndex = bone->GetParentIndex();
			if (parentIndex < 0)
				parent = XMMatrixIdentity();
			else
				parent = bones[parentIndex];


			Matrix animation;
			shared_ptr<ModelKeyframe> frame = clip->GetKeyframe(bone->GetName());


			if (frame != nullptr)
			{
				ModelKeyframeData& data = frame->Transforms[f];

				Matrix S, R, T;
				S = XMMatrixScaling(data.Scale.x, data.Scale.y, data.Scale.z);
				R = XMMatrixRotationQuaternion(data.Rotation);
				T = XMMatrixTranslation(data.Translation.x, data.Translation.y, data.Translation.z);

				animation = S * R * T;
			}
			else
				animation = XMMatrixIdentity();

			/*
			animation 행렬 : 해당 프레임에 해당 본이 얼마만큼 움직일지 결정

			animation : Local(Relative)
			parent : Global (AssimpConverter에서 만든 값.)
			bone[b] = Local(Relative) * Global
			bone[b] : Global

			clipTransforms[index].Transform[f][b] = invGlobal * bones[b];
			= Global * Global

			   50       20        50
			-------> A ---->  B ---->

			70(Global) + Local(50) = 120(Global)

			Global * Global == 70(Global) * 120(Global)이 되게 때문에 Global의 역행렬을 위해줌 invGlobal = XMMatrixInverse(nullptr, invGlobal);
			Global 의 역행렬 = Local(Relative)
			그러면 Local(Relative) * Global이 성립이 됨
			*/
			bones[b] = animation * parent;
			clipTransforms[index].Transform[f][b] = invGlobal * bones[b];
		}//for(b)
	}//for(f)
}

HRESULT Animator::Initialize(shared_ptr<Model> _model)
{
	Graphic = GraphicDevice::GetInstance();
	model = _model;
	shared_ptr<ConstantBuffer<KeyframeDesc>> temp(new ConstantBuffer<KeyframeDesc>);
	keyframeBuffer = temp;
	keyframeBuffer->Create(Graphic->GetDevice());

	return S_OK;
}

shared_ptr<Animator> Animator::Create(shared_ptr<Model> _model)
{
	shared_ptr<Animator> Instance(new Animator());

	if (FAILED(Instance->Initialize(_model)))
	{
		MSG_BOX("Failed to create Animator.");
		return nullptr;
	}
	return Instance;
}
