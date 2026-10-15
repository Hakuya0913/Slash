#pragma once

/*

AnimationClip(モーションデータ)を管理する

*/

#include<vector>
#include<string>
#include<cstdint>
#include"AnimationStructure.h"

class AnimationResource
{
public:

	AnimationResource() = default;
	~AnimationResource() = default;

	void Create(const Skeleton& skeleton, const std::vector<AnimationClip>& animationClip);
	void Release();
	
	bool IsValid() { return (skeleton.bones.empty() == false); }

	const Skeleton& GetSkeleton() const { return skeleton; }

	const AnimationClip& GetAnimationClip(const size_t index) const { return animationClips.at(index); }
	const AnimationClip* GetAnimationClip(const std::string name) const;

	size_t GetAnimationClipCount() const { return animationClips.size(); }

private:

	Skeleton skeleton;
	std::vector<AnimationClip> animationClips;

};