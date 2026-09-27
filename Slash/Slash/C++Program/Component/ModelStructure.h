#pragma once

#include<DirectXMath.h>
#include<string>
#include<vector>
#include<cstdint>
#include<array>

struct Vertex
{
	DirectX::XMFLOAT3 position{};	//頂点
	DirectX::XMFLOAT3 normal{};		//法線
	DirectX::XMFLOAT2 texCoord{};	//テクスチャ座標
	DirectX::XMFLOAT3 tangent{};	//接線(normalMap用)
};

struct BoneWeight
{

	uint32_t boneIndex = 0;
	float weight = 0.0f;	//ボーンの影響度

};

struct Bone
{

	std::string name;

	//Node配列上でのBoneの位置
	uint32_t nodeIndex = 0;

	//Mesh空間 -> Bone空間
	DirectX::XMFLOAT4X4 offsetMatrix =
	{
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

};

struct Mesh
{

	static constexpr uint32_t MaxBoneInfluences = 4;

	std::string name;

	std::vector<Vertex>		vertices;
	std::vector<uint32_t>	indices;

	//各頂点が影響を受けるBone
	std::vector<std::array<BoneWeight, MaxBoneInfluences>> boneWeights;

	std::vector<uint32_t> boneIndices;	//このMeshで使用するBoneインデックス
	uint32_t materialIndex = 0;			//このMeshで使用するマテリアル

};

struct Node
{

	std::string name;

	int32_t parentIndex = -1;

	DirectX::XMFLOAT4X4 localTransform =
	{
		1.0f, 0.0f,0.0f,0.0f,
		0.0f, 1.0f,0.0f,0.0f,
		0.0f, 0.0f,1.0f,0.0f,
		0.0f, 0.0f,0.0f,1.0f
	};

	std::vector<uint32_t> children;

};

//Textureの種類
enum class TextureType
{

	BaseColor,
	Normal,
	Metallic,
	Roughness,
	AmbientOcculusion,
	Emissive

};

/*

FBXモデルに埋め込まれたテクスチャデータ構造体

Assimpでは圧縮画像データ・ピクセルデータ
で格納される

*/

struct EmbeddedTexture
{

	std::string name;

	//テクスチャデータ
	std::vector<uint8_t> data;

	uint32_t width = 0;
	uint32_t height = 0;

	//圧縮画像ならファイル拡張子がつく
	//ピクセルデータは空文字列
	std::string formatHint;

	//画像・ピクセルの識別変数
	bool isCompressed = false;

};

//マテリアルからテクスチャを参照する
struct TextureReference
{

	//外部テクスチャのファイルパス
	std::string path;

	//埋め込みテクスチャのインデックス
	//0以上なら埋め込みテクスチャ使用
	int32_t embeddedTextureIndex = -1;

	bool IsEmbedded() const { return embeddedTextureIndex >= 0; };
	bool IsExternal() const { return !path.empty(); }
	bool IsValid()	  const { return IsEmbedded() || IsExternal(); }

};

struct Material
{

	std::string name;

	//PBR用のパラメータ
	DirectX::XMFLOAT4 baseColor{ 1.0f,1.0f,1.0f,1.0f };
	float metallic	= 0.0f;
	float roughness = 1.0f;
	float ambientOcclusion = 1.0f;
	DirectX::XMFLOAT3 emissiveColor{ 0.0f,0.0f,0.0f };
	float emissiveStrength = 1.0f;

	//Textureの参照
	TextureReference baseColorTexture;
	TextureReference metallicTexture;
	TextureReference roughnessTexture;
	TextureReference normalTexture;
	TextureReference ambientOcclusionTexture;
	TextureReference emissiveTexture;

};