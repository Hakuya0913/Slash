#include"AnimationEvaluation.h"

#include<algorithm>

using namespace DirectX;
using namespace DirectX::SimpleMath;

//このCPPのみで使用する関数
namespace
{

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

	BoneTransform EvaluateSingleBone(const BoneAnimation& animation, float time)
	{

		const Vector3	 position = EvaluatePosition(animation, time);
		const Quaternion rotation = EvaluateRotation(animation, time);
		const Vector3	 scale = EvaluateScale(animation, time);

		BoneTransform result;

		result.transform =
			Matrix::CreateScale(scale) *
			Matrix::CreateFromQuaternion(rotation) *
			Matrix::CreateTranslation(position);

		return result;

	}

}

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

		Matrix localTransform = Matrix::Identity;

		if (animation != nullptr)
		{

			BoneTransform boneTransform;

			boneTransform = EvaluateSingleBone(*animation, evaluateTime);

			localTransform = boneTransform.transform;

		}

		//親ボーンがある場合親のTransformを適用
		if (bone.parentIndex >= 0 &&
			static_cast<size_t>(bone.parentIndex) < pose.boneTransforms.size())
		{

			const Matrix& parentTransform = pose.boneTransforms[bone.parentIndex].transform;

			pose.boneTransforms[boneIndex].transform = parentTransform * localTransform;

		}
		else
		{

			pose.boneTransforms[boneIndex].transform = localTransform;

		}

	}

	return pose;

}


