#pragma once

/*

再生中のモーションと再生状態を管理する

*/

#include<cstdint>
#include"AnimationStructure.h"
#include"AnimationPlayer.h"
#include"AnimationController.h"
#include"SkeletonComponent.h"

class AnimationComponent
{
public:

	AnimationComponent() = default;
	~AnimationComponent() = default;

	void Init(AnimationResource* resource);

	void Update(float deltaTime);

	//保持しているアニメーションの解放
	void Release();

	void Play(int32_t animationIndex, bool isLoop = false, float playbackSpeed = 1.0f);

	void Stop();	//アニメーション停止
	void ReStart();	//アニメーション先頭から開始
	void Reset();	//初期状態に変更

	const AnimationResource* GetResource() const { return resource; }
	const AnimationPlayer& GetPlayer() const { return player; }
	const AnimationController& GetController() const { return controller; }
	const SkeletonComponent& GetSkeleton() const { return skeleton; }
	
	bool IsPlaying() const { return controller.IsPlaying(); }
	int32_t GetCurrentAnimationIndex() const { return controller.GetCurrentAnimationIndex(); }

private:

	const AnimationResource* resource = nullptr;

	AnimationPlayer player;
	AnimationController controller;

	SkeletonComponent skeleton;

};