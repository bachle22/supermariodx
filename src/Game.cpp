#include <fstream>

#include "Game.h"
#include "Camera.h"
#include "ScenePlayer.h"
#include "Debug.h"
#include "Strings.h"

#define MAX_GAME_LINE 1024


#define GAME_FILE_SECTION_UNKNOWN -1
#define GAME_FILE_SECTION_SETTINGS 1
#define GAME_FILE_SECTION_SCENES 2

Game* Game::__instance = NULL;

void Game::Init(HWND hWnd)
{
	d3d = Direct3DCreate9(D3D_SDK_VERSION);
	this->hWnd = hWnd;
	D3DPRESENT_PARAMETERS d3dpp;				// Struct to hold various device information

	ZeroMemory(&d3dpp, sizeof(d3dpp));			// Clear out the struct for use
	d3dpp.Windowed = TRUE;						// Program windowed, not fullscreena
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;	// Discard old frames
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;	// 32-bit RGB pixel format, where 8 bits are reserved for each color.
	d3dpp.hDeviceWindow = hWnd;					// Set the window to be used by DirectX
	d3dpp.BackBufferCount = 0;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

	// Retrieve window width & height so that we can create backbuffer height & width accordingly 
	RECT r;
	GetClientRect(hWnd, &r);

	screenWidth = SCREEN_WIDTH + 1;
	screenHeight = SCREEN_HEIGHT + 1;

	// Create a device class using this information and information from the d3dpp stuct
	d3d->CreateDevice(
		D3DADAPTER_DEFAULT,						// Use default GPU
		D3DDEVTYPE_HAL,
		hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,	// D3DCREATE_SOFTWARE_VERTEXPROCESSING is not recommended since Windows version 1607
		&d3dpp,
		&d3ddev);

	if (d3ddev == NULL)
	{
		DebugOut(L"[ERROR] CreateDevice failed\n");
		return;
	}

	d3ddev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backBuffer);

	// Initialize sprite helper from Direct3DX helper library
	D3DXCreateSprite(d3ddev, &spriteHandler);

	DebugOut(L"[INFO] InitGame done;\n");
	isPaused = false;
}

/*
	Utility function to wrap LPD3DXSPRITE::Draw
*/
void Game::Draw(int nx, float x, float y, LPDIRECT3DTEXTURE9 texture, int left, int top, int right, int bottom, int alpha)
{
	Draw(nx, x, y, texture,
		left, top, right, bottom,
		alpha, D3DXVECTOR2(0, 0), NOROTATE);
}

void Game::Draw(float x, float y, LPDIRECT3DTEXTURE9 texture, int left, int top, int right, int bottom)
{
	Draw(NOFLIP, x, y, texture,
		left, top, right, bottom,
		VISIBLE, D3DXVECTOR2(0, 0), NOROTATE);
}

void Game::Draw(int nx, float x, float y, LPDIRECT3DTEXTURE9 texture, int left, int top, int right, int bottom, int alpha, D3DXVECTOR2 translation)
{
	Draw(nx, x, y, texture,
		left, top, right, bottom,
		VISIBLE, translation, NOROTATE);
}

void Game::Draw(
	int nx, float x, float y,
	LPDIRECT3DTEXTURE9 texture,
	int left, int top, int right, int bottom,
	int alpha, D3DXVECTOR2 translation, int rotation)
{
	D3DXVECTOR3 p(ceil(x - Camera::GetInstance()->GetPosition()->x),
		ceil(y - Camera::GetInstance()->GetPosition()->y), 0);
	RECT r;
	r.left = left;
	r.top = top;
	r.right = right;
	r.bottom = bottom;

	// Flip sprite
	D3DXMATRIX oldTransform;
	D3DXMATRIX newTransform;

	// Retrieve current transformation state
	spriteHandler->GetTransform(&oldTransform);
	D3DXVECTOR2 center = D3DXVECTOR2(p.x + (right - left) / 2, p.y + (bottom - top) / 2);
	D3DXVECTOR2 flip = D3DXVECTOR2(nx > 0 ? -1.0f : 1.0f, 1.0f * rotation);

	D3DXMatrixTransformation2D(&newTransform, &center, NULL, &flip, NULL, 0.0f, &translation);

	spriteHandler->SetTransform(&newTransform);
	spriteHandler->Draw(texture, &r, NULL, &p, D3DCOLOR_ARGB(alpha, 255, 255, 255));
}

