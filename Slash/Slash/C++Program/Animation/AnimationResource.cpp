#include"AnimationResource.h"

void AnimationResource::Create(const Skeleton& skeleton, const std::vector<AnimationClip>& animationClip)
{

	this->skeleton = skeleton;
	this->animationClips = animationClip;

}

void AnimationResource::Release()
{

	skeleton.bones.clear();
	animationClips.clear();

}

const AnimationClip* AnimationResource::GetAnimationClip(const std::string name) const
{

	for (const AnimationClip& clip : animationClips)
	{

		if (clip.name == name)
		{
			return &clip;
		}

	}

	return nullptr;

}
