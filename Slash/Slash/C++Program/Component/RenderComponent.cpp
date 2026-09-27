#include"RenderComponent.h"

void RenderComponent::SetTransform(TransformComponent& transform)
{

	transformComponent = &transform;

}

void RenderComponent::SetModel(ModelComponent& model)
{

	modelComponent = &model;

}