int Game::IsKeyDown(int KeyCode)
{
	return (keyStates[KeyCode] & 0x80) > 0;
}


void Game::InitKeyboard()
{
	HRESULT
		hr = DirectInput8Create
		(
			(HINSTANCE)GetWindowLongPtrW(hWnd, GWLP_HINSTANCE),
			DIRECTINPUT_VERSION,
			IID_IDirectInput8, (VOID**)&di, NULL
		);

	if (hr != DI_OK)
	{
		DebugOut(L"[ERROR] DirectInput8Create failed!\n");
		return;
	}

	hr = di->CreateDevice(GUID_SysKeyboard, &didev, NULL);

	// TO-DO: put in exception handling
	if (hr != DI_OK)
	{
		DebugOut(L"[ERROR] CreateDevice failed!\n");
		return;
	}

	// Set the data format to "keyboard format" - a predefined data format 
	//
	// A data format specifies which controls on a device we
	// are interested in, and how they should be reported.
	//
	// This tells DirectInput that we will be passing an array
	// of 256 bytes to IDirectInputDevice::GetDeviceState.

	hr = didev->SetDataFormat(&c_dfDIKeyboard);

	hr = didev->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);


	// IMPORTANT STEP TO USE BUFFERED DEVICE DATA!
	//
	// DirectInput uses unbuffered I/O (buffer size = 0) by default.
	// If you want to read buffered data, you need to set a nonzero
	// buffer size.
	//
	// Set the buffer size to DINPUT_BUFFERSIZE (defined above) elements.
	//
	// The buffer size is a ULONGLONG property associated with the device.
	DIPROPDWORD dipdw;

	dipdw.diph.dwSize = sizeof(DIPROPDWORD);
	dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	dipdw.diph.dwObj = 0;
	dipdw.diph.dwHow = DIPH_DEVICE;
	dipdw.dwData = KEYBOARD_BUFFER_SIZE; // Arbitary buffer size

	hr = didev->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph);

	hr = didev->Acquire();
	if (hr != DI_OK)
	{
		DebugOut(L"[ERROR] DINPUT8::Acquire failed!\n");
		return;
	}

	DebugOut(L"[INFO] Keyboard has been initialized successfully\n");
}

void Game::ProcessKeyboard()
{
	HRESULT hr;

	// Collect all key states first
	hr = didev->GetDeviceState(sizeof(keyStates), keyStates);
	if (FAILED(hr))
	{
		// If the keyboard lost focus or was not acquired then try to get control back.
		if ((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED))
		{
			HRESULT h = didev->Acquire();
			if (h == DI_OK)
			{
				DebugOut(L"[INFO] Keyboard re-acquired!\n");
			}
			else return;
		}
		else
		{
			DebugOut(L"[ERROR] DINPUT::GetDeviceState failed. Error: %d\n", hr);
			return;
		}
	}

	keyHandler->KeyState((BYTE*)&keyStates);



	// Collect all buffered events
	DWORD dwElements = KEYBOARD_BUFFER_SIZE;
	hr = didev->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), keyEvents, &dwElements, 0);
	if (FAILED(hr))
	{
		//DebugOut(L"[ERROR] DINPUT::GetDeviceData failed. Error: %d\n", hr);
		return;
	}

	// Scan through all buffered events, check if the key is pressed or released
	for (DWORD i = 0; i < dwElements; i++)
	{
		int KeyCode = keyEvents[i].dwOfs;
		int KeyState = keyEvents[i].dwData;
		if ((KeyState & 0x80) > 0)
			keyHandler->OnKeyDown(KeyCode);
		else
			keyHandler->OnKeyUp(KeyCode);
	}
}


