#include"AnimationLoader.h"
#include<assimp/Importer.hpp>
#include<assimp/postprocess.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

//AnimationLoader用の関数群
namespace
{

	bool LoadScene(const aiScene* scene, AnimationResource& resource);

	bool LoadAnimationClips(const aiScene* scene, std::vector<AnimationClip>& animationClips);

	bool LoadAnimationClip(const aiAnimation* animation, AnimationClip& result);

	bool LoadBoneAnimation(const aiNodeAnim* nodeAnim, BoneAnimation& result);

	XMFLOAT3 ConvertFLOAT3(const aiVector3D& vector);

	Quaternion ConvertQuaternion(const aiQuaternion& quaternion); 

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