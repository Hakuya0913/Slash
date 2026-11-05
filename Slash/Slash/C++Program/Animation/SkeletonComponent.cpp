#include"SkeletonComponent.h"

void SkeletonComponent::Release()
{

	resource = nullptr;

	pose.boneTransforms.clear();

}

void SkeletonComponent::SetResource(const AnimationResource* resource, const Skeleton& skeleton)
{

	this->resource = resource;

	pose.boneTransforms.clear();

	if (resource == nullptr)
	{
		return;
	}

	pose.boneTransforms.resize(skeleton.bones.size());

}
