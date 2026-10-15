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

	void Init(const AnimationResource* resource);

	void Update(float deltaTime);
	
	void Play(int32_t animationIndex, bool isLooping = false, float playbackSpeed = 1.0f);
	void Stop();
	void Reset();

	bool IsPlaying() const;
	int32_t GetCurrentAnimationIndex() const;
	const std::vector<BoneTransform>& GetBoneTransforms() const;

private:

	const AnimationResource* resource = nullptr;
	AnimationState state;
	std::vector<BoneTransform> boneTransforms;

};