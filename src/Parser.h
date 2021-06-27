#pragma once

#include "Mario.h"
#include "GameObject.h"

#define SCENE_SECTION_UNKNOWN -1
#define SCENE_SECTION_TEXTURES 2
#define SCENE_SECTION_SPRITES 3
#define SCENE_SECTION_ANIMATIONS 4
#define SCENE_SECTION_ANIMATION_SETS	5
#define SCENE_SECTION_TILEMAP 7
#define SCENE_SECTION_OBJECTS	6

#define OBJECT_TYPE_MARIO	0
#define OBJECT_TYPE_BRICK	1
#define OBJECT_TYPE_GOOMBA	2
#define OBJECT_TYPE_KOOPAS	3

#define OBJECT_TYPE_PORTAL	50

#define MAX_SCENE_LINE 1024

namespace Parser {
	void _Sprites(std::string path);
	void _Textures(std::string line);
	void _Animations(std::string line);
	void _AnimationSets(std::string line);
	std::vector<LPGAMEOBJECT> _Objects(std::string line, Mario* player);
}