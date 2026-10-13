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