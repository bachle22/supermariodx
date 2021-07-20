#include <fstream>

#include "Game.h"
#include "Camera.h"
#include "ScenePlayer.h"
#include "Textures.h"
#include "Sprites.h"
#include "Portal.h"
#include "Strings.h"
#include "Debug.h"
#include "Brick.h"
#include "Mario.h"
#include "Goomba.h"
#include "Koopa.h"
#include "Platform.h"
#include "Block.h"
#include "Plant.h"

ScenePlayer::ScenePlayer(int id, LPCWSTR filePath) : Scene(id, filePath)
{
	keyHandler = new ScenePlayerInputHandler(this);
}

/*
	Load scene resources from scene file (textures, sprites, animations and objects)
*/


#define MAX_SCENE_LINE 1024

void ScenePlayer::_ParseSection_TEXTURES(std::string line)
{
	std::vector<std::string> tokens = split(line);

	if (tokens.size() < 5) {
		DebugOut(L"[WARNING] Invalid texture config at line: %s\n", ToLPCWSTR(line));
		return;
	};

	int textureId = atoi(tokens[0].c_str());
	std::wstring path = ToWSTR(tokens[1]);
	int R = atoi(tokens[2].c_str());
	int G = atoi(tokens[3].c_str());
	int B = atoi(tokens[4].c_str());


	Textures::GetInstance()->Add(textureId, path.c_str(), D3DCOLOR_XRGB(R, G, B));
}

