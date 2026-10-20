#pragma once

/*

アニメーションの再生状態を管理するクラス

*/

#include<string>
#include"AnimationPlayer.h"
#include"AnimationResource.h"

class AnimationController
{
public:

	AnimationController() = default;
	~AnimationController() = default;

	void Init(AnimationPlayer* player) { this->player = player; }

	void Update(float deltaTime);

	void Play(int32_t animationIndex, float playbackSpeed = 1.0f, bool isLoop = false);
	void Stop();
	void Reset();
	void ReStart();

	bool IsPlaying() const;
	int32_t GetCurrentAnimationIndex() const;

private:

	//扱うAnimationPlayer
	AnimationPlayer* player = nullptr;

};