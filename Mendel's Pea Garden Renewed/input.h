#pragma once

#include "stdafx.h"
#include "classes.h"
#include "transform.h"

#define LEFT_JOYSTICK_DEAD	10000.0f
#define RIGHT_JOYSTICK_DEAD 10000.0f
#define LEFT_TRIGGER_DEAD	30.0f
#define RIGHT_TRIGGER_DEAD	30.0f

typedef class tagCONTROLLER
{
public:
	POSITION		GetLJoystickPos() noexcept;										// Get the position of the left joystick.
	POSITION		GetRJoystickPos() noexcept;										// Get the position of the right joystick.
	BOOL			CheckState() noexcept;											// Check the controller state.
	WORD			GetButtonState() noexcept;										// Get the button state.
	constexpr void	InitState(int) noexcept;										// Initializes the state.
	float			GetLTriggerState() noexcept;									// Get the left trigger.
	float			GetRTriggerState() noexcept;									// Get the right trigger.


private:
	int ID;
	XINPUT_STATE state;
} CONTROLLER, *PCONTROLLER;

bool GetKeyStatus(WORD wS, WORD wKey)
{
	return wS & wKey;
}

#pragma region Definitions

// Gets the position of the left joystick.
// Returns the position in POSITION class.
inline POSITION tagCONTROLLER::GetLJoystickPos() noexcept
{
	POSITION pos;

	short sX = state.Gamepad.sThumbLX;
	short sY = state.Gamepad.sThumbLY;

	if (sqrt(sX * sX + sY * sY) > LEFT_JOYSTICK_DEAD)
	{
		float x;
		float y;

		x = (sX - LEFT_JOYSTICK_DEAD) / (32767.0f - LEFT_JOYSTICK_DEAD);
		y = (sY - LEFT_JOYSTICK_DEAD) / (32767.0f - LEFT_JOYSTICK_DEAD);

		pos.SetPosCartesian(x, y);
	}
	else
	{
		pos.SetPosCartesian(0, 0);
	}

	return pos;
}

// Gets the position of the right joystick.
// Returns the position in POSITION class.
inline POSITION tagCONTROLLER::GetRJoystickPos() noexcept
{
	POSITION pos;

	short sX = state.Gamepad.sThumbRX;
	short sY = state.Gamepad.sThumbRY;

	if (sqrt(sX * sX + sY * sY) > LEFT_JOYSTICK_DEAD)
	{
		float x;
		float y;

		x = (sX - LEFT_JOYSTICK_DEAD) / (32767.0f - LEFT_JOYSTICK_DEAD);
		y = (sY - LEFT_JOYSTICK_DEAD) / (32767.0f - LEFT_JOYSTICK_DEAD);

		pos.SetPosCartesian(x, y);
	}
	else
	{
		pos.SetPosCartesian(0, 0);
	}

	return pos;
}

// Checks if a new controller is connected.
// Returns TRUE if connected, FALSE otherwise.
inline BOOL tagCONTROLLER::CheckState() noexcept
{
	DWORD dw;
	DWORD dwPrev;

	dwPrev = state.dwPacketNumber;

	ZeroMemory(&state, sizeof(XINPUT_STATE));

	dw = XInputGetState(ID, &state);

	if (dw == ERROR_SUCCESS)
	{
		return (dwPrev == state.dwPacketNumber) ? FALSE : TRUE;
	}

	return FALSE;
}

// Get the button state. (Button includes LB, RB, A, B, X, Y, DPAD_UP, DPAD_LEFT, DPAD_DOWN, DPAD_RIGHT, LJS, RJS)
// Returns a word that use bits to store button status.
inline WORD tagCONTROLLER::GetButtonState() noexcept
{
	return state.Gamepad.wButtons;
}

// Initializes controller state and ID.
// Parameter(s):
//     i (int): the controller ID.
constexpr void tagCONTROLLER::InitState(int i) noexcept
{
	state.dwPacketNumber = -1;
	ID = i;
}

// Gets the state of LT.
// Returns a regularized float value.
inline float tagCONTROLLER::GetLTriggerState() noexcept
{
	BYTE b = state.Gamepad.bLeftTrigger;

	if (b > LEFT_TRIGGER_DEAD)
	{
		double i = b;
		return (float)(i - LEFT_TRIGGER_DEAD) / (255.0f - LEFT_TRIGGER_DEAD);
	}
	else
	{
		return 0;
	}
}

// Gets the state of RT.
// Returns a regularized float value.
inline float tagCONTROLLER::GetRTriggerState() noexcept
{
	BYTE b = state.Gamepad.bRightTrigger;

	if (b > RIGHT_TRIGGER_DEAD)
	{
		double i = b;
		return (float)(i - RIGHT_TRIGGER_DEAD) / (255.0f - RIGHT_TRIGGER_DEAD);
	}
	else
	{
		return 0;
	}
}

#pragma endregion 


