#pragma once

#include "GameObject.h"
#include "Scene.h"
#include "TileMap.h"
#include "Mario.h"
#include "HUD.h"

constexpr int DEFAULT_MAX_TIME = 300;

class ScenePlayer : public Scene
{
	static ScenePlayer* __instance;
	friend class ScenePlayerInputHandler;

protected:
	Mario* player;
	TileMap* map;
	HUD* hud;

	ULONGLONG interval;
	int timer;

	std::vector<LPGAMEOBJECT> objects;

	void _ParseSection_TEXTURES(std::string line);
	void _ParseSection_SPRITES(std::string line);
	void _ParseSection_ANIMATIONS(std::string line);
	void _ParseSection_ANIMATION_SETS(std::string line);
	void _ParseSection_OBJECTS(std::string line);
	void _ParseSection_TILEMAP(std::string line);
public:
	ScenePlayer(int id, LPCWSTR filePath);
	void Init();
	virtual void Load();
	virtual void Update(ULONGLONG dt);
	virtual void Render();
	virtual void Unload();

	Mario* GetPlayer() { return player; }
	HUD* GetHUD() { return hud; }

	void AddObject(LPGAMEOBJECT object) { objects.push_back(object); }

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

