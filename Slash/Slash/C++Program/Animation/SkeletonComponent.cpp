#include"SkeletonComponent.h"

void SkeletonComponent::Release()
{

	resource = nullptr;

	pose.boneTransforms.clear();

}

void SkeletonComponent::SetResource(const AnimationResource* resource)
{

	this->resource = resource;

	pose.boneTransforms.clear();

	if (resource == nullptr)
	{
		return;
	}

	const Skeleton& skeleton = resource->GetSkeleton();

	pose.boneTransforms.resize(skeleton.bones.size());

}
