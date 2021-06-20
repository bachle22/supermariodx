#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>

#include "Animations.h"
#include "Debug.h"
#include "Definition.h"
#include "Game.h"
#include "Mario.h"
#include "Textures.h"
#include "Sprites.h"
#include "Input.h"
#include "Collision.h"

#include "Brick.h"
#include "Goomba.h"

#define BACKGROUND_COLOR D3DCOLOR_XRGB(200, 200, 255)


Game* game;
Mario* mario;
Goomba* goomba;
std::vector<LPGAMEOBJECT> objects;

class SampleKeyHandler : public InputHandler
{
	virtual void KeyState(BYTE* states);
	virtual void OnKeyDown(int KeyCode);
	virtual void OnKeyUp(int KeyCode);
};

SampleKeyHandler* keyHandler;

void SampleKeyHandler::OnKeyDown(int KeyCode)
{
	DebugOut(L"[INFO] KeyDown: %d\n", KeyCode);
	switch (KeyCode)
	{
	case DIK_SPACE:
		mario->SetState(MARIO_STATE_JUMP);
		break;
	case DIK_A: // reset
		mario->SetState(MARIO_STATE_IDLE);
		mario->SetLevel(MARIO_LEVEL_BIG);
		mario->SetPosition(50.0f, 0.0f);
		mario->SetSpeed(0, 0);
		break;
	}
}

void SampleKeyHandler::OnKeyUp(int KeyCode)
{
	DebugOut(L"[INFO] KeyUp: %d\n", KeyCode);
}

void SampleKeyHandler::KeyState(BYTE* states)
{
	// disable control key when Mario die 
	if (mario->GetState() == MARIO_STATE_DIE) return;
	if (game->IsKeyDown(DIK_RIGHT))
		mario->SetState(MARIO_STATE_WALKING_RIGHT);
	else if (game->IsKeyDown(DIK_LEFT))
		mario->SetState(MARIO_STATE_WALKING_LEFT);
	else
		mario->SetState(MARIO_STATE_IDLE);
}