Game::~Game()
{
	if (spriteHandler != NULL) spriteHandler->Release();
	if (backBuffer != NULL) backBuffer->Release();
	if (d3ddev != NULL) d3ddev->Release();
	if (d3d != NULL) d3d->Release();
}

Game* Game::GetInstance()
{
	if (__instance == NULL) __instance = new Game();
	return __instance;
}

void Game::_ParseSection_SETTINGS(std::string line)
{
	std::vector<std::string> tokens = split(line);

	if (tokens.size() < 2) return;
	if (tokens[0] == "start")
		currentScene = atoi(tokens[1].c_str());
	else
		DebugOut(L"[ERROR] Unknown game setting %s\n", ToWSTR(tokens[0]).c_str());
}

void Game::_ParseSection_SCENES(std::string line)
{
	std::vector<std::string> tokens = split(line);

	if (tokens.size() < 2) return;
	int id = atoi(tokens[0].c_str());
	LPCWSTR path = ToLPCWSTR(tokens[1]);

	LPSCENE scene = new ScenePlayer(id, path);
	scenes[id] = scene;
}


/*
	Load game map file
	TODO: Fix nullptr error when invalid map file is processed
*/
void Game::Load(LPCWSTR gameFile)
{
	DebugOut(L"[INFO] Start loading game file : %s\n", gameFile);

	std::ifstream f;
	f.open(gameFile);
	char str[MAX_GAME_LINE];

	// current resource section flag
	int section = GAME_FILE_SECTION_UNKNOWN;

	while (f.getline(str, MAX_GAME_LINE))
	{
		std::string line(str);

		if (line[0] == '#') continue;	// skip comment lines	

		if (line == "[SETTINGS]") { section = GAME_FILE_SECTION_SETTINGS; continue; }
		if (line == "[SCENES]") { section = GAME_FILE_SECTION_SCENES; continue; }

		//
		// data section
		//
		switch (section)
		{
		case GAME_FILE_SECTION_SETTINGS: _ParseSection_SETTINGS(line); break;
		case GAME_FILE_SECTION_SCENES: _ParseSection_SCENES(line); break;
		}
	}
	f.close();

	DebugOut(L"[INFO] Loading game file : %s has been loaded successfully\n", gameFile);

	SwitchScene(currentScene);
}


void Game::SwitchScene(int scene_id)
{
	DebugOut(L"[INFO] Switching to scene %d\n", scene_id);

	scenes[currentScene]->Unload();
	scenes[currentScene]->SetLoadingStatus(true);

	Textures::GetInstance()->Clear();
	Sprites::GetInstance()->Clear();
	Animations::GetInstance()->Clear();
	AnimationSets::GetInstance()->Clear();

	currentScene = scene_id;
	LPSCENE s = scenes[currentScene];
	Game::GetInstance()->SetKeyHandler(s->GetKeyEventHandler());
	s->Load();
}

void Game::FastSwitchScene(int scene_id)
{
	DebugOut(L"[INFO] Fast switching to scene %d\n", scene_id);

	lastScene = currentScene;
	currentScene = scene_id;

	LPSCENE s = scenes[currentScene];
	Game::GetInstance()->SetKeyHandler(s->GetKeyEventHandler());

	Mario* mario = ((ScenePlayer*)scenes[lastScene])->GetPlayer();
	HUD* hud = ((ScenePlayer*)scenes[lastScene])->GetHUD();
	((ScenePlayer*)scenes[lastScene])->UnsetPlayer();
	((ScenePlayer*)scenes[lastScene])->UnsetHUD();

	if (!scenes[currentScene]->IsLoaded()) s->Load();

	((ScenePlayer*)s)->SetPlayer(mario);
	mario->SetPosition(200, 50);
	((ScenePlayer*)s)->SetHUD(hud);

	int timer;
	ULONGLONG interval;
	((ScenePlayer*)scenes[lastScene])->GetTimer(timer, interval);
	((ScenePlayer*)scenes[currentScene])->SetTimer(timer, interval);

}