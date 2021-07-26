#pragma once

#include "Animations.h"

typedef std::vector<LPANIMATION> AnimationSet;
typedef AnimationSet* LPANIMATION_SET;

/*
	Manage animation sets database
*/
class AnimationSets
{
	static AnimationSets* __instance;
	std::unordered_map<int, LPANIMATION_SET> animation_sets;

public:
	AnimationSets() noexcept {};
	void Add(int id, LPANIMATION_SET ani);
	LPANIMATION_SET Get(int id);
	void Clear();

	static AnimationSets* GetInstance();
};
