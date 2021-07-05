#include <fstream>

#include "Game.h"
#include "ScenePlayer.h"
#include "Parser.h"
#include "Textures.h"
#include "Sprites.h"
#include "Portal.h"
#include "Strings.h"
#include "Debug.h"
#include "Brick.h"
#include "Mario.h"
#include "Goomba.h"
#include "Koopa.h"
#include "Parser.h"

ScenePlayer::ScenePlayer(int id, LPCWSTR filePath) : Scene(id, filePath)
{
	keyHandler = new ScenePlayerInputHandler(this);
}

/*
	Load scene resources from scene file (textures, sprites, animations and objects)
*/

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

void ScenePlayer::_ParseSection_TEXTURES(std::string line)
{
	std::vector<std::string> tokens = split(line);

	if (tokens.size() < 5) {
		DebugOut(L"[WARNING] Invalid texture config in line: %s\n", ToLPCWSTR(line));
		return;
	};

	int textureId = atoi(tokens[0].c_str());
	std::wstring path = ToWSTR(tokens[1]);
	int R = atoi(tokens[2].c_str());
	int G = atoi(tokens[3].c_str());
	int B = atoi(tokens[4].c_str());


	Textures::GetInstance()->Add(textureId, path.c_str(), D3DCOLOR_XRGB(R, G, B));
}

void ScenePlayer::_ParseSection_SPRITES(std::string line)
{
	LPCWSTR path = ToLPCWSTR(line);
	std::ifstream f;
	f.open(path);

	if (f.is_open()) {
		std::string str;
		int linenum = 0;
		while (std::getline(f, str)) {
			linenum++;
			if (str[0] == '#' || str == "") continue;
			std::vector<std::string> tokens = split(str);

			if (tokens.size() < 6) {
				DebugOut(L"[WARNING] Invalid sprite config in line %d: %s\n", linenum, ToLPCWSTR(str));
				return;
			};

			int ID = atoi(tokens[0].c_str());
			int l = atoi(tokens[1].c_str());
			int t = atoi(tokens[2].c_str());
			int r = atoi(tokens[3].c_str());
			int b = atoi(tokens[4].c_str());
			int textureId = atoi(tokens[5].c_str());

			LPDIRECT3DTEXTURE9 texture = Textures::GetInstance()->Get(textureId);
			if (texture == NULL) return;

			Sprites::GetInstance()->Add(ID, l, t, r, b, texture);
		}
		f.close();
	}
}

void ScenePlayer::_ParseSection_ANIMATIONS(std::string line)
{
	LPCWSTR path = ToLPCWSTR(line);
	std::ifstream f;
	f.open(path);

	if (f.is_open()) {
		int linenum = 0;
		std::string str;
		while (std::getline(f, str)) {
			linenum++;
			if (str[0] == '#' || str == "") continue;

			std::vector<std::string> tokens = split(str);
			if (tokens.size() % 2 < 1) {
				DebugOut(L"[WARNING] Invalid animation config in line %d: %s\n", linenum, ToLPCWSTR(str));
				return;
			};

			LPANIMATION ani = new Animation();

			int ani_id = atoi(tokens[0].c_str());
			for (size_t i = 1; i < tokens.size(); i += 2)	// sprite_id | frame_time  
			{
				int sprite_id = atoi(tokens[i].c_str());
				int frame_time = atoi(tokens[i + 1].c_str());
				ani->Add(sprite_id, frame_time);
			}

			Animations::GetInstance()->Add(ani_id, ani);
		}
		f.close();
	}
}

void ScenePlayer::_ParseSection_ANIMATION_SETS(std::string line)
{
	LPCWSTR path = ToLPCWSTR(line);
	std::ifstream f;
	f.open(path);

	if (f.is_open()) {
		std::string str;
		int linenum = 0;
		while (std::getline(f, str)) {
			linenum++;
			if (str[0] == '#' || str == "") continue;

			std::vector<std::string> tokens = split(str);
			if (tokens.size() < 2) {
				DebugOut(L"[WARNING] Invalid animation set config in line %d: %s\n", linenum, ToLPCWSTR(str));
				return;
				// skip invalid lines - an animation set must at least id and one animation id
			}

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
		f.close();
	}
}

/*
	Parse a line in section [OBJECTS]
*/
void ScenePlayer::_ParseSection_OBJECTS(std::string line)
{
	LPCWSTR path = ToLPCWSTR(line);
	std::ifstream f;
	f.open(path);

	if (f.is_open()) {
		std::string str;
		int linenum = 0;
		while (std::getline(f, str)) {
			linenum++;
			if (str[0] == '#' || str == "") continue;

			std::vector<std::string> tokens = split(str);
			if (tokens.size() < 4)
			{
				DebugOut(L"[WARNING] Invalid animation set config in line %d: %s\n", linenum, ToLPCWSTR(str));
				return;
			}

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
					return;
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
				return;
			}

			// General object setup
			obj->SetPosition(x, y);

			LPANIMATION_SET ani_set = animation_sets->Get(ani_set_id);

			obj->SetAnimationSet(ani_set);
			objects.push_back(obj);

		}
		f.close();
	}
}

