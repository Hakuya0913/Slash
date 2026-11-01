#pragma once

/*

FBXファイルからモーションデータを読み込み
AnimationResourceを作成する

*/

#include<string>
#include<vector>
#include<assimp/scene.h>
#include<DirectXMath.h>
#include<directxtk/SimpleMath.h>
#include"AnimationStructure.h"
#include"AnimationResource.h"

namespace AnimationLoader
{

	//Animaton読み込み
	bool Load(const std::string& filePath, AnimationResource& resource);

}