LRESULT CALLBACK WindowProc(
	_In_ HWND hWnd,
	_In_ UINT message,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

void LoadResources()
{
	Textures* textures = Textures::GetInstance();

	textures->Add(ID_TEX_MARIO, L"..\\assets\\textures\\mario.png", D3DCOLOR_XRGB(255, 255, 255));
	textures->Add(ID_TEX_MISC, L"..\\assets\\textures\\misc.png", D3DCOLOR_XRGB(176, 224, 248));
	textures->Add(ID_TEX_ENEMY, L"..\\assets\\textures\\enemies.png", D3DCOLOR_XRGB(3, 26, 110));

	textures->Add(ID_TEX_BBOX, L"..\\assets\\textures\\bbox.png", D3DCOLOR_XRGB(255, 255, 255));


	Sprites* sprites = Sprites::GetInstance();
	Animations* animations = Animations::GetInstance();


	LPDIRECT3DTEXTURE9 texMario = textures->Get(ID_TEX_MARIO);

	// big
	sprites->Add(10001, 246, 154, 260, 181, texMario);		// idle right

	sprites->Add(10002, 275, 154, 290, 181, texMario);		// walk
	sprites->Add(10003, 304, 154, 321, 181, texMario);

	sprites->Add(10011, 186, 154, 200, 181, texMario);		// idle left
	sprites->Add(10012, 155, 154, 170, 181, texMario);		// walk
	sprites->Add(10013, 125, 154, 140, 181, texMario);

	sprites->Add(10099, 215, 120, 231, 135, texMario);		// die 

	// small
	sprites->Add(10021, 247, 0, 259, 15, texMario);			// idle small right
	sprites->Add(10022, 275, 0, 291, 15, texMario);			// walk 
	sprites->Add(10023, 306, 0, 320, 15, texMario);			// 

	sprites->Add(10031, 187, 0, 198, 15, texMario);			// idle small left

	sprites->Add(10032, 155, 0, 170, 15, texMario);			// walk
	sprites->Add(10033, 125, 0, 139, 15, texMario);			// 


	LPDIRECT3DTEXTURE9 texMisc = textures->Get(ID_TEX_MISC);
	sprites->Add(20001, 408, 225, 424, 241, texMisc);

	LPDIRECT3DTEXTURE9 texEnemy = textures->Get(ID_TEX_ENEMY);
	sprites->Add(30001, 5, 14, 21, 29, texEnemy);
	sprites->Add(30002, 25, 14, 41, 29, texEnemy);

	sprites->Add(30003, 45, 21, 61, 29, texEnemy); // die sprite

	LPANIMATION ani;

	ani = new Animation(100);	// idle big right
	ani->Add(10001);
	animations->Add(400, ani);

	ani = new Animation(100);	// idle big left
	ani->Add(10011);
	animations->Add(401, ani);

	ani = new Animation(100);	// idle small right
	ani->Add(10021);
	animations->Add(402, ani);

	ani = new Animation(100);	// idle small left
	ani->Add(10031);
	animations->Add(403, ani);

	ani = new Animation(100);	// walk right big
	ani->Add(10001);
	ani->Add(10002);
	ani->Add(10003);
	animations->Add(500, ani);

	ani = new Animation(100);	// // walk left big
	ani->Add(10011);
	ani->Add(10012);
	ani->Add(10013);
	animations->Add(501, ani);

	ani = new Animation(100);	// walk right small
	ani->Add(10021);
	ani->Add(10022);
	ani->Add(10023);
	animations->Add(502, ani);

	ani = new Animation(100);	// walk left small
	ani->Add(10031);
	ani->Add(10032);
	ani->Add(10033);
	animations->Add(503, ani);


	ani = new Animation(100);		// Mario die
	ani->Add(10099);
	animations->Add(599, ani);



	ani = new Animation(100);		// brick
	ani->Add(20001);
	animations->Add(601, ani);

	ani = new Animation(300);		// Goomba walk
	ani->Add(30001);
	ani->Add(30002);
	animations->Add(701, ani);

	ani = new Animation(1000);		// Goomba dead
	ani->Add(30003);
	animations->Add(702, ani);

	mario = new Mario();
	mario->AddAnimation(400);		// idle right big
	mario->AddAnimation(401);		// idle left big
	mario->AddAnimation(402);		// idle right small
	mario->AddAnimation(403);		// idle left small

	mario->AddAnimation(500);		// walk right big
	mario->AddAnimation(501);		// walk left big
	mario->AddAnimation(502);		// walk right small
	mario->AddAnimation(503);		// walk left big

	mario->AddAnimation(599);		// die

	mario->SetPosition(50.0f, 0);
	objects.push_back(mario);
	for (int i = 0; i < 5; i++)
	{
		Brick* brick = new Brick();
		brick->AddAnimation(601);
		brick->SetPosition(100.0f + i * 60.0f, 74.0f);
		objects.push_back(brick);

		brick = new Brick();
		brick->AddAnimation(601);
		brick->SetPosition(100.0f + i * 60.0f, 90.0f);
		objects.push_back(brick);

		brick = new Brick();
		brick->AddAnimation(601);
		brick->SetPosition(84.0f + i * 60.0f, 90.0f);
		objects.push_back(brick);
	}


	for (int i = 0; i < 100; i++)
	{
		Brick* brick = new Brick();
		brick->AddAnimation(601);
		brick->SetPosition(0 + i * 16.0f, 150);
		objects.push_back(brick);
	}

	// and Goombas 
	for (int i = 0; i < 4; i++)
	{
		goomba = new Goomba();
		goomba->AddAnimation(701);
		goomba->AddAnimation(702);
		goomba->SetPosition(200 + i * 60, 135);
		goomba->SetState(GOOMBA_STATE_WALKING);
		objects.push_back(goomba);
	}
}

void Render(void)
{
	Game* game = Game::GetInstance();
	LPDIRECT3DDEVICE9 d3ddev = game->GetDirect3DDevice();
	LPDIRECT3DSURFACE9 backbuffer = game->GetBackBuffer();
	LPD3DXSPRITE spriteHandler = game->GetSpriteHandler();


	// Clear out back buffer
	d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, BACKGROUND_COLOR, 1.0f, 0);
	d3ddev->BeginScene();

	// Render sprites & animation
	spriteHandler->Begin(D3DXSPRITE_ALPHABLEND);

	for (int i = 0; i < objects.size(); i++)
		objects[i]->Render();

	spriteHandler->End();
	d3ddev->EndScene();

	d3ddev->Present(NULL, NULL, NULL, NULL);    // Displays the created frame
}

