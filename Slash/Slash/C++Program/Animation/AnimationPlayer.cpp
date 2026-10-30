#include"AnimationPlayer.h"
#include<cmath>
#include"AnimationEvaluation.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

void AnimationPlayer::Init(const AnimationResource* resource, const Skeleton* skeleton)
{

	this->resource = resource;
	this->skeleton = skeleton;

	pose.boneTransforms.clear();

	if (resource == nullptr)
	{
		return;
	}

	pose.boneTransforms.resize(skeleton->bones.size());

	for (BoneTransform& transform : pose.boneTransforms)
	{

		transform.transform = Matrix::Identity;

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
	if (clip->duration > 0.0f &&
		state.currentTime >= clip->duration)
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

	UpdatePose();

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

	//モーション開始直後の姿勢を即座に反映
	UpdatePose();

}

void AnimationPlayer::Reset()
{

	state = AnimationState{};

	pose.boneTransforms.clear();

	if (resource == nullptr)
	{
		return;
	}

	const Skeleton& skeleton = resource->GetSkeleton();

	pose.boneTransforms.resize(skeleton.bones.size());


	for (BoneTransform& transform : pose.boneTransforms)
	{

		transform.transform = Matrix::Identity;

	}

}

void AnimationPlayer::ReStart()
{

	if (resource == nullptr)
	{
		return;
	}

	if (state.currentAnimationIndex < 0)
	{
		return;
	}

	state.currentTime = 0.0f;
	state.isPlaying = true;

	UpdatePose();

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

void AnimationPlayer::UpdatePose()
{

	if (resource == nullptr)
	{
		return;
	}

	const AnimationClip* clip = GetCurrentAnimationClip();

	if (clip == nullptr)
	{
		return;
	}

	const Skeleton& skeleton = resource->GetSkeleton();

	pose = AnimationEvaluation::Evaluate(*clip, skeleton, state.currentTime);

}
