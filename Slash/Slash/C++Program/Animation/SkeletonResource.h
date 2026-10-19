#pragma once

/*

Skeleton固定のリソースを保持する

*/

#include<cstdint>
#include<string>
#include<unordered_map>
#include"AnimationStructure.h"

class SkeletonResource
{
public:

	SkeletonResource() = default;
	~SkeletonResource() = default;

	//作成・破棄
	void Create(const Skeleton& skeleton);
	void Release();

	//Getter
	const Skeleton& GetSkeleton() const { return skeleton; }
	const BoneInfo& GetBone(size_t index) const { return skeleton.bones.at(index); }
	size_t GetBoneCount() const { return skeleton.bones.size(); }
	int32_t GetBoneIndex(const std::string& name) const;
	bool IsValid() { return !skeleton.bones.empty(); }

private:

	//Skeleton本体
	Skeleton skeleton;

	std::unordered_map<std::string, int32_t> boneIndexMap;

};