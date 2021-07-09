#pragma once

#include <d3dx9.h>
#include "Input.h"

enum SceneSection
{
	SCENE_SECTION_UNKNOWN = -1,
	SCENE_SECTION_TEXTURES = 2,
	SCENE_SECTION_SPRITES = 3,
	SCENE_SECTION_ANIMATIONS = 4,
	SCENE_SECTION_ANIMATION_SETS = 5,
	SCENE_SECTION_OBJECTS = 6,
	SCENE_SECTION_TILEMAP = 7,
	SCENE_SECTION_GRID = 8,
};

class Scene
{
protected:
	int id;
	LPCWSTR sceneFilePath;
	LPKEYEVENTHANDLER keyHandler;

public:
	Scene(int id, LPCWSTR filePath);
	~Scene();
	InputHandler* GetKeyEventHandler() { return keyHandler; }
	virtual void Load() = 0;
	virtual void Unload() = 0;
	virtual void Update(ULONGLONG dt) = 0;
	virtual void Render() = 0;
};

typedef Scene* LPSCENE;


class SceneInputHandler : public InputHandler
{
protected:
	LPSCENE scene;

public:
	virtual void KeyState(BYTE* states) = 0;
	virtual void OnKeyDown(int KeyCode) = 0;
	virtual void OnKeyUp(int KeyCode) = 0;
	SceneInputHandler(LPSCENE s) : InputHandler() { scene = s; }
};