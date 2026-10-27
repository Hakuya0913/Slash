#include"AnimationComponent.h"

void AnimationComponent::Init(AnimationResource* resource)
{

	this->resource = resource;

	player.Init(resource);
	skeleton.SetResource(resource);

	controller.Init(&player);

}

void AnimationComponent::Update(float deltaTime)
{

	if (resource == nullptr)
	{
		return;
	}

	controller.Update(deltaTime);

	skeleton.SetPose(player.GetPose());

}

void AnimationComponent::Release()
{

	controller.Reset();
	skeleton.Release();
	player.Reset();

	resource = nullptr;

}

void AnimationComponent::Play(int32_t animationIndex, bool isLoop, float playbackSpeed)
{

	if (resource == nullptr)
	{
		return;
	}

	controller.Play(animationIndex, isLoop, playbackSpeed);

}

void AnimationComponent::Stop()
{

	controller.Stop();

}

void AnimationComponent::ReStart()
{

	controller.ReStart();

}

void AnimationComponent::Reset()
{

	controller.Reset();

}