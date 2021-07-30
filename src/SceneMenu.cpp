#include <fstream>

#include "SceneMenu.h"
#include "Game.h"
#include "MenuObject.h"
#include "Textures.h"
#include "Sprites.h"
#include "Definition.h"
#include "Transition.h"
#include "Strings.h"
#include "Debug.h"
#include "Camera.h"


SceneMenu::SceneMenu(int id, LPCWSTR filePath) : Scene(id, filePath)
{
	keyHandler = new SceneMenuInputHandler(this);
}

/*
	Load scene resources from scene file (textures, sprites, animations and objects)
*/
void SceneMenu::_ParseSection_TEXTURES(std::string line)
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

void SceneMenu::_ParseSection_SPRITES(std::string pathString)
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

void SceneMenu::_ParseSection_ANIMATIONS(std::string pathString)
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

void SceneMenu::_ParseSection_ANIMATION_SETS(std::string pathString)
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
void SceneMenu::_ParseSection_OBJECTS(std::string pathString)
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
					DebugOut(L"[ERROR] PLAYER object was created before!\n");
					return;
				}
				obj = new MenuPlayer();
				player = (MenuPlayer*)obj;
				DebugOut(L"[INFO] Player object created!\n");
				break;

			case OBJECT_TYPE_MENUOBJECT:
			{
				int type = atoi(tokens[4].c_str());
				obj = new MenuObject(type);
				if (type == OBJECT_TYPE_PANEL || type == OBJECT_TYPE_STOP)
				{
					bool l, r, u, d;
					l = atof(tokens[5].c_str());
					r = atof(tokens[6].c_str());
					u = atof(tokens[7].c_str());
					d = atof(tokens[8].c_str());
					((MenuObject*)obj)->SetMovement(l, r, u, d);

					int id = atof(tokens[9].c_str());
					((MenuObject*)obj)->SetSceneId(id);
				}
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

void SceneMenu::_ParseSection_TILEMAP(std::string pathString)
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

void SceneMenu::Load()
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

	Init();

	DebugOut(L"[INFO] Done loading scene resources %s\n", sceneFilePath);
}

void SceneMenu::Init()
{
	hud = new HUD();
	isTransitionDone = false;
	Transition::GetInstance()->FadeOut();
	//Game::GetInstance()->Pause();
}

void SceneMenu::Update(ULONGLONG dt)
{
	Camera::GetInstance()->Update();
	if (!isTransitionDone)
	{
		if (Transition::GetInstance()->IsFinished())
		{
			Game::GetInstance()->Unpause();
			isTransitionDone = true;
		}
	}

	if (Game::GetInstance()->IsPaused()) dt = 0;

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

}
void SceneMenu::Render()
{
	map->Render();
	for (int i = 0; i < objects.size(); i++) objects[i]->Render();
	hud->Render();
}

/*
	Unload current scene
*/
void SceneMenu::Unload()
{
	for (int i = 0; i < objects.size(); i++)
		delete objects[i];
	objects.clear();
	player = NULL;

	DebugOut(L"[INFO] Scene %s unloaded! \n", sceneFilePath);
}


void SceneMenuInputHandler::OnKeyDown(int KeyCode)
{
	MenuPlayer* player = ((SceneMenu*)scene)->GetPlayer();
	if (player != NULL)
	{
		switch (KeyCode)
		{
		case DIK_RIGHT:
			player->SetState(PLAYER_STATE_RIGHT);
			break;
		case DIK_LEFT:
			player->SetState(PLAYER_STATE_LEFT);
			break;
		case DIK_UP:
			player->SetState(PLAYER_STATE_UP);
			break;
		case DIK_DOWN:
			player->SetState(PLAYER_STATE_DOWN);
			break;
		case DIK_S:
			player->SwitchScene();
			break;

		}
	}
}

void SceneMenuInputHandler::OnKeyUp(int KeyCode)
{
	//DebugOut(L"[INFO] KeyUp: %d\n", KeyCode);

	Game* game = Game::GetInstance();

}

void SceneMenuInputHandler::KeyState(BYTE* states)
{
	Game* game = Game::GetInstance();


}