/*
	Update world status for this frame
	dt: time period between beginning of last frame and beginning of this frame
*/
void Update(DWORD dt)
{
	// TO-DO: This is a "dirty" way, need a more organized way 

	std::vector<LPGAMEOBJECT> coObjects;
	for (int i = 1; i < objects.size(); i++)
	{
		coObjects.push_back(objects[i]);
	}

	for (int i = 0; i < objects.size(); i++)
	{
		objects[i]->Update(dt, &coObjects);
	}

	// Update camera to follow mario
	float cx, cy;
	mario->GetPosition(cx, cy);

	cx -= SCREEN_WIDTH / 2;
	cy -= SCREEN_HEIGHT / 2;

	Game::GetInstance()->SetCamPos(cx, 0.0f /*cy*/);

}

HWND CreateGameWindow(
	HINSTANCE hInstance,
	int nCmdShow,
	LPCWSTR WindowClassName,
	LPCWSTR WindowTitle,
	int ScreenWidth,
	int ScreenHeight)
{
	WNDCLASSEX wc;

	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.hInstance = hInstance;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpszClassName = WindowClassName;

	RegisterClassEx(&wc);

	HWND hWnd = CreateWindow(
		WindowClassName,
		WindowTitle,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,			// X-position of the window
		CW_USEDEFAULT,			// Y-position of the window
		ScreenWidth,
		ScreenHeight,
		NULL,					// No parent window, NULL
		NULL,					// No menu, NULL
		hInstance,
		NULL);

	if (!hWnd)
	{
		DWORD ErrCode = GetLastError();
		DebugOut(L"[ERROR] CreateWindow failed");
		return 0;
	}
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	return hWnd;
}

WPARAM HandleWindowMessage(void)
{
	bool isDone = 0;
	MSG msg;
	DWORD frameStart = GetTickCount64();
	DWORD tickPerFrame = 1000 / MAX_FRAME_RATE;

	while (!isDone)
	{
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{

			TranslateMessage(&msg);
			DispatchMessage(&msg);

			switch (msg.message)
			{
			case WM_QUIT:
				isDone = 1;
				break;
			case WM_SIZING:
				// TODO: Handle window resizing
				break;
			}
		}

		DWORD now = GetTickCount64();

		// dt: the time between (beginning of last frame) and now
		// this frame: the frame we are about to render
		DWORD dt = now - frameStart;

		if (dt >= tickPerFrame)
		{
			frameStart = now;
			game->ProcessKeyboard();
			Update(dt);
			Render();
		}
		else Sleep(tickPerFrame - dt);
	}
	return msg.wParam;
}

int WINAPI WinMain(
	_In_     HINSTANCE	hInstance,
	_In_opt_ HINSTANCE	hPrevInstance,
	_In_     LPSTR		lpCmdLine,
	_In_     int		nCmdShow)
{
	HWND hWnd = CreateGameWindow(
		hInstance, nCmdShow,
		WINDOW_CLASS_NAME, MAIN_WINDOW_TITLE,
		SCREEN_WIDTH, SCREEN_HEIGHT);

	//SetDebugWindow(hWnd);

	game = Game::GetInstance();
	game->Init(hWnd);

	keyHandler = new SampleKeyHandler();
	game->InitKeyboard(keyHandler);

	LoadResources();
	SetWindowPos(hWnd, 0, 0, 0, SCREEN_WIDTH * 2, SCREEN_HEIGHT * 2, SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);
	HandleWindowMessage();

}

