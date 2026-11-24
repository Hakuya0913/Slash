#pragma once

/*

スケルトンの本体、
スケルトンの状態・実行状態などを管理する

*/

#include<vector>
#include<cstdint>
#include"AnimationStructure.h"
#include"AnimationResource.h"
#include"SkeletonResource.h"

class SkeletonComponent
{
public:

	SkeletonComponent() = default;
	~SkeletonComponent() = default;

	void Init(const Skeleton& skeleton);

	//姿勢の初期化
	void ResetPose();

	//Getter
	Skeleton& GetSkeleton() { return skeleton; }
	SkeletonPose& GetPose() { return pose; }
	BoneTransform& GetBoneTransform(size_t boneIndex) { return pose.boneTransforms.at(boneIndex); }
	size_t GetBoneCount() const { return pose.boneTransforms.size(); }
	bool HasPose() const { return pose.boneTransforms.empty() == false; }	//Poseが設定されているか

private:

	Skeleton skeleton;	//ボーン本体
	SkeletonPose pose;	//ボーンの現在の姿勢

};