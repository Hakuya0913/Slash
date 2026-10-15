#pragma once

/*

Animation系のクラスが使用する
構造体をまとめて配置する

*/

#include<DirectXMath.h>
#include<directxtk/SimpleMath.h>
#include<string>
#include<vector>
#include<cstdint>

struct BoneInfo
{

	std::string name;

	//Model空間から見たボーンの逆バインド行列
	DirectX::XMFLOAT4X4 offsetMatrix;
	
	//親ボーンインデックス
	int32_t parentIndex = -1;

};

struct Skeleton
{

	std::vector<BoneInfo> bones;

};

struct PositionKey
{

	float time = 0.0f;
	DirectX::XMFLOAT3 value = DirectX::SimpleMath::Vector3::Zero;

};

struct RotationKey
{

	float time = 0.0f;
	DirectX::XMFLOAT4 value{ 0.0f,0.0f,0.0f,1.0f };

};

struct ScaleKey
{

	float time = 0.0f;
	DirectX::XMFLOAT3 value = DirectX::SimpleMath::Vector3::One;

};

//1ボーン分のアニメーション
struct BoneAnimation
{

	std::string boneName;

	std::vector<PositionKey> positionKeys;
	std::vector<RotationKey> rotationKeys;
	std::vector<ScaleKey> scaleKeys;

};

struct AnimationClip
{

	std::string name;

	//アニメーション全長
	float duration = 0.0f;

	//秒間ティック数
	float ticksPerSecond = 1.0f;

	std::vector<BoneAnimation> boneAnimations;

};

//アニメーションの再生状態
struct AnimationState
{

	int32_t currentAnimationIndex = -1;	//アニメーションハンドル

	float currentTime = 0.0f;	//再生箇所
	float playbackSpeed = 1.0f;	//再生速度

	bool isPlaying = false;	//再生中か
	bool isLooping = false;	//ループするか

};

struct AnimationBlendState
{

	int32_t previousAnimationIndex = -1;
	int32_t currentAnimationIndex = -1;

	float blendTime = 0.0f;
	float blendDuration = 0.0f;

	bool isBlending = false;

};

struct BoneTransform
{

	DirectX::XMFLOAT4X4 transfrom;

};
