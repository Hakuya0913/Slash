#include"AnimationPlayer.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

void AnimationPlayer::Init(const AnimationResource* resource)
{

	this->resource = resource;

	boneTransforms.clear();

	if (resource == nullptr)
	{
		return;
	}

	const Skeleton& skeleton = resource->GetSkeleton();

	boneTransforms.resize(skeleton.bones.size());

	for (BoneTransform& transform : boneTransforms)
	{

		XMStoreFloat4x4(
			&transform.transfrom,
			XMMatrixIdentity()
		);

	}

}

void AnimationPlayer::Update(float deltaTime)
{

	if (resource == nullptr) return;
	if (state.isPlaying == false) return;

	const AnimationClip* clip = 

}