#include "pch.h"
#include "Struct.h"

void TransformMatrix(XMFLOAT4X4& out, const aiMatrix4x4& in)
{
	out._11 = in.a1;
	out._12 = in.a2;
	out._13 = in.a3;
	out._14 = in.a4;

	out._21 = in.b1;
	out._22 = in.b2;
	out._23 = in.b3;
	out._24 = in.b4;

	out._31 = in.c1;
	out._32 = in.c2;
	out._33 = in.c3;
	out._34 = in.c4;

	out._41 = in.d1;
	out._42 = in.d2;
	out._43 = in.d3;
	out._44 = in.d4;
}

void MeshEntry::Initialize(ID3D11Device * _Device, vector<SkinnedMesh>& Vertices, const vector<UINT>& Indices)
{
	NumIndices = Indices.size();


	CreateStaticBuffer(_Device, &Vertices[0], Vertices.size(), sizeof(SkinnedMesh), D3D11_BIND_VERTEX_BUFFER, VertexBuffers.GetAddressOf());
	CreateStaticBuffer(_Device, &Indices[0], Indices.size(), sizeof(UINT), D3D11_BIND_INDEX_BUFFER, IndexBuffers.GetAddressOf());
}

void MeshEntry::RenderBuffer(ID3D11DeviceContext * _DeviceContext)
{
	UINT stride;
	UINT offset;


	stride = sizeof(SkinnedMesh);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	_DeviceContext->IASetVertexBuffers(0, 1, VertexBuffers.GetAddressOf(), &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	_DeviceContext->IASetIndexBuffer(IndexBuffers.Get(), DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	_DeviceContext->DrawIndexed(NumIndices, 0, 0);
}

AnimEvaluator::AnimEvaluator(const aiAnimation * pAnim)
{
	LastTime = 0.0;
	TicksPerSecond = static_cast<float>(pAnim->mTicksPerSecond != 0.0f ? pAnim->mTicksPerSecond : 100.0f);
	Duration = static_cast<float>(pAnim->mDuration);
	Name = pAnim->mName.data;

	Channels.resize(pAnim->mNumChannels);
	for (unsigned int a = 0; a < pAnim->mNumChannels; a++)
	{
		Channels[a].Name = pAnim->mChannels[a]->mNodeName.data;
		for (unsigned int i(0); i < pAnim->mChannels[a]->mNumPositionKeys; i++)
			Channels[a].PositionKeys.push_back(pAnim->mChannels[a]->mPositionKeys[i]);

		for (unsigned int i(0); i < pAnim->mChannels[a]->mNumRotationKeys; i++)
			Channels[a].RotationKeys.push_back(pAnim->mChannels[a]->mRotationKeys[i]);

		for (unsigned int i(0); i < pAnim->mChannels[a]->mNumScalingKeys; i++) 
			Channels[a].ScalingKeys.push_back(pAnim->mChannels[a]->mScalingKeys[i]);
	}
	LastPositions.resize(pAnim->mNumChannels, std::make_tuple(0, 0, 0));
}

void AnimEvaluator::Evaluate(float pTime, map<string, BONE*>& bones)
{
	pTime *= TicksPerSecond;

	float time = 0.0f;
	if (Duration > 0.0)
		time = fmod(pTime, Duration);

	// calculate the transformations for each animation channel
	for (unsigned int a = 0; a < Channels.size(); a++) {
		const AnimationChannel* channel = &Channels[a];
		std::map<std::string, BONE*>::iterator bonenode = bones.find(channel->Name);

		if (bonenode == bones.end()) {
			//OUTPUT_DEBUG_MSG("did not find the bone node "<<channel->Name);
			continue;
		}

		// ******** Position *****
		aiVector3D presentPosition(0, 0, 0);
		if (channel->PositionKeys.size() > 0) {
			// Look for present frame number. Search from last position if time is after the last time, else from beginning
			// Should be much quicker than always looking from start for the average use case.
			unsigned int frame = (time >= LastTime) ? get<0>(LastPositions[a]) : 0;
			while (frame < channel->PositionKeys.size() - 1) {
				if (time < channel->PositionKeys[frame + 1].mTime) {
					break;
				}
				frame++;
			}

			// interpolate between this frame's value and next frame's value
			unsigned int nextFrame = (frame + 1) % channel->PositionKeys.size();

			const aiVectorKey& key = channel->PositionKeys[frame];
			const aiVectorKey& nextKey = channel->PositionKeys[nextFrame];
			double diffTime = nextKey.mTime - key.mTime;
			if (diffTime < 0.0)
				diffTime += Duration;
			if (diffTime > 0) {
				float factor = float((time - key.mTime) / diffTime);
				presentPosition = key.mValue + (nextKey.mValue - key.mValue) * factor;
			}
			else {
				presentPosition = key.mValue;
			}
			get<0>(LastPositions[a]) = frame;
		}
		// ******** Rotation *********
		aiQuaternion presentRotation(1, 0, 0, 0);
		if (channel->RotationKeys.size() > 0)
		{
			unsigned int frame = (time >= LastTime) ? get<1>(LastPositions[a]) : 0;
			while (frame < channel->RotationKeys.size() - 1) {
				if (time < channel->RotationKeys[frame + 1].mTime)
					break;
				frame++;
			}

			// interpolate between this frame's value and next frame's value
			unsigned int nextFrame = (frame + 1) % channel->RotationKeys.size();

			const aiQuatKey& key = channel->RotationKeys[frame];
			const aiQuatKey& nextKey = channel->RotationKeys[nextFrame];
			double diffTime = nextKey.mTime - key.mTime;
			if (diffTime < 0.0) diffTime += Duration;
			if (diffTime > 0) {
				float factor = float((time - key.mTime) / diffTime);
				aiQuaternion::Interpolate(presentRotation, key.mValue, nextKey.mValue, factor);
			}
			else presentRotation = key.mValue;
			get<1>(LastPositions[a]) = frame;
		}

		// ******** Scaling **********
		aiVector3D presentScaling(1, 1, 1);
		if (channel->ScalingKeys.size() > 0) {
			unsigned int frame = (time >= LastTime) ? get<2>(LastPositions[a]) : 0;
			while (frame < channel->ScalingKeys.size() - 1) {
				if (time < channel->ScalingKeys[frame + 1].mTime)
					break;
				frame++;
			}

			presentScaling = channel->ScalingKeys[frame].mValue;
			get<2>(LastPositions[a]) = frame;
		}

		aiMatrix4x4 mat = aiMatrix4x4(presentRotation.GetMatrix());

		mat.a1 *= presentScaling.x; mat.b1 *= presentScaling.x; mat.c1 *= presentScaling.x;
		mat.a2 *= presentScaling.y; mat.b2 *= presentScaling.y; mat.c2 *= presentScaling.y;
		mat.a3 *= presentScaling.z; mat.b3 *= presentScaling.z; mat.c3 *= presentScaling.z;
		mat.a4 = presentPosition.x; mat.b4 = presentPosition.y; mat.c4 = presentPosition.z;
		mat.Transpose();

		TransformMatrix(bonenode->second->LocalTransform, mat);
	}
	LastTime = time;
}

vector<XMFLOAT4X4>& AnimEvaluator::GetTransforms(float dt)
{
	return Transforms[GetFrameIndexAt(dt)];
}

unsigned int AnimEvaluator::GetFrameIndexAt(float _time)
{
	_time *= TicksPerSecond;

	float time = 0.0f;
	if (Duration > 0.0)
		time = fmod(_time, Duration);

	float percent = time / Duration;
	if (!PlayAnimationForward) percent = (percent - 1.0f)*-1.0f;// this will invert the percent so the animation plays backwards
	return static_cast<unsigned int>((static_cast<float>(Transforms.size()) * percent));
}

#pragma warning(disable : 4996)

//////////////////////////////////////////////////////////////////////////
///@brief 문자열 자르기
///@param orgin : 원본 문자열
///@param tok : 자를 기준이 되는 문자열
///@return 완료된 문자열 배열
//////////////////////////////////////////////////////////////////////////
void SplitString(vector<string>* result, string origin, string tok)
{
	result->clear();

	int cutAt = 0; //자를 위치s
	while ((cutAt = (int)origin.find_first_of(tok)) != origin.npos)
	{
		if (cutAt > 0) //자르는 위치가 0보다크면
			result->push_back(origin.substr(0, cutAt));

		origin = origin.substr(cutAt + 1);
	}

	if (origin.length() > 0) //자르고도 남은 것이 있다면
		result->push_back(origin.substr(0, cutAt));
}

void SplitString(vector<wstring>* result, wstring origin, wstring tok)
{
	result->clear();

	int cutAt = 0; //자를 위치s
	while ((cutAt = (int)origin.find_first_of(tok)) != origin.npos)
	{
		if (cutAt > 0) //자르는 위치가 0보다크면
			result->push_back(origin.substr(0, cutAt));

		origin = origin.substr(cutAt + 1);
	}

	if (origin.length() > 0) //자르고도 남은 것이 있다면
		result->push_back(origin.substr(0, cutAt));
}


//////////////////////////////////////////////////////////////////////////
///@brief 시작 문자열이 같은지 체크
///@param str : 체크하려는 문자열
///@param comp : 시작 비교문자열
//////////////////////////////////////////////////////////////////////////
bool StartsWith(string str, string comp)
{
	wstring::size_type index = str.find(comp);
	if (index != wstring::npos && (int)index == 0)
		return true;

	return false;
}

bool StartsWith(wstring str, wstring comp)
{
	wstring::size_type index = str.find(comp);
	if (index != wstring::npos && (int)index == 0)
		return true;

	return false;
}

//////////////////////////////////////////////////////////////////////////
///@brief 해당 문자열이 포함되어 있는지
///@param str : 체크하려는 문자열
///@param comp : 비교문자열
//////////////////////////////////////////////////////////////////////////
bool Contain(string str, string comp)
{
	size_t found = str.find(comp);

	return found != wstring::npos;
}

bool Contain(wstring str, wstring comp)
{
	size_t found = str.find(comp);

	return found != wstring::npos;
}


//////////////////////////////////////////////////////////////////////////
///@brief 해당 문자열에서 comp 문자를 rep로 변경
///@param str : 체크하려는 문자열
///@param comp : 비교문자열
///@param rep : 바꿀문자열
//////////////////////////////////////////////////////////////////////////
void Replace(string * str, string comp, string rep)
{
	string temp = *str;

	size_t start_pos = 0;
	while ((start_pos = temp.find(comp, start_pos)) != wstring::npos)
	{
		temp.replace(start_pos, comp.length(), rep);
		start_pos += rep.length();
	}

	*str = temp;
}

void Replace(wstring* str, wstring comp, wstring rep)
{
	wstring temp = *str;

	size_t start_pos = 0;
	while ((start_pos = temp.find(comp, start_pos)) != wstring::npos)
	{
		temp.replace(start_pos, comp.length(), rep);
		start_pos += rep.length();
	}

	*str = temp;
}

//////////////////////////////////////////////////////////////////////////
///@brief string형을 wstring형으로 변경
///@param value : 변환할 문자열
///@return 변환 완료 문자열
//////////////////////////////////////////////////////////////////////////
wstring ToWString(string value)
{
	wstring temp = L"";
	temp.assign(value.begin(), value.end());

	return temp;
}

//////////////////////////////////////////////////////////////////////////
///@brief wstring형을 string형으로 변경
///@param value : 변환할 문자열
///@return 변환 완료 문자열
//////////////////////////////////////////////////////////////////////////
string ToString(wstring value)
{
	string temp = "";
	temp.assign(value.begin(), value.end());

	return temp;
}
string Format(const string format, ...)
{
	va_list args;

	va_start(args, format);
	size_t len = vsnprintf(NULL, 0, format.c_str(), args);
	va_end(args);

	vector<char> vec(len + 1);

	va_start(args, format);
	vsnprintf(&vec[0], len + 1, format.c_str(), args);
	va_end(args);

	return &vec[0];
}

wstring Format(const wstring format, ...)
{
	va_list args;

	va_start(args, format);
	size_t len = _vsnwprintf(NULL, 0, format.c_str(), args);
	va_end(args);

	vector<WCHAR> vec(len + 1);

	va_start(args, format);
	_vsnwprintf(&vec[0], len + 1, format.c_str(), args);
	va_end(args);

	return &vec[0];
}