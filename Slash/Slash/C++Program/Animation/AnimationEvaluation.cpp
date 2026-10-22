#include"AnimationEvaluation.h"

#include<algorithm>

using namespace DirectX;
using namespace DirectX::SimpleMath;

#pragma region このCPP内でのみ使用する関数プロトタイプ宣言

//キーフレームの補完率計算
float CalculateInterpolationFactor(float currentTime, float startTime, float endTime);

//Transformの評価
Vector3 EvaluatePosition(const BoneAnimation& animation, float time);
Quaternion EvaluateRotation(const BoneAnimation& animation, float time);
Vector3 EvaluateScale(const BoneAnimation& animation, float time);

#pragma endregion


SkeletonPose AnimationEvaluation::Evaluate(
	const AnimationClip& clip,
	const Skeleton& skeleton,
	float time
)
{

	SkeletonPose pose;

	pose.boneTransforms.resize(skeleton.bones.size());

	//clipの時間内に収める
	float evaluateTime = time;

	if (clip.duration > 0.0f)
	{

		evaluateTime = std::clamp(
			evaluateTime,
			0.0f, clip.duration
		);

	}

	//Skeletonにあるボーンを順に評価
	for (size_t boneIndex = 0; boneIndex < skeleton.bones.size(); ++boneIndex)
	{

		const BoneInfo& bone = skeleton.bones[boneIndex];

		const BoneAnimation* animation = nullptr;

		//Bone名に対応するAnimation検索
		for (const BoneAnimation& boneAnimation : clip.boneAnimations)
		{

			if (boneAnimation.boneName == bone.name)
			{

				animation = &boneAnimation;

				break;

			}

		}

		if (animation == nullptr)
		{

			pose.boneTransforms[boneIndex].transform = Matrix::Identity;

			continue;

		}

		pose.boneTransforms[boneIndex] =

	}

}

#pragma region このCPP内でのみ使用する関数定義

//補完率計算
float CalculateInterpolationFactor(float currentTime, float startTime, float endTime)
{

	float result;

	const float duration = endTime - startTime;

	if (duration <= 0.0f)
	{
		return 0.0f;
	}

	result = std::clamp(
		(currentTime - startTime) / duration,
		0.0f, 1.0f
	);

	return result;

}

//Position評価
Vector3 EvaluatePosition(const BoneAnimation& animation, float time)
{

	const auto& keys = animation.positionKeys;

	if (keys.empty() == true)
	{
		return Vector3::Zero;
	}

	if (keys.size() == 1)
	{
		return keys[0].value;
	}

	if (time <= keys.front().time)
	{
		return keys.front().value;
	}

	if (time >= keys.back().time)
	{
		return keys.back().value;
	}

	for (size_t i = 0; i < keys.size() - 1; ++i)
	{

		const auto& current = keys[i];
		const auto& next = keys[i + 1];

		if (time >= current.time &&
			time <= next.time)
		{

			const float factor = CalculateInterpolationFactor(
				time, current.time, next.time
			);

			Vector3 result;

			result = Vector3::Lerp(
				current.value, next.value,
				factor
			);

			return result;

		}

	}

	return keys.back().value;

}

//Rotation評価
Quaternion EvaluateRotation(const BoneAnimation& animation, float time)
{

	const auto& keys = animation.rotationKeys;

	if (keys.empty() == true)
	{
		return Quaternion::Identity;
	}

	if (keys.size() == 1)
	{
		return keys[0].value;
	}

	if (time <= keys.front().time)
	{
		return keys.front().value;
	}

	if (time >= keys.back().time)
	{
		return keys.back().value;
	}

	for (size_t i = 0; i < keys.size() - 1; ++i)
	{

		const auto& current = keys[i];
		const auto& next = keys[i + 1];

		if (time >= current.time &&
			time <= next.time)
		{

			const float factor = CalculateInterpolationFactor(
				time, current.time, next.time
			);

			Quaternion result;

			Quaternion::Slerp(
				current.value, next.value,
				factor,
				result
			);

			return result;

		}

	}

	return keys.back().value;

}

Vector3 EvaluateScale(const BoneAnimation& animation, float time)
{

	const auto& keys = animation.scaleKeys;

	if (keys.empty() == true)
	{
		return Vector3::One;
	}

	if (keys.size() == 1)
	{
		return keys[0].value;
	}

	if (time <= keys.front().time)
	{
		return keys.front().value;
	}

	if (time >= keys.back().time)
	{
		return keys.back().value;
	}

	for (size_t i = 0; i < keys.size() - 1; ++i)
	{

		const auto& current = keys[i];
		const auto& next = keys[i + 1];

		if (time >= current.time &&
			time <= next.time)
		{

			const float factor = CalculateInterpolationFactor(
				time, current.time, next.time
			);

			Vector3 result;

			result = Vector3::Lerp(
				current.value, next.value,
				factor
			);

			return result;

		}

	}

	return keys.back().value;

}

#pragma endregion
