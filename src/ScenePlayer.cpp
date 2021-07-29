#include <fstream>

#include "Game.h"
#include "Camera.h"
#include "Sprites.h"
#include "Textures.h"
#include "Transition.h"

#include "ScenePlayer.h"
#include "Portal.h"
#include "Strings.h"
#include "Debug.h"
#include "Brick.h"
#include "Goomba.h"
#include "Koopa.h"
#include "Platform.h"
#include "Block.h"
#include "Plant.h"
#include "Roulette.h"
#include "FloatingWood.h"

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
			{
				int type = atoi(tokens[4].c_str());
				obj = new Goomba(type);
				break;
			}
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
				//obj = new Portal(x, y, r, b, scene_id);
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
			case OBJECT_TYPE_COIN:
				obj = new Coin();
				break;
			case OBJECT_TYPE_ROULETTE:
				obj = new Roulette();
				break;
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
	int mapWidth, mapHeight;
	LPCWSTR path = ToLPCWSTR(pathString);
	std::ifstream f;
	f.open(path);
	if (!f) DebugOut(L"[ERROR] Unable to open map config!\n");

	f >> id >> mapRows >> mapColumns >>
		tilesheetRows >> tilesheetColumns >> totalTiles >>
		mapWidth >> mapHeight;

	int** tiles = new int* [mapRows];
	for (int i = 0; i < mapRows; i++)
	{
		tiles[i] = new int[mapColumns];
		for (int j = 0; j < mapColumns; j++)
			f >> tiles[i][j];
	}

	f.close();

	map = new TileMap(id, mapRows, mapColumns, tilesheetRows, tilesheetColumns, totalTiles);
	map->SetMapSize(mapWidth, mapHeight);
	map->SetTileSprites();
	map->SetTileMapData(tiles);
}

void ScenePlayer::_ParseSection_GRID(std::string pathString)
{
	LPCWSTR path = ToLPCWSTR(pathString);
	std::ifstream f;

	f.open(path);
	if (!f) DebugOut(L"[ERROR] Unable to open grid config!\n");

	if (f.is_open()) {
		char str[MAX_SCENE_LINE];
		int section = SCENE_SECTION_UNKNOWN;
		int linenum = 0;

		while (f.getline(str, MAX_SCENE_LINE))
		{
			linenum++;
			std::string line(str);

			if (line[0] == '#' || line == "") continue;

			if (line == "[SETTINGS]") {
				section = SCENE_SECTION_GRID_SETTINGS; continue;
			}
			if (line == "[OBJECTS]") {
				section = SCENE_SECTION_GRID_OBJECTS; continue;
			}
			if (line[0] == '[') { section = SCENE_SECTION_UNKNOWN; continue; }


			switch (section)
			{
			case SCENE_SECTION_GRID_SETTINGS:
			{
				std::vector<std::string> tokens = split(str);
				if (tokens.size() < 2)
				{
					DebugOut(L"[WARNING] Invalid grid setting\n");
					return;
				}
				if (!atoi(tokens[0].c_str()) || !atoi(tokens[1].c_str()))
					return;

				grid = new Grid(atoi(tokens[0].c_str()), atoi(tokens[1].c_str()));
				break;
			}
			case SCENE_SECTION_GRID_OBJECTS:
			{
				std::vector<std::string> tokens = split(str);

				int object_type = atoi(tokens[0].c_str());
				if (tokens.size() < 5 && object_type != OBJECT_TYPE_MARIO)
				{
					DebugOut(L"[WARNING] Invalid grid object config at line %d\n", linenum);
					return;
				}
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
				case OBJECT_TYPE_GOOMBA:
				{
					int type = atoi(tokens[4].c_str());
					obj = new Goomba(type);
					break;
				}
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
					float width = strtof(tokens[3].c_str(), NULL);
					float height = strtof(tokens[4].c_str(), NULL);
					int scene_id = atoi(tokens[5].c_str());
					float destX = strtof(tokens[6].c_str(), NULL);
					float destY = strtof(tokens[7].c_str(), NULL);
					obj = new Portal(width, height, scene_id, destX, destY);
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
				case OBJECT_TYPE_COIN:
					obj = new Coin();
					break;
				case OBJECT_TYPE_ROULETTE:
					obj = new Roulette();
					break;
				case OBJECT_TYPE_FLOATING_WOOD:
					obj = new FloatingWood();
					break;
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

				if (object_type != OBJECT_TYPE_MARIO)
				{
					int gridCol = (int)atoi(tokens[tokens.size() - 1].c_str());
					int gridRow = (int)atoi(tokens[tokens.size() - 2].c_str());
					Unit* unit = new Unit(grid, obj, gridRow, gridCol);
				}

				break;
			}

			default:
				DebugOut(L"[ERR] Invalid section!\n");
				return;
			}
		}
		f.close();
	}
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
		case SCENE_SECTION_GRID: _ParseSection_GRID(line); break;
		}
	}

	f.close();
	DebugOut(L"[INFO] Done loading scene resources %s\n", sceneFilePath);

	Init();
	IsLoaded(true);
}

