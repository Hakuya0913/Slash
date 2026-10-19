#pragma once

/*

スケルトンの状態・実行状態などを管理する

ボーンそのものは保持しない

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

	//要素の解放
	void Release();

	//現在のスケルトン姿勢更新
	void Update();

	//Setter
	void SetResource(const AnimationResource* resource);			//使用するAnimationResource設定
	void SetPose(const SkeletonPose& pose) { this->pose = pose; }	//Skeletonの現在の姿勢を設定

	//Getter
	const AnimationResource* GetResource() const { return resource; }
	const SkeletonPose& GetPos() const { return pose; }
	const BoneTransform& GetBoneTransform(size_t boneIndex) const { return pose.boneTransforms.at(boneIndex); }
	size_t GetBoneCount() const { return pose.boneTransforms.size(); }
	bool IsValid() const { return resource != nullptr; }					//Skeletonが設定されているか
	bool HasPose() const { return pose.boneTransforms.empty() == false; }	//Poseが設定されているか

private:

	//使用するAnimationResource
	const AnimationResource* resource = nullptr;

	SkeletonPose pose;

};