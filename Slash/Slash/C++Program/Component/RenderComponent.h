#pragma once

/*

モデルの描画を行うコンポーネント

*/

#include"TransformComponent.h"
#include"ModelComponent.h"

class RenderComponent
{
public:

	RenderComponent() = default;
	~RenderComponent() = default;

	//Setter
	void SetTransform(TransformComponent& transform);
	void SetModel(ModelComponent& model);

	//Getter
	TransformComponent& GetTransform() const	{ return *transformComponent; }
	ModelComponent& GetModel() const { return *modelComponent; }

private:

	//描画に使用するコンポーネントへの参照
	TransformComponent* transformComponent;
	ModelComponent* modelComponent;

};