void ScenePlayer::_ParseSection_SPRITES(std::string pathString)
{
	LPCWSTR path = ToLPCWSTR(pathString);
	std::ifstream f;
	f.open(path);
	if (!f) DebugOut(L"[ERROR] Unable to open sprite config!\n");

	if (f.is_open()) {
		std::string str;
		int linenum = 0;
		while (std::getline(f, str)) {
			linenum++;
			if (str[0] == '#' || str == "") continue;
			std::vector<std::string> tokens = split(str);

			if (tokens.size() < 6) {
				DebugOut(L"[WARNING] Invalid sprite config at line %d: %s\n", linenum, ToLPCWSTR(str));
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

void ScenePlayer::_ParseSection_ANIMATIONS(std::string pathString)
{
	LPCWSTR path = ToLPCWSTR(pathString);
	std::ifstream f;
	f.open(path);
	if (!f) DebugOut(L"[ERROR] Unable to open animation config!\n");

	if (f.is_open()) {
		int linenum = 0;
		std::string str;
		while (std::getline(f, str)) {
			linenum++;
			if (str[0] == '#' || str == "") continue;

			std::vector<std::string> tokens = split(str);
			if (tokens.size() % 2 < 1) {
				DebugOut(L"[WARNING] Invalid animation config at line %d: %s\n", linenum, ToLPCWSTR(str));
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

void ScenePlayer::_ParseSection_ANIMATION_SETS(std::string pathString)
{
	LPCWSTR path = ToLPCWSTR(pathString);
	std::ifstream f;
	f.open(path);
	if (!f) DebugOut(L"[ERROR] Unable to open animation set config!\n");

	if (f.is_open()) {
		std::string str;
		int linenum = 0;
		while (std::getline(f, str)) {
			linenum++;
			if (str[0] == '#' || str == "") continue;

			std::vector<std::string> tokens = split(str);
			if (tokens.size() < 2) {
				DebugOut(L"[WARNING] Invalid animation set config at line %d: %s\n", linenum, ToLPCWSTR(str));
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
void ScenePlayer::_ParseSection_OBJECTS(std::string pathString)
{
	LPCWSTR path = ToLPCWSTR(pathString);
	std::ifstream f;
	f.open(path);
	if (!f) DebugOut(L"[ERROR] Unable to open game object config!\n");

	if (f.is_open()) {
		std::string str;
		int linenum = 0;
		while (std::getline(f, str)) {
			linenum++;
			if (str[0] == '#' || str == "") continue;

			std::vector<std::string> tokens = split(str);
			if (tokens.size() < 3)
			{
				DebugOut(L"[WARNING] Invalid object config at line %d: %s\n", linenum, ToLPCWSTR(str));
				return;
			}

			int object_type = atoi(tokens[0].c_str());
			float x = strtof(tokens[1].c_str(), NULL);
			float y = strtof(tokens[2].c_str(), NULL);



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
			case OBJECT_TYPE_GOOMBA:
				obj = new Goomba();
				break;
			case OBJECT_TYPE_BRICK:
			{
				int type = atoi(tokens[4].c_str());
				obj = new Brick(type);
				break;
			}
			case OBJECT_TYPE_PLATFORM:
				obj = new Platform();
				break;
			case OBJECT_TYPE_KOOPA:
			{
				int type = atoi(tokens[4].c_str());
				obj = new Koopa(type);
				break;
			}
			case OBJECT_TYPE_PORTAL:
			{
				float r = strtof(tokens[4].c_str(), NULL);
				float b = strtof(tokens[5].c_str(), NULL);
				int scene_id = atoi(tokens[6].c_str());
				obj = new Portal(x, y, r, b, scene_id);
				break;
			}
			case OBJECT_TYPE_BLOCK:
			{
				int w = atoi(tokens[3].c_str());
				int h = atoi(tokens[4].c_str());
				obj = new Block(w, h);
				break;
			}
			case OBJECT_TYPE_PLANT:
			{
				int type = atoi(tokens[4].c_str());
				obj = new Plant(type);
				break;
			}
			default:
				DebugOut(L"[ERR] Invalid object type: %d\n", object_type);
				return;
			}

			// General object setup
			obj->SetPosition(x, y);

			if (object_type != OBJECT_TYPE_PLATFORM &&
				object_type != OBJECT_TYPE_BLOCK)
			{
				int ani_set_id = atoi(tokens[3].c_str());
				LPANIMATION_SET ani_set = animation_sets->Get(ani_set_id);

				obj->SetAnimationSet(ani_set);
			}

			objects.push_back(obj);

		}
		f.close();
	}
}

void ScenePlayer::_ParseSection_TILEMAP(std::string pathString)
{
	int id, mapRows, mapColumns, tilesheetColumns, tilesheetRows, totalTiles;
	LPCWSTR path = ToLPCWSTR(pathString);
	std::ifstream f;
	f.open(path);
	if (!f) DebugOut(L"[ERROR] Unable to open map config!\n");

	f >> id >> mapRows >> mapColumns >> tilesheetRows >> tilesheetColumns >> totalTiles;
	Camera::GetInstance()->SetViewSize(0, 0, 2560, 240);

	int** tiles = new int* [mapRows];
	for (int i = 0; i < mapRows; i++)
	{
		tiles[i] = new int[mapColumns];
		for (int j = 0; j < mapColumns; j++)
			f >> tiles[i][j];
	}

	f.close();

	map = new TileMap(id, mapRows, mapColumns, tilesheetRows, tilesheetColumns, totalTiles);
	map->SetTileSprites();
	map->SetTileMapData(tiles);
}


void ScenePlayer::Load()
{
	DebugOut(L"[INFO] Start loading scene resources from : %s \n", sceneFilePath);

	std::ifstream f;
	f.open(sceneFilePath);
	if (!f) DebugOut(L"[ERROR] Unable to open scene config!\n");

	// current resource section flag
	int section = SCENE_SECTION_UNKNOWN;

	char str[MAX_SCENE_LINE];
	while (f.getline(str, MAX_SCENE_LINE))
	{
		std::string line(str);

		if (line[0] == '#' || line == "") continue;	// skip comment lines	

		if (line == "[TEXTURES]") {
			section = SCENE_SECTION_TEXTURES; continue;
		}
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
		if (line == "[TILEMAP]") {
			section = SCENE_SECTION_TILEMAP; continue;
		}
		if (line == "[GRID]") {
			section = SCENE_SECTION_GRID; continue;
		}
		if (line[0] == '[') { section = SCENE_SECTION_UNKNOWN; continue; }


		switch (section)
		{
		case SCENE_SECTION_TEXTURES: _ParseSection_TEXTURES(line); break;
		case SCENE_SECTION_SPRITES: _ParseSection_SPRITES(line); break;
		case SCENE_SECTION_ANIMATIONS: _ParseSection_ANIMATIONS(line); break;
		case SCENE_SECTION_ANIMATION_SETS: _ParseSection_ANIMATION_SETS(line); break;
		case SCENE_SECTION_OBJECTS: _ParseSection_OBJECTS(line); break;
		case SCENE_SECTION_TILEMAP: _ParseSection_TILEMAP(line); break;
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
	AddObject(hud);
	score = 0;
	stats = Stats::GetInstance();
}

void ScenePlayer::Update(ULONGLONG dt)
{
	if (Game::GetInstance()->IsPaused()) dt = 0;
	// We know that Mario is the first object in the list hence we won't add him into the colliable object list
	// TO-DO: This is a "dirty" way, need a more organized way 
	std::vector<LPGAMEOBJECT> coObjects;
	for (size_t i = 1; i < objects.size(); i++)
	{
		if (objects[i]->IsEnabled())
			coObjects.push_back(objects[i]);
	}

	for (size_t i = 0; i < objects.size(); i++)
	{
		if (objects[i]->IsEnabled())
			objects[i]->Update(dt, &coObjects);
	}

	// skip the rest if scene was already unloaded (Mario::Update might trigger PlayScene::Unload)
	if (player == NULL) return;

	Camera::GetInstance()->Update();

	// Timer
	interval += dt;
	if (interval / 1000 >= timer && timer < TIME_MAX) {
		if (DEFAULT_MAX_TIME - timer >= 0) hud->SetTime(DEFAULT_MAX_TIME - timer++);
		else player->SetState(MARIO_DEAD);
	}
	hud->SetPowerMeter((GetPlayer()->GetPowerMeter()));
}

void ScenePlayer::Render()
{
	map->Render();
	for (size_t i = 0; i < objects.size(); i++)
	{
		if (objects[i]->IsEnabled()) objects[i]->Render();
	}
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
		if (!game->IsKeyDown(DIK_X)) mario->SetAction(SUPER_JUMPING);
		if (!mario->GetAction(JUMPING)) mario->SetMovement(UP);
		break;
	case DIK_X:
		if (!game->IsKeyDown(DIK_S)) mario->UnsetAction(SUPER_JUMPING);
		if (!mario->GetAction(JUMPING)) mario->SetMovement(UP);
		if (mario->GetPowerMeter() == MAX_POWER_METER) mario->SetAction(FLYING);
		mario->SetAction(DESCENDING);
		break;
	case DIK_A:
		mario->SetAction(GAINING_POWER);
		break;
	case DIK_Z:
		mario->SetAction(SPINNING);
		break;
	case DIK_P:
		if (game->IsPaused()) game->Unpause();
		else game->Pause();
		break;

		// Cheat keys
	case DIK_1:
		mario->SetMovement(UP);
		mario->SetState(MARIO_SMALL);
		break;
	case DIK_2:
		mario->SetMovement(UP);
		mario->SetState(MARIO_BIG);
		break;
	case DIK_3:
		mario->SetMovement(UP);
		mario->SetState(MARIO_RACOON);
		break;
	case DIK_4:
		mario->SetState(MARIO_SMALL_TO_BIG);
		break;
	case DIK_5:
		mario->SetState(MARIO_BIG_TO_SMALL);
		break;
	case DIK_R:
		mario->Reset();
		break;
	case DIK_F6:
		mario->SetPosition(2112, 300);
		break;
	case DIK_F1:
		mario->SetPosition(518, 310);
		break;
	case DIK_F2:
		mario->SetPosition(700, 370);
		break;
	case DIK_F3:
		mario->SetPosition(1420, 150);
		break;
	case DIK_F4:
		mario->SetPosition(1825, 300);
		break;
	case DIK_L:
		game->DEBUG_X++;
		DebugOut(L"X %d\n", game->DEBUG_X);
		break;
	case DIK_J:
		game->DEBUG_X--;
		DebugOut(L"X %d\n", game->DEBUG_X);
		break;
	case DIK_I:
		game->DEBUG_Y--;
		DebugOut(L"Y %d\n", game->DEBUG_Y);
		break;
	case DIK_K:
		game->DEBUG_Y++;
		DebugOut(L"Y %d\n", game->DEBUG_Y);
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
		mario->UnsetAction(DONE_JUMPING);
		mario->UnsetAction(SUPER_JUMPING);
		mario->UnsetMovement(UP);
		break;
	case DIK_X:
		mario->UnsetMovement(UP);
		mario->UnsetAction(FLYING);
		mario->UnsetAction(DESCENDING);
		break;
	case DIK_Z:
		mario->UnsetAction(SPINNING);
		break;
	case DIK_A:
		mario->UnsetAction(GAINING_POWER);
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
	if (mario->GetState() == MARIO_DEAD) return;
	//if (game->IsKeyDown(DIK_L))
	//{
	//	game->DEBUG_X++;
	//	DebugOut(L"X %d\n", game->DEBUG_X);
	//}
	//if (game->IsKeyDown(DIK_J))
	//{
	//	game->DEBUG_X--;
	//	DebugOut(L"X %d\n", game->DEBUG_X);
	//}
	//if (game->IsKeyDown(DIK_I))
	//{
	//	game->DEBUG_Y--;
	//	DebugOut(L"Y %d\n", game->DEBUG_Y);
	//}
	//if (game->IsKeyDown(DIK_K))
	//{
	//	game->DEBUG_Y++;
	//	DebugOut(L"Y %d\n", game->DEBUG_Y);
	//}
}
