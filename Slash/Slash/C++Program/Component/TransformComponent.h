#pragma once

/*

Position,Rotation,Scaleを持つコンポーネント

*/

#include<DirectXMath.h>
#include<SimpleMath.h>

class TransformComponent
{
public:

	TransformComponent() = default;
	~TransformComponent() = default;

	//Setter
	void SetPosition(const DirectX::SimpleMath::Vector3& pos) { position = pos; }
	void SetRotation(const DirectX::SimpleMath::Vector3& rot) { rotation = rot; }
	void SetScale(	 const DirectX::SimpleMath::Vector3& scl) { scale	 = scl; }

	//Getter
	const DirectX::SimpleMath::Vector3& GetPosition()	const { return position; }
	const DirectX::SimpleMath::Vector3& GetRotation()	const { return rotation; }
	const DirectX::SimpleMath::Vector3& GetScale()		const { return scale; }

	//World行列を作成し、返す
	DirectX::XMMATRIX GetWorldMatrix() const;

private:

	DirectX::SimpleMath::Vector3 position	= DirectX::SimpleMath::Vector3::Zero;
	DirectX::SimpleMath::Vector3 rotation	= DirectX::SimpleMath::Vector3::Zero;
	DirectX::SimpleMath::Vector3 scale		= DirectX::SimpleMath::Vector3::One;

};
