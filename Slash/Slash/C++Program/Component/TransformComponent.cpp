#include"TransformComponent.h"

using namespace DirectX;

XMMATRIX TransformComponent::GetWorldMatrix() const
{

	XMMATRIX scaleM, rotationM, translationM;

	scaleM = XMMatrixScaling(
		scale.x,
		scale.y,
		scale.z
	);

	rotationM = XMMatrixRotationRollPitchYaw(
		rotation.x,
		rotation.y,
		rotation.z
	);

	translationM = XMMatrixTranslation(
		position.x,
		position.y,
		position.z
	);

	return scaleM * rotationM * translationM;

}