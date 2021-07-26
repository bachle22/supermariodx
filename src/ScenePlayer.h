#pragma once

#include "GameObject.h"
#include "Scene.h"
#include "TileMap.h"
#include "Mario.h"
#include "HUD.h"
#include "Stats.h"
#include "Grid.h"

constexpr int DEFAULT_MAX_TIME = 300;

class ScenePlayer : public Scene
{
	static ScenePlayer* __instance;
	friend class ScenePlayerInputHandler;

protected:
	Mario* player{};
	TileMap* map{};
	Grid* grid{};
	HUD* hud{};
	Stats* stats{};

	ULONGLONG interval{};
	int timer{};
	int score{};

	std::vector<LPGAMEOBJECT> objects;
	std::vector<Unit*> units;

	bool isTransitionDone;

	void GetObjectFromGrid();
	Grid* GetGrid() { return grid; };
	void UpdateGrid();

	void _ParseSection_TEXTURES(std::string line);
	void _ParseSection_SPRITES(std::string line);
	void _ParseSection_ANIMATIONS(std::string line);
	void _ParseSection_ANIMATION_SETS(std::string line);
	void _ParseSection_OBJECTS(std::string line);
	void _ParseSection_TILEMAP(std::string line);
	void _ParseSection_GRID(std::string line);

public:
	ScenePlayer(int id, LPCWSTR filePath);
	void Init();
	virtual void Load();
	virtual void Update(ULONGLONG dt);
	virtual void Render();
	virtual void Unload();

	Mario* GetPlayer() { return player; }
	void SetPlayer(Mario* mario) { player = mario; }
	void UnsetPlayer() { player = NULL; }

	void UnsetTransition() { isTransitionDone = false; }

	void GetTimer(int& timer, ULONGLONG& interval);
	void SetTimer(int timer, ULONGLONG interval);

	HUD* GetHUD() { return hud; }
	void SetHUD(HUD* hud) { this->hud = hud; }
	void UnsetHUD() { this->hud = NULL; }
	std::vector<LPGAMEOBJECT>* GetObjects(){ return &objects; }
	void AddObject(LPGAMEOBJECT object);
};

class ScenePlayerInputHandler : public SceneInputHandler
{
public:
	virtual void KeyState(BYTE* states);
	virtual void OnKeyDown(int KeyCode);
	virtual void OnKeyUp(int KeyCode);
	ScenePlayerInputHandler(LPSCENE s) : SceneInputHandler(s) {};
	bool isCameraFollowing = false;

};

