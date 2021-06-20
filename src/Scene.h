#pragma once

#include <d3dx9.h>
#include "Input.h"

class Scene
{
protected:
	InputHandler* key_handler;
	int id;
	LPCWSTR sceneFilePath;

public:
	Scene(int id, LPCWSTR filePath);

	InputHandler* GetKeyEventHandler() { return key_handler; }
	virtual void Load() = 0;
	virtual void Unload() = 0;
	virtual void Update(DWORD dt) = 0;
	virtual void Render() = 0;
};

typedef Scene* LPSCENE;


class SceneInputHandler : public InputHandler
{
protected:
	Scene* scene;

public:
	virtual void KeyState(BYTE* states) = 0;
	virtual void OnKeyDown(int KeyCode) = 0;
	virtual void OnKeyUp(int KeyCode) = 0;
	SceneInputHandler(Scene* s) : InputHandler() { scene = s; }
};