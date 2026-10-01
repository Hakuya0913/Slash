#include"ModelRenderer.h"
#include<d3dcompiler.h>
#include<cstring>

#pragma comment(lib,"d3dcompiler.lib")

using namespace DirectX;
using namespace DirectX::SimpleMath;

bool ModelRenderer::Init(
	ID3D11Device* device,
	ID3D11DeviceContext* context,
	Camera& camera
)
{

	if (device == nullptr || context == nullptr)
	{
		return false;
	}

	this->device = device;
	this->context = context;
	this->camera = &camera;

	HRESULT hr;

	//VSì«Ç›çûÇ›
	ComPtr<ID3DBlob> vsBlob;
	ComPtr<ID3DBlob> errorBlob;

}