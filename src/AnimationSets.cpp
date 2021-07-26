#include "AnimationSets.h"
#include "Debug.h"

AnimationSets* AnimationSets::__instance = NULL;

AnimationSets* AnimationSets::GetInstance()
{
	if (__instance == NULL) __instance = new AnimationSets();
	return __instance;
}

LPANIMATION_SET AnimationSets::Get(int id)
{
	LPANIMATION_SET ani_set = animation_sets[id];
	if (ani_set == NULL) DebugOut(L"[ERROR] Failed to find animation set id: %d\n", id);
	return ani_set;
}

void AnimationSets::Add(int id, LPANIMATION_SET ani_set)
{
	animation_sets[id] = ani_set;
}

void AnimationSets::Clear()
{
	for (auto a : animation_sets)
	{
		LPANIMATION_SET animset = a.second;
		delete animset;
	}
	animation_sets.clear();
}