void ScenePlayer::Init()
{
	hud = new HUD();
	isTransitionDone = false;
	Transition::GetInstance()->FadeOut();
	Game::GetInstance()->Pause();
}

void ScenePlayer::GetObjectFromGrid()
{
	units.clear();
	objects.clear();

	grid->Get(units);

	for (UINT i = 0; i < units.size(); i++)
	{
		LPGAMEOBJECT obj = units[i]->GetObject();
		objects.push_back(obj);
	}
}


void ScenePlayer::Update(ULONGLONG dt)
{
	if (!isTransitionDone)
	{
		if (Transition::GetInstance()->IsFinished())
		{
			Game::GetInstance()->Unpause();
			isTransitionDone = true;
		}
	}

	if (Game::GetInstance()->IsPaused()) dt = 0;

	GetObjectFromGrid();
	// We know that Mario is the first object in the list hence we won't add him into the colliable object list
	// TO-DO: This is a "dirty" way, need a more organized way 
	std::vector<LPGAMEOBJECT> coObjects;
	for (size_t i = 0; i < objects.size(); i++)
	{
		if (objects[i]->IsEnabled())
			coObjects.emplace_back(objects[i]);
	}

	for (size_t i = 0; i < objects.size(); i++)
	{
		if (objects[i]->IsEnabled())
			objects[i]->Update(dt, &coObjects);
	}

	// skip the rest if scene was already unloaded (Mario::Update might trigger PlayScene::Unload)
	if (player == NULL) return;
	player->Update(dt, &coObjects);

	Camera::GetInstance()->Update();

	// Timer
	interval += dt;
	if (interval / 1000 >= timer && timer < TIME_MAX) {
		if (DEFAULT_MAX_TIME - timer >= 0) hud->SetTime(DEFAULT_MAX_TIME - timer++);
		else player->SetState(MARIO_DEAD);
	}
	if (player != NULL) hud->SetPowerMeter(player->GetPowerMeter());

	UpdateGrid();
}

void ScenePlayer::Render()
{
	map->Render();

	std::vector<LPGAMEOBJECT> priotizedObjects;
	for (size_t i = 0; i < objects.size(); i++)
		if (objects[i]->IsEnabled())
		{
			objects[i]->Render();
			if (objects[i]->IsPrioritized())
				priotizedObjects.emplace_back(objects[i]);
		}


	player->Render();

	for (size_t i = 0; i < priotizedObjects.size(); i++)
		priotizedObjects[i]->Render();

	hud->Render();

	Transition::GetInstance()->Render();
	//if (!Transition::GetInstance()->IsFinished());
}

/*
	Unload current scene
*/
void ScenePlayer::Unload()
{
	delete player;
	player = NULL;

	if (grid != NULL) grid->Clear();

	objects.clear();
	units.clear();

	this->IsLoaded(false);
	DebugOut(L"[INFO] Scene %s unloaded! \n", sceneFilePath);
}


void ScenePlayer::AddObject(LPGAMEOBJECT object)
{
	float objectX, objectY;
	object->GetPosition(objectX, objectY);
	Unit* unit = new Unit(grid, object, objectX, objectY);
}

void ScenePlayer::UpdateGrid()
{
	for (int i = 0; i < units.size(); i++)
	{
		LPGAMEOBJECT obj = units[i]->GetObject();

		//if (obj->IsEnabled() == false)
		//	continue;
		float newPosX, newPosY;

		obj->GetPosition(newPosX, newPosY);
		units[i]->Move(newPosX, newPosY);
	}
}

void ScenePlayer::GetTimer(int& timer, ULONGLONG& interval)
{
	timer = this->timer;
	interval = this->interval;
}

void ScenePlayer::SetTimer(int timer, ULONGLONG interval)
{
	this->timer = timer;
	this->interval = interval;
	if (interval / 1000 >= timer && timer < TIME_MAX) {
		if (DEFAULT_MAX_TIME - timer >= 0) hud->SetTime(DEFAULT_MAX_TIME - timer++);
		else player->SetState(MARIO_DEAD);
	}
	hud->SetTime(DEFAULT_MAX_TIME - timer++);
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
		if (!mario->GetAction(ON_PORTAL)) mario->SetMovement(DOWN);
		mario->SetAction(ACTIVATING_PORTAL);
		break;
	case DIK_UP:
		mario->SetAction(ACTIVATING_PORTAL);
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
		mario->SetState(MARIO_BIG_TO_SMALL);
		break;
	case DIK_2:
		mario->SetMovement(UP);
		mario->SetState(MARIO_SMALL_TO_BIG);
		break;
	case DIK_3:
		mario->SetMovement(UP);
		mario->SetState(MARIO_BIG_TO_RACOON);
		break;
	case DIK_R:
		mario->Reset();
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
	case DIK_F5:
		mario->SetPosition(2272, 80);
		break;
		//case DIK_F6:
		//	Game::GetInstance()->FastSwitchScene(11);
		//	break;
		//case DIK_F7:
		//	Game::GetInstance()->FastSwitchScene(10);

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
		mario->UnsetAction(ACTIVATING_PORTAL);
		break;
	case DIK_UP:
		mario->UnsetAction(ACTIVATING_PORTAL);
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
