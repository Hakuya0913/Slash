#pragma once

/*

アニメーションの再生
現在時刻におけるボーンのTransform計算

*/

#include<vector>
#include<cstdint>
#include<DirectXMath.h>
#include<directxtk/SimpleMath.h>
#include"AnimationStructure.h"
#include"AnimationResource.h"

class AnimationPlayer
{
public:

	AnimationPlayer() = default;
	~AnimationPlayer() = default;

	void Init(const AnimationResource* resource);

	void Update(float deltaTime);
	
	void Play(int32_t animationIndex, bool isLooping = false, float playbackSpeed = 1.0f);
	void Stop() { state.isPlaying = false; }
	void Reset(); //終了・初期化
	void ReStart();	//再度再生

	bool IsPlaying() const { return state.isPlaying; }
	int32_t GetCurrentAnimationIndex() const { return state.currentAnimationIndex; }
	const std::vector<BoneTransform>& GetBoneTransforms() const { return boneTransforms; }

private:

	const AnimationClip* GetCurrentAnimationClip() const;

	//ボーンアニメーションからTransform取得
	DirectX::SimpleMath::Vector3 EvaluatePosition(const BoneAnimation& animation, float time) const;
	DirectX::SimpleMath::Quaternion EvaluateRotation(const BoneAnimation& animation, float time) const;
	DirectX::SimpleMath::Vector3 EvaluateScale(const BoneAnimation& animation, float time) const;

	//各種キーフレームの検索
	size_t FindPositionKeyIndex(const BoneAnimation& animation, float time) const;
	size_t FindRotationKeyIndex(const BoneAnimation& animation, float time) const;
	size_t FindScaleKeyIndex(	const BoneAnimation& animation, float time) const;

	//ローカルTransformからModel空間Transformを計算
	void UpdateBoneTransforms();

	//ボーン階層を考慮したTransform計算
	void CalculateBoneTransform(
		int32_t boneIndex, 
		const DirectX::SimpleMath::Matrix& parentTransform
	);

	const AnimationResource* resource = nullptr;
	AnimationState state;
	std::vector<BoneTransform> boneTransforms;

};