#pragma once

/*

AnimationClip‚ğw’è‚³‚ê‚½ŠÔ‚É‰‚¶‚Ä•]‰¿
SkeletonPose‚ğ¶¬‚·‚é

*/

#include<DirectXMath.h>
#include<directxtk/SimpleMath.h>
#include"AnimationStructure.h"

namespace AnimationEvaluation
{

	SkeletonPose Evaluate(
		const AnimationClip& clip,
		const Skeleton& skeleton,
		float time
	);

}