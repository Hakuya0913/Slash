#pragma once

/*

アニメーションの再生管理
現在時刻におけるボーンのTransform計算

*/

#include<vector>
#include<cstdint>
#include<DirectXMath.h>
#include<directxtk/SimpleMath.h>
#include"AnimationStructure.h"
#include"AnimationResource.h"
#include"SkeletonComponent.h"

class AnimationPlayer
{
public:

	AnimationPlayer() = default;
	~AnimationPlayer() = default;

	void Init(const AnimationResource* resource, SkeletonComponent* skeletonComponent);

	void Update(float deltaTime);
	
	void Play(int32_t animationIndex, bool isLooping = false, float playbackSpeed = 1.0f);
	void Stop() { state.isPlaying = false; }
	void Reset(); //終了・初期化
	void ReStart();	//再度再生

	bool IsPlaying() const { return state.isPlaying; }
	int32_t GetCurrentAnimationIndex() const { return state.currentAnimationIndex; }

private:

	const AnimationClip* GetCurrentAnimationClip() const;

	//現在時刻のPose計算
	void UpdatePose();

	const AnimationResource* resource = nullptr;

	SkeletonComponent* skeletonComponent;	//操作する対象ボーン

	//再生状態
	AnimationState state;

};