void ScenePlayer::_ParseSection_TILEDMAP(std::string line)
{
	int id, mapRows, mapColumns, tilesheetColumns, tilesheetRows, totalTiles;
	LPCWSTR path = ToLPCWSTR(line);
	std::ifstream f;
	f.open(path);

	f >> id >> mapRows >> mapColumns >> tilesheetRows >> tilesheetColumns >> totalTiles;

	int** tiles = new int* [mapRows];
	for (int i = 0; i < mapRows; i++)
	{
		tiles[i] = new int[mapColumns];
		for (int j = 0; j < mapColumns; j++)
			f >> tiles[i][j];
	}

	f.close();

	map = new TiledMap(id, mapRows, mapColumns, tilesheetRows, tilesheetColumns, totalTiles);
	map->SetTileSprites();
	map->SetTileMapData(tiles);
}

void ScenePlayer::Load()
{
	DebugOut(L"[INFO] Start loading scene resources from : %s \n", sceneFilePath);

	std::ifstream f;
	f.open(sceneFilePath);

	// current resource section flag
	int section = SCENE_SECTION_UNKNOWN;

	char str[MAX_SCENE_LINE];
	while (f.getline(str, MAX_SCENE_LINE))
	{
		std::string line(str);

		if (line[0] == '#' || line == "") continue;	// skip comment lines	
		if (line == "[TEXTURES]") { section = SCENE_SECTION_TEXTURES; continue; }
		if (line == "[SPRITES]") {
			section = SCENE_SECTION_SPRITES; continue;
		}
		if (line == "[ANIMATIONS]") {
			section = SCENE_SECTION_ANIMATIONS; continue;
		}
		if (line == "[ANIMATION_SETS]") {
			section = SCENE_SECTION_ANIMATION_SETS; continue;
		}
		if (line == "[OBJECTS]") {
			section = SCENE_SECTION_OBJECTS; continue;
		}
		if (line == "[TILEDMAP]") {
			section = SCENE_SECTION_TILEMAP; continue;
		}
		if (line[0] == '[') { section = SCENE_SECTION_UNKNOWN; continue; }

		//
		// data section
		//
		switch (section)
		{
		case SCENE_SECTION_TEXTURES: _ParseSection_TEXTURES(line); break;
		case SCENE_SECTION_SPRITES: _ParseSection_SPRITES(line); break;
		case SCENE_SECTION_ANIMATIONS: _ParseSection_ANIMATIONS(line); break;
		case SCENE_SECTION_ANIMATION_SETS: _ParseSection_ANIMATION_SETS(line); break;
		case SCENE_SECTION_OBJECTS: _ParseSection_OBJECTS(line); break;
		case SCENE_SECTION_TILEMAP: _ParseSection_TILEDMAP(line); break;
		}
	}

	f.close();
	DebugOut(L"[INFO] Done loading scene resources %s\n", sceneFilePath);

	Init();
}

void ScenePlayer::Init()
{
	interval = 0;
	timer = 0;
	hud = new HUD();
}

void ScenePlayer::Update(ULONGLONG dt)
{
	// We know that Mario is the first object in the list hence we won't add him into the colliable object list
	// TO-DO: This is a "dirty" way, need a more organized way 

	std::vector<LPGAMEOBJECT> coObjects;
	for (size_t i = 1; i < objects.size(); i++)
	{
		coObjects.push_back(objects[i]);
	}

	for (size_t i = 0; i < objects.size(); i++)
	{
		objects[i]->Update(dt, &coObjects);
	}

	// skip the rest if scene was already unloaded (Mario::Update might trigger PlayScene::Unload)
	if (player == NULL) return;

	// Update camera to follow mario
	float cx, cy;
	player->GetPosition(cx, cy);

	Game* game = Game::GetInstance();
	cx -= game->GetScreenWidth() / 2;
	cy -= game->GetScreenHeight() / 2;

	Game::GetInstance()->SetCamPos(cx, cy < 180 ? 240.1f - 180 + cy : 240.1f);
	DebugOut(L"cx: %f, cy %f\n", cx, cy);

	// Timer
	interval += dt;
	if (interval / 1000 >= timer && timer < TIME_MAX) {
		if (DEFAULT_MAX_TIME - timer >= 0) hud->SetTime(DEFAULT_MAX_TIME - timer++);
		else player->SetState(MARIO_STATE_DIE);
	}
	hud->SetPowerMeter((GetPlayer()->GetPowerMeter()));
}

