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

	const AnimationClip* clip = GetCurrentAnimationClip();

	if (clip == nullptr)
	{
		return;
	}

	//再生時間更新
	state.currentTime += deltaTime * clip->ticksPerSecond * state.playbackSpeed;

	//アニメーション終了
	if (state.currentTime >= clip->duration)
	{

		if (state.isLooping == true)
		{

			state.currentTime = std::fmod(state.currentTime, clip->duration);

		}
		else
		{

			state.currentTime = clip->duration;
			state.isPlaying = false;

		}

	}

	UpdateBoneTransforms();

}

void AnimationPlayer::Play(int32_t animationIndex, bool isLooping, float playbackSpeed)
{

	if (resource == nullptr)
	{
		return;
	}

	if (animationIndex < 0 || 
		animationIndex >= static_cast<int32_t>(resource->GetAnimationClipCount())
	)
	{
		return;
	}

	state.currentAnimationIndex = animationIndex;
	state.currentTime = 0.0f;
	state.playbackSpeed = playbackSpeed;
	state.isPlaying = true;
	state.isLooping = isLooping;

}

void AnimationPlayer::Reset()
{

	state.currentTime = 0.0f;

	if (state.currentAnimationIndex >= 0)
	{

		UpdateBoneTransforms();

	}

}

const AnimationClip* AnimationPlayer::GetCurrentAnimationClip() const
{

	if (resource == nullptr)			 return nullptr;
	if (state.currentAnimationIndex < 0) return nullptr;

	if (state.currentAnimationIndex >= static_cast<int32_t>(resource->GetAnimationClipCount()))
	{
		return nullptr;
	}

	return &resource->GetAnimationClip(state.currentAnimationIndex);

}

Vector3 AnimationPlayer::EvaluatePosition(const BoneAnimation& animation, float time) const
{

	if (animation.positionKeys.empty() == true)
	{
		return Vector3::Zero;
	}

	if (animation.positionKeys.size() == 1)
	{

		Vector3 result;

		result.x = animation.positionKeys[0].value.x;
		result.y = animation.positionKeys[0].value.y;
		result.z = animation.positionKeys[0].value.z;

		return result;

	}

	const size_t index = FindPositionKeyIndex(animation, time);

	if (index >= animation.positionKeys.size() - 1)
	{

		Vector3 result;

		const PositionKey& key = animation.positionKeys.back();

		result.x = key.value.x;
		result.y = key.value.y;
		result.z = key.value.z;

		return result;

	}

	const PositionKey& current = animation.positionKeys[index];
	const PositionKey& next = animation.positionKeys[index + 1];
	const float length = next.time - current.time;

	float factor = 0.0f;

	if (length > 0.0f)
	{

		factor = (time - current.time) / length;

	}

	Vector3 currentValue;
	currentValue.x = current.value.x;
	currentValue.y = current.value.y;
	currentValue.z = current.value.z;

	Vector3 nextValue;
	nextValue.x = next.value.x;
	nextValue.y = next.value.y;
	nextValue.z = next.value.z;

	return Vector3::Lerp(
		currentValue,
		nextValue,
		factor
	);

}

Quaternion AnimationPlayer::EvaluateRotation(const BoneAnimation& animation, float time) const
{

	if (animation.rotationKeys.empty() == true)
	{
		return Quaternion::Identity;
	}

	if (animation.rotationKeys.size() == 1)
	{

		Quaternion result;

		result.x = animation.rotationKeys[0].value.x;
		result.y = animation.rotationKeys[0].value.y;
		result.z = animation.rotationKeys[0].value.z;
		result.w = animation.rotationKeys[0].value.w;

		return result;

	}

	const size_t index = FindRotationKeyIndex(animation, time);

	if (index >= animation.rotationKeys.size() - 1)
	{

		Quaternion result;

		const RotationKey& key = animation.rotationKeys.back();

		result.x = key.value.x;
		result.y = key.value.y;
		result.z = key.value.z;
		result.w = key.value.w;

		return result;

	}

	const RotationKey& current = animation.rotationKeys[index];
	const RotationKey& next = animation.rotationKeys[index + 1];
	const float length = next.time - current.time;

	float factor = 0.0f;

	if (length > 0.0f)
	{

		factor = (time - current.time) / length;

	}

	Quaternion currentValue;
	currentValue.x = current.value.x;
	currentValue.y = current.value.y;
	currentValue.z = current.value.z;
	currentValue.w = current.value.w;

	Quaternion nextValue;
	nextValue.x = next.value.x;
	nextValue.y = next.value.y;
	nextValue.z = next.value.z;
	nextValue.w = next.value.w;

	return Quaternion::Slerp(
		currentValue,
		nextValue,
		factor
	);

}

Vector3 AnimationPlayer::EvaluateScale(const BoneAnimation& animation, float time) const
{

	if (animation.scaleKeys.empty() == true)
	{
		return Vector3::Zero;
	}

	if (animation.scaleKeys.size() == 1)
	{

		Vector3 result;

		result.x = animation.scaleKeys[0].value.x;
		result.y = animation.scaleKeys[0].value.y;
		result.z = animation.scaleKeys[0].value.z;

		return result;

	}

	const size_t index = FindScaleKeyIndex(animation, time);

	if (index >= animation.scaleKeys.size() - 1)
	{

		Vector3 result;

		const ScaleKey& key = animation.scaleKeys.back();

		result.x = key.value.x;
		result.y = key.value.y;
		result.z = key.value.z;

		return result;

	}

	const ScaleKey& current = animation.scaleKeys[index];
	const ScaleKey& next = animation.scaleKeys[index + 1];
	const float length = next.time - current.time;

	float factor = 0.0f;

	if (length > 0.0f)
	{

		factor = (time - current.time) / length;

	}

	Vector3 currentValue;
	currentValue.x = current.value.x;
	currentValue.y = current.value.y;
	currentValue.z = current.value.z;

	Vector3 nextValue;
	nextValue.x = next.value.x;
	nextValue.y = next.value.y;
	nextValue.z = next.value.z;

	return Vector3::Lerp(
		currentValue,
		nextValue,
		factor
	);

}