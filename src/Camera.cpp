#include "Camera.h"
#include "ScenePlayer.h"
#include "Debug.h"

Camera* Camera::__instance = NULL;

Camera::Camera()
{
	position = D3DXVECTOR3(0, 0, 0);

	width = Game::GetInstance()->GetScreenWidth();
	height = Game::GetInstance()->GetScreenHeight();

	viewLeft = 0; viewTop = 0; 
	viewRight = 0; viewBottom = 0;
}

Camera* Camera::GetInstance()
{
	if (__instance == NULL) __instance = new Camera();
	return __instance;
}

void Camera::SetPosition(D3DXVECTOR3 position)
{
	this->position = position;
}

void Camera::SetPosition(float x, float y)
{
	SetPosition(D3DXVECTOR3(x, y, 0));
}

void Camera::SetViewSize(float left, float top, float right, float bottom)
{
	viewLeft = left;
	viewTop = top;
	viewRight = right;
	viewBottom = bottom;
}

void Camera::Update()
{
	float cx, cy;
	Mario* mario = ((ScenePlayer*)Game::GetInstance()->GetCurrentScene())->GetPlayer();

	mario->GetPosition(cx, cy);
	cx -= width / 2; cy -= height / 2;
	if (cx < viewLeft) cx = 0;
	else if (cx > viewRight) cx = viewRight;

	// TODO: Only follow Mario vertical position when on the cloud
	if (cy < CAMERA_CEIL_Y && mario->GetState() == MARIO_RACOON)
		cy = viewBottom - CAMERA_CEIL_Y + cy;
	else cy = viewBottom;
	if (cy < viewTop) cy = 0;

	SetPosition(floor(cx) + Game::GetInstance()->DEBUG_X, (cy) + Game::GetInstance()->DEBUG_Y);
}