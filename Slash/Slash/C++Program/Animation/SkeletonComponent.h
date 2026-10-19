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

	//現在のスケルトン姿勢更新
	void Update();

	//Setter
	void SetResource(const AnimationResource* resource);
	void SetPose(const SkeletonPose& pose);

	//Getter
	const AnimationResource* GetResource() const { return resource; }
	const SkeletonPose& GetPos() const { return pose; }


private:

	//使用するAnimationResource
	const AnimationResource* resource = nullptr;

	SkeletonPose pose;

};