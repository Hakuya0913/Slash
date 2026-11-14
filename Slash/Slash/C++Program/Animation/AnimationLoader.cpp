#include"AnimationLoader.h"
#include<assimp/Importer.hpp>
#include<assimp/postprocess.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

//AnimationLoader—p‚ÌŠÖ”ŒQ
namespace
{

	XMFLOAT3 ConvertFLOAT3(const aiVector3D& vector)
	{

		XMFLOAT3 result;

		result.x = vector.x;
		result.y = vector.y;
		result.z = vector.z;

		return result;

	}

	Quaternion ConvertQuaternion(const aiQuaternion& quaternion)
	{

		Quaternion result;

		result.x = quaternion.x;
		result.y = quaternion.y;
		result.z = quaternion.z;
		result.w = quaternion.w;

		return result;

	}

	bool LoadBoneAnimation(const aiNodeAnim* channel, BoneAnimation& result)
	{

		if (channel == nullptr)
		{
			return false;
		}

		result.boneName = channel->mNodeName.C_Str();

		result.positionKeys.clear();
		result.rotationKeys.clear();
		result.scaleKeys.clear();

		result.positionKeys.reserve(channel->mNumPositionKeys);
		result.rotationKeys.reserve(channel->mNumRotationKeys);
		result.scaleKeys.reserve(channel->mNumScalingKeys);

		for (size_t i = 0; i < channel->mNumPositionKeys; ++i)
		{
			const aiVectorKey& key = channel->mPositionKeys[i];

			PositionKey positionKey;
			positionKey.time = key.mTime;
			positionKey.value = ConvertFLOAT3(key.mValue);

			result.positionKeys.emplace_back(positionKey);

		}

		for (size_t i = 0; i < channel->mNumRotationKeys; ++i)
		{

			const aiQuatKey& key = channel->mRotationKeys[i];

			RotationKey rotationKey;
			rotationKey.time = key.mTime;
			rotationKey.value = ConvertQuaternion(key.mValue);

			result.rotationKeys.emplace_back(rotationKey);

		}

		for (size_t i = 0; i < channel->mNumScalingKeys; ++i)
		{

			const aiVectorKey& key = channel->mScalingKeys[i];

			ScaleKey scaleKey;
			scaleKey.time = key.mTime;
			scaleKey.value = ConvertFLOAT3(key.mValue);

			result.scaleKeys.emplace_back(scaleKey);

		}

		return true;

	}
	
	bool LoadAnimationClip(const aiAnimation* animation, AnimationClip& result)
	{

		if (animation == nullptr)
		{
			return false;
		}

		result.name = animation->mName.C_Str();
		result.duration = animation->mDuration;
		
		if (animation->mTicksPerSecond > 0.0)
		{
			
			result.ticksPerSecond = animation->mTicksPerSecond;

		}
		else
		{
			
			result.ticksPerSecond = 1.0f;

		}

		result.boneAnimations.clear();
		result.boneAnimations.reserve(animation->mNumChannels);

		for (size_t i = 0; i < animation->mNumChannels; ++i)
		{

			const aiNodeAnim* channel = animation->mChannels[i];

			if (channel == nullptr)
			{
				continue;
			}

			BoneAnimation boneAnimation;

			if (LoadBoneAnimation(channel, boneAnimation) == false)
			{
				return false;
			}

			result.boneAnimations.emplace_back(boneAnimation);

		}

		return true;

	}

	bool LoadAnimationClips(const aiScene* scene, std::vector<AnimationClip>& animationClips)
	{

		if (scene == nullptr)
		{
			return false;
		}

		animationClips.clear();
		animationClips.reserve(scene->mNumAnimations);

		for (size_t i = 0; i < scene->mNumAnimations; ++i)
		{

			const aiAnimation* animation = scene->mAnimations[i];

			if (animation == nullptr)
			{
				continue;
			}

			AnimationClip result;

			if (LoadAnimationClip(animation, result) == false)
			{
				return false;
			}

			animationClips.emplace_back(result);
			
		}

		return animationClips.empty() == false;

	}

	bool LoadScene(const aiScene* scene, AnimationResource& resource)
	{

		if (scene == nullptr)
		{
			return false;
		}

		std::vector<AnimationClip> animationClips;

		if (LoadAnimationClips(scene, animationClips) == false)
		{
			return false;
		}

		resource.Create(animationClips);

		return resource.IsValid();

	}

}

bool AnimationLoader::Load(const std::string& filePath, AnimationResource& resource)
{
	
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(
		filePath,
		aiProcess_ConvertToLeftHanded
	);

	if (scene == nullptr)
	{
		return false;
	}

	if ((scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) != 0)
	{
		return false;
	}

	return LoadScene(scene, resource);

}