void ScenePlayer::Render()
{
	map->Render();
	for (size_t i = 0; i < objects.size(); i++) objects[i]->Render();
	hud->Render();
}

/*
	Unload current scene
*/
void ScenePlayer::Unload()
{
	for (size_t i = 0; i < objects.size(); i++)
		delete objects[i];

	objects.clear();
	player = NULL;

	DebugOut(L"[INFO] Scene %s unloaded! \n", sceneFilePath);
}

void ScenePlayerInputHandler::OnKeyDown(int KeyCode)
{
	//DebugOut(L"[INFO] KeyDown: %d\n", KeyCode);

	Game* game = Game::GetInstance();
	Mario* mario = ((ScenePlayer*)scene)->GetPlayer();
	switch (KeyCode)
	{
	case DIK_SPACE:
		mario->SetState(MARIO_STATE_JUMP);
		break;
	case DIK_LEFT:
		mario->UnsetMovement(RIGHT);
		mario->SetMovement(LEFT);
		break;
	case DIK_RIGHT:
		mario->UnsetMovement(LEFT);
		mario->SetMovement(RIGHT);
		break;
	case DIK_DOWN:
		mario->SetMovement(DOWN);
		break;
	case DIK_S:
		if (!game->IsKeyDown(DIK_X)) mario->SetSuperJump(true);
		if (!mario->IsJumping()) mario->SetMovement(UP);
		break;
	case DIK_X:
		if (!game->IsKeyDown(DIK_S)) mario->SetSuperJump(false);
		if (!mario->IsJumping()) mario->SetMovement(UP);
		if (mario->GetPowerMeter() == MAX_POWER_METER) mario->SetFlying(true);
		break;
	case DIK_A:
		mario->SetPowerIncreament(true);
		break;

	// Cheat keys
	case DIK_1:
		mario->SetMovement(UP);
		mario->SetLevel(MARIO_LEVEL_SMALL);
		break;
	case DIK_2:
		mario->SetMovement(UP);
		mario->SetLevel(MARIO_LEVEL_BIG);
		break;
	case DIK_3:
		mario->SetMovement(UP);
		mario->SetLevel(MARIO_LEVEL_RACOON);
		break;
	case DIK_R:
		mario->Reset();
		break;
	}
}

void ScenePlayerInputHandler::OnKeyUp(int KeyCode)
{
	//DebugOut(L"[INFO] KeyUp: %d\n", KeyCode);

	Game* game = Game::GetInstance();
	Mario* mario = ((ScenePlayer*)scene)->GetPlayer();
	switch (KeyCode)
	{
	case DIK_LEFT:
		if (game->IsKeyDown(DIK_RIGHT)) mario->SetMovement(RIGHT);
		mario->UnsetMovement(LEFT);
		break;
	case DIK_RIGHT:
		if (game->IsKeyDown(DIK_LEFT)) mario->SetMovement(LEFT);
		mario->UnsetMovement(RIGHT);
		break;
	case DIK_DOWN:
		mario->UnsetMovement(DOWN);
		break;
	case DIK_S:
		mario->CanJumpAgain(true);
		mario->SetSuperJump(false);
		mario->UnsetMovement(UP);
		break;
	case DIK_X:
		mario->UnsetMovement(UP);
		mario->SetFlying(false);
		break;
	case DIK_A:
		mario->SetPowerIncreament(false);
		break;

	// Cheat keys
	case DIK_1:
		mario->UnsetMovement(UP);
		break;
	case DIK_2:
		mario->UnsetMovement(UP);
		break;
	case DIK_3:
		mario->UnsetMovement(UP);
		break;
	case DIK_C:
		isCameraFollowing = isCameraFollowing ? false : true;
		break;
	}

}

void ScenePlayerInputHandler::KeyState(BYTE* states)
{
	Game* game = Game::GetInstance();
	Mario* mario = ((ScenePlayer*)scene)->GetPlayer();

	// disable control key when Mario die 
	if (mario->GetState() == MARIO_STATE_DIE) return;
	if (game->IsKeyDown(DIK_L))
	{
		game->DEBUG_X++;
		DebugOut(L"X %d\n", game->DEBUG_X);
	}
	if (game->IsKeyDown(DIK_J))
	{
		game->DEBUG_X--;
		DebugOut(L"X %d\n", game->DEBUG_X);
	}
	if (game->IsKeyDown(DIK_I))
	{
		game->DEBUG_Y--;
		DebugOut(L"Y %d\n", game->DEBUG_Y);
	}
	if (game->IsKeyDown(DIK_K))
	{
		game->DEBUG_Y++;
		DebugOut(L"Y %d\n", game->DEBUG_Y);
	}
}