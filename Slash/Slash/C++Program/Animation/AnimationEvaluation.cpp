#include"AnimationEvaluation.h"

SkeletonPose AnimationEvaluation::Evaluate(
	const AnimationClip& clip,
	const Skeleton& skeleton,
	float time
)
{

	SkeletonPose pose;

	const size_t boneCount = skeleton.bones.size();

	pose.boneTransforms.resize(boneCount);

}