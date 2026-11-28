#include"SkeletonComponent.h"

void SkeletonComponent::Init(const Skeleton& skeleton)
{

	this->skeleton = skeleton;

	pose.boneTransforms.clear();

	pose.boneTransforms.resize(skeleton.bones.size());

	ResetPose();

}

void SkeletonComponent::ResetPose()
{
	
	//全ボーンのTransformを単位行列に初期化
	for (auto& bone : pose.boneTransforms)
	{
		bone.transform = DirectX::SimpleMath::Matrix::Identity;
	}

}
