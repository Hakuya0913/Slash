#pragma once

/*

アニメーションの再生状態を管理するクラス

*/

#include<cstdint>
#include"AnimationPlayer.h"

class AnimationController
{
public:

	AnimationController() = default;
	~AnimationController() = default;

	void Init(AnimationPlayer* player) { this->player = player; }

	void Update(float deltaTime);

	void Play(int32_t animationIndex, bool isLoop = false, float playbackSpeed = 1.0f);
	void Stop();
	void Reset();
	void ReStart();

	bool IsPlaying() const;
	int32_t GetCurrentAnimationIndex() const;

private:

	//扱うAnimationPlayer
	AnimationPlayer* player = nullptr;

};