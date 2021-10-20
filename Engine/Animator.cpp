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
	
	// ���� �ִϸ��̼�.
	{
		auto& clip = clips[desc.clip];
		desc.RunningTime += _timeDelta;

		// �ð� ����
		float time = 1.f / clip->GetFrameRate() / desc.Speed;

		// desc.Time�� 1���� ũ�ų� ������ �ִϸ��̼��� ���� ������
		if (desc.Time >= 1.f)
		{
			desc.RunningTime = 0.f;

			// ����. �ѹ��� �÷����ϰ� �ʹٸ� ������ �����ӿ��� ���߸� ��
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
	���ø޸��� ũ��� �����쿡�� �Ϲ������� 2MB ������ �Ҵ��.
	memcpy�� ũ�⵵ ���� �޸𸮿� �����ؼ� 2MB�̻� ����� �� �����Ƿ�
	VirtualAlloc�� �����.
	*/
	//Matrix matrix[MAX_MODEL_KEYFRAMES][MAX_MODEL_TRANSFORMS];

	// 2���� �迭�� �����Ҵ��ؼ� 3���� �迭�� �����.
	//shared_ptr<ClipTransform[]> temp(new ClipTransform[clips.size()]);
	//clipTransforms = temp;
	
	clipTransforms = new ClipTransform[clips.size()];

	// Ŭ������ ��ȸ�ϸ鼭 ó��.
	for (UINT i = 0; i < clips.size(); i++)
		CreateClipTransform(i);


	/*
	��� �ϳ��� ũ�� = 64 Byte
	�ȼ� �ϳ��� �� �� �ִ� �������� ũ�� = 16 Byte

	��� �ϳ��� 4���� ������(16 Byte) �ȼ��� �����ϰ� ���̴����� �ٽ� ��ķ� ���� ��. �׷��� ���� = desc.Width = MAX_MODEL_TRANSFORMS * 4;

	*/
	//Create Texture
	{
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
		desc.Width = MAX_MODEL_TRANSFORMS * 4;
		desc.Height = MAX_MODEL_KEYFRAMES;
		desc.ArraySize = clips.size();
		desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; // �ȼ� �ϳ��� 16Byte�� �� �� ����. 16Byte * 4 = 64Byte
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.MipLevels = 1;
		desc.SampleDesc.Count = 1;

		UINT pageSize = MAX_MODEL_TRANSFORMS * 4 * 16 * MAX_MODEL_KEYFRAMES;
		//void* p = malloc(pageSize * model->ClipCount());  // malloc�ϸ� stackoverflow
		void* p = VirtualAlloc(NULL, pageSize * clips.size(), MEM_RESERVE, PAGE_READWRITE);
		/*
		MEM_RESERVE: �����ϰڴ�.
		PAGE_READWRITE : ������ ���� �а� ���� �ְ� ����ΰڴ�.

		
		VirtualQuery(������ �Ҵ�� ũ�⸦ �˾Ƴ� �� ����) ���ϰ� : MEMORY_BASIC_INFORMATION
		*/ 
		
		for (UINT c = 0; c < clips.size(); c++)
		{
			// �� Ŭ���� ���� ��ȣ
			UINT start = c * pageSize;

			for (UINT k = 0; k < MAX_MODEL_KEYFRAMES; k++)
			{
				// ���� �ּ�
				void* temp = (BYTE *)p + MAX_MODEL_TRANSFORMS * k * sizeof(Matrix) + start;

				// ���� VirtualAlloc�� ����. ����� ���� �Ҵ�.
				VirtualAlloc(temp, MAX_MODEL_TRANSFORMS * sizeof(Matrix), MEM_COMMIT, PAGE_READWRITE);
				memcpy(temp, clipTransforms[c].Transform[k], MAX_MODEL_TRANSFORMS * sizeof(Matrix));
			}
		}//for(c)


		D3D11_SUBRESOURCE_DATA* subResources = new D3D11_SUBRESOURCE_DATA[clips.size()];
		for (UINT c = 0; c < clips.size(); c++)
		{
			void* temp = (BYTE *)p + c * pageSize;

			subResources[c].pSysMem = temp; // �� �������� �����ּ�
			subResources[c].SysMemPitch = MAX_MODEL_TRANSFORMS * sizeof(Matrix); // ���� ���� ũ��
			subResources[c].SysMemSlicePitch = pageSize; // �� ���� ũ��. SysMemPitch, SysMemSlicePitch�� �ؽ��İ� �� �� �϶��� �ʿ����
		}

		Graphic->GetDevice()->CreateTexture2D(&desc, subResources, texture.GetAddressOf());


		SAFEDELETEARRAY(subResources);
		VirtualFree(p, 0, MEM_RELEASE);
	}


	/*
	���̴� ���ҽ� ��
	������ ���� �����͸� ���̴����� ��� �޾Ƽ� � �������� �޾Ƽ� ����ұ �������ִ� ��.
	�� �޸� ������ ��� ���ִ��ĸ� ���̴����� �˷��ֱ� ���� ����ϴ� ��.
	*/
	//Create SRV
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY; // ���ҽ��� ����
		desc.Texture2DArray.MipLevels = 1; // ���ǰ� �´� desc�� ��������� ���� �־������.
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
			animation ��� : �ش� �����ӿ� �ش� ���� �󸶸�ŭ �������� ����

			animation : Local(Relative)
			parent : Global (AssimpConverter���� ���� ��.)
			bone[b] = Local(Relative) * Global
			bone[b] : Global

			clipTransforms[index].Transform[f][b] = invGlobal * bones[b];
			= Global * Global

			   50       20        50
			-------> A ---->  B ---->

			70(Global) + Local(50) = 120(Global)

			Global * Global == 70(Global) * 120(Global)�� �ǰ� ������ Global�� ������� ������ invGlobal = XMMatrixInverse(nullptr, invGlobal);
			Global �� ����� = Local(Relative)
			�׷��� Local(Relative) * Global�� ������ ��
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
