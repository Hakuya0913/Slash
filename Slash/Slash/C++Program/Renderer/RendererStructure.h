#pragma once

/*

Rendererクラスで使用し、共通化できる構造体をまとめるファイル

*/

#include<DirectXMath.h>
#include<SimpleMath.h>
#include<d3d11.h>
#include<cstdint>

#include"../Utility/ComPtr.h"

struct TransformData
{

	DirectX::XMFLOAT4X4 world{};
	DirectX::XMFLOAT4X4 view{};
	DirectX::XMFLOAT4X4 proj{};

};

struct MeshResource
{

	ComPtr<ID3D11Buffer> vertexBuffer;
	ComPtr<ID3D11Buffer> indexBuffer;

	uint32_t indexCount = 0;
	uint32_t materialIndex = 0;

};

struct TextureResource
{

	ComPtr<ID3D11Texture2D> texture;
	ComPtr<ID3D11ShaderResourceView> shaderResourceView;

	uint32_t embeddedTextureIndex = 0;

};

struct MaterialData
{

	DirectX::XMFLOAT4 baseColor{ 1.0f,1.0f,1.0f,1.0f };

	float metallic = 0.0f;
	float roughness = 1.0f;
	float ambientOcclusion = 1.0f;
	float padding0 = 0.0f;	//16byte制限対策

	DirectX::XMFLOAT3 emissiveColor{};
	float emissiveStrength = 1.0f;

};