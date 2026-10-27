#pragma once

/*

FBXモデルを表示する

*/

#include<d3d11.h>
#include<vector>
#include<cstdint>
#include<DirectXMath.h>

#include"RendererStructure.h"
#include"../Utility/ComPtr.h"
#include"../Component/Camera.h"
#include"../Model/ModelComponent.h"
#include"../Component/TransformComponent.h"
#include"../Model/ModelResource.h"
#include"../Animation/AnimationStructure.h"

class ModelRenderer
{
public:

	ModelRenderer() = default;
	~ModelRenderer() = default;

	bool Init(ID3D11Device* device, ID3D11DeviceContext* context, Camera& camera);

	void Render(
		const ModelComponent& model,
		const ModelResource& resource,
		const TransformComponent& transform,
		const Skeleton& skeleton,
		const SkeletonPose& pose
	);

private:

	//各初期化関数
	bool CreateRasterizerState();
	bool CreateConstantBuffers();

	//Shader読み込み
	bool ReadShaderFile(const wchar_t* filePath, std::vector<uint8_t>& data);

	//更新処理
	void UpdateTransformBuffer(const TransformComponent& transform);
	void UpdateMaterialBuffer(const Material& material);
	void UpdateBoneBuffer(const ModelComponent& model, const Skeleton& skeleton, const SkeletonPose& pose);

	//DX11Object
	ComPtr<ID3D11Device> device;
	ComPtr<ID3D11DeviceContext> context;

	//カメラへの参照
	Camera* camera = nullptr;

	//定数バッファ
	ComPtr<ID3D11Buffer> transformBuffer;
	ComPtr<ID3D11Buffer> materialBuffer;
	ComPtr<ID3D11Buffer> boneBuffer;

	TransformData transformData{};
	MaterialData materialData{};
	BoneData boneData{};

	//Shader
	ComPtr<ID3D11VertexShader> vertexShader;
	ComPtr<ID3D11PixelShader> pixelShader;

	ComPtr<ID3D11InputLayout> inputLayout;
	ComPtr<ID3D11SamplerState> samplerState;

	//RasterizerState
	ComPtr<ID3D11RasterizerState> rasterizerState;

};