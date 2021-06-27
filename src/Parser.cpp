#include <fstream>
#include <vector>

#include "Parser.h"
#include "Textures.h"
#include "Debug.h"
#include "Sprites.h"
#include "Strings.h"
#include "Animations.h"
#include "AnimationSets.h"
#include "GameObject.h"
#include "Mario.h"
#include "Goomba.h"
#include "Brick.h"
#include "Koopa.h"
#include "Portal.h"

void Parser::_Sprites(std::string path)
{
	std::ifstream f;
	f.open(ToLPCWSTR(path));

	if (f.is_open()) {
		std::string str;
		while (std::getline(f, str)) {

			if (str[0] == '#' || str == "") continue;
			std::vector<std::string> tokens = split(str);

			int ID = atoi(tokens[0].c_str());
			int l = atoi(tokens[1].c_str());
			int t = atoi(tokens[2].c_str());
			int r = atoi(tokens[3].c_str());
			int b = atoi(tokens[4].c_str());
			int textureId = atoi(tokens[5].c_str());

			LPDIRECT3DTEXTURE9 texture = Textures::GetInstance()->Get(textureId);
			if (texture == NULL)
			{
				DebugOut(L"[ERROR] Texture ID %d not found!\n", textureId);
				return;
			}

			Sprites::GetInstance()->Add(ID, l, t, r, b, texture);
		}
		f.close();
	}
}

void Parser::_Textures(std::string line)
{
	std::vector<std::string> tokens = split(line);

	if (tokens.size() < 5) return; // skip invalid lines

	int textureId = atoi(tokens[0].c_str());
	std::wstring path = ToWSTR(tokens[1]);
	int R = atoi(tokens[2].c_str());
	int G = atoi(tokens[3].c_str());
	int B = atoi(tokens[4].c_str());

	Textures::GetInstance()->Add(textureId, path.c_str(), D3DCOLOR_XRGB(R, G, B));
}

void Parser::_Animations(std::string line)
{
	std::vector<std::string> tokens = split(line);

	if (tokens.size() < 3) return; // skip invalid lines - an animation must at least has 1 frame and 1 frame time

	//DebugOut(L"--> %s\n",ToWSTR(line).c_str());

	LPANIMATION ani = new Animation();

	int ani_id = atoi(tokens[0].c_str());
	for (size_t i = 1; i < tokens.size(); i += 2)	// why i+=2 ?  sprite_id | frame_time  
	{
		int sprite_id = atoi(tokens[i].c_str());
		int frame_time = atoi(tokens[i + 1].c_str());
		ani->Add(sprite_id, frame_time);
	}

	Animations::GetInstance()->Add(ani_id, ani);
}

void Parser::_AnimationSets(std::string line)
{
	std::vector<std::string> tokens = split(line);

	if (tokens.size() < 2) return; // skip invalid lines - an animation set must at least id and one animation id

	int ani_set_id = atoi(tokens[0].c_str());

	LPANIMATION_SET s = new AnimationSet();
	Animations* animations = Animations::GetInstance();

	for (size_t i = 1; i < tokens.size(); i++)
	{
		int ani_id = atoi(tokens[i].c_str());

		LPANIMATION ani = animations->Get(ani_id);
		s->push_back(ani);
	}

	AnimationSets::GetInstance()->Add(ani_set_id, s);
}

/*
	Parse a line in section [OBJECTS]
*/
std::vector<LPGAMEOBJECT> Parser::_Objects(std::string line, Mario* player)
{
	std::vector<LPGAMEOBJECT> objects;
	std::vector<std::string> tokens = split(line);

	//DebugOut(L"--> %s\n",ToWSTR(line).c_str());

	int object_type = atoi(tokens[0].c_str());
	float x = strtof(tokens[1].c_str(), NULL);
	float y = strtof(tokens[2].c_str(), NULL);

	int ani_set_id = atoi(tokens[3].c_str());

	AnimationSets* animation_sets = AnimationSets::GetInstance();

	GameObject* obj = NULL;

	switch (object_type)
	{
	case OBJECT_TYPE_MARIO:
		if (player != NULL)
		{
			DebugOut(L"[ERROR] MARIO object was created before!\n");
		}
		obj = new Mario(x, y);
		player = (Mario*)obj;

		DebugOut(L"[INFO] Player object created!\n");
		break;
	case OBJECT_TYPE_GOOMBA: obj = new Goomba(); break;
	case OBJECT_TYPE_BRICK: obj = new Brick(); break;
	case OBJECT_TYPE_KOOPAS: obj = new Koopa(); break;
	case OBJECT_TYPE_PORTAL:
	{
		float r = strtof(tokens[4].c_str(), NULL);
		float b = strtof(tokens[5].c_str(), NULL);
		int scene_id = atoi(tokens[6].c_str());
		obj = new Portal(x, y, r, b, scene_id);
	}
	break;
	default:
		DebugOut(L"[ERR] Invalid object type: %d\n", object_type);
	}

	// General object setup
	obj->SetPosition(x, y);

	LPANIMATION_SET ani_set = animation_sets->Get(ani_set_id);

	obj->SetAnimationSet(ani_set);
	objects.push_back(obj);
	return objects;
}