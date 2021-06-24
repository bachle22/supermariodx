#pragma once

#include <dinput.h>
#include <windows.h>

constexpr int KEYBOARD_BUFFER_SIZE = 1024;

/*
Abstract class to define keyboard event handlers
*/
class InputHandler
{
public:
	virtual void KeyState(BYTE* state) = 0;
	virtual void OnKeyDown(int KeyCode) = 0;
	virtual void OnKeyUp(int KeyCode) = 0;
};

typedef InputHandler* LPKEYEVENTHANDLER;
