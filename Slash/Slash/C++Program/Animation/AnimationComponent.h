#pragma once

/*

Ä¶’†‚Ìƒ‚[ƒVƒ‡ƒ“‚ÆÄ¶ó‘Ô‚ğŠÇ—‚·‚é

*/

#include<cstdint>
#include"AnimationStructure.h"
#include"AnimationResource.h"

class AnimationComponent
{
public:

	AnimationComponent() = default;
	~AnimationComponent() = default;

	void Init(AnimationResource* resource);

	void Update(float deltaTime);

	void Play(int32_t animationIndex);	//“r’†Ä¶—p
	void Play();

	void Stop();
	void Pause();
	void Resume();

	bool IsPlaying() const;
	bool HasAnimation() const;

	int32_t GetCurrentAnimation() const;

	float GetCurrentTime() const;
	float GetPlaybackSpeed() const;

	bool IsLooping() const;

	void SetPlaybackSpeed(float speed);
	void SetLoop(bool isloop);
	void SetCurrentTime(float time);

	AnimationResource* GetResource();
	const AnimationResource* GetResource() const;

private:

	AnimationResource* resource = nullptr;
	AnimationState state;

};