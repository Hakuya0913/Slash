#include"SkeletonResource.h"

void SkeletonResource::Create(const Skeleton& skeleton)
{

	Release();

	this->skeleton = skeleton;

	for (size_t i = 0; i < this->skeleton.bones.size(); ++i)
	{

		boneIndexMap[this->skeleton.bones[i].name] = static_cast<int32_t>(i);

	}

}

void SkeletonResource::Release()
{

	skeleton.bones.clear();
	boneIndexMap.clear();

}