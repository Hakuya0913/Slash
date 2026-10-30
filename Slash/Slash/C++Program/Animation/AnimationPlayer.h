#pragma once

/*

アニメーションの再生
現在時刻におけるボーンのTransform計算

*/

#include<vector>
#include<cstdint>
#include<DirectXMath.h>
#include<directxtk/SimpleMath.h>
#include"AnimationStructure.h"
#include"AnimationResource.h"

class AnimationPlayer
{
public:

	AnimationPlayer() = default;
	~AnimationPlayer() = default;

	void Init(const AnimationResource* resource, const Skeleton* skeleton);

	void Update(float deltaTime);
	
	void Play(int32_t animationIndex, bool isLooping = false, float playbackSpeed = 1.0f);
	void Stop() { state.isPlaying = false; }
	void Reset(); //終了・初期化
	void ReStart();	//再度再生

	bool IsPlaying() const { return state.isPlaying; }
	int32_t GetCurrentAnimationIndex() const { return state.currentAnimationIndex; }
	const SkeletonPose& GetPose() const { return pose; }

private:

	const AnimationClip* GetCurrentAnimationClip() const;

	//現在時刻のPose計算
	void UpdatePose();

	const AnimationResource* resource = nullptr;
	const Skeleton* skeleton = nullptr;

	AnimationState state;
	SkeletonPose pose;

};