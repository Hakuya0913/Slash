#include"AnimationController.h"

void AnimationController::Update(float deltaTime)
{

	if (player == nullptr)
	{
		return;
	}
	
	player->Update(deltaTime);

}

void AnimationController::Play(int32_t animationIndex, bool isLoop, float playbackSpeed)
{

	if (player == nullptr)
	{
		return;
	}

	player->Play(animationIndex, isLoop, playbackSpeed);

}

void AnimationController::Stop()
{

	if (player == nullptr)
	{
		return;
	}

	player->Stop();

}

void AnimationController::Reset()
{

	if (player == nullptr)
	{
		return;
	}

	player->Reset();

}

void AnimationController::ReStart()
{

	if (player == nullptr)
	{
		return;
	}

	player->ReStart();

}

bool AnimationController::IsPlaying() const
{

	if (player == nullptr)
	{
		return false;
	}

	return player->IsPlaying();

}

int32_t AnimationController::GetCurrentAnimationIndex() const
{

	if (player == nullptr)
	{
		return -1;
	}

	return player->GetCurrentAnimationIndex();

}