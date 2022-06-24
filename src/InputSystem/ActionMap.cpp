#include "ActionMap.h"
#include<windows.h>
#include<iostream>
#include "InputSystem.h"

physx::PxVec3 up(0, 1, 0);
POINT p;
extern InputSyetem inputSystem;
extern float deltaTime;
extern bool isInGameMode;

extern void SwitchMode();

ActionMap::ActionMap()
{
	EditKeyEvent = SwitchMode;
}

void ActionMap::InputAction()
{
	//鼠标
	GetCursorPos(&p);
	int dx = lastX - p.x;
	int dy = lastY - p.y;

	if (needToPass)
	{
		needToPass = false;
	}
	else 
	{
		if (isInGameMode)
		{
			sCamera->handleMotion(dx, dy);
		}
		else
		{
			if (GetAsyncKeyState(VK_RBUTTON))
			{
				sCamera->handleMotion(dx, dy);
			}
		}
	}
	//到达窗口边界
	if (isInGameMode)
	{
		if (p.x <= 0 || p.x >= GetSystemMetrics(SM_CXSCREEN) - 1 || p.y <= 0 || p.y >= GetSystemMetrics(SM_CYSCREEN) - 1)
		{
			SetCursorPos(GetSystemMetrics(SM_CXSCREEN) / 2, GetSystemMetrics(SM_CYSCREEN) / 2);
			needToPass = true;
		}
	}

	lastX = p.x;
	lastY = p.y;

	//键盘
	if (GetAsyncKeyState(VK_CONTROL))
	{
		if (!isMKeyDown)
		{
			EditKeyEvent();
			isMKeyDown = true;
		}
	}
	else
	{
		isMKeyDown = false;
	}

	if (GetAsyncKeyState(VK_ESCAPE))
	{
		exit(0);
	}
}


physx::PxVec2 CharacterActionMap::GetArrowKeyValue()
{
	return arrowKey;
}

void CharacterActionMap::InputAction()
{
	physx::PxVec2 arrow(0,0);

	ActionMap::InputAction();
	
	if (GetAsyncKeyState(VK_SPACE))
	{
		if (!isSpaceKeyDown)
		{
		SpaceKeyEvent();
		isSpaceKeyDown = true;
		}
	}
	else
	{
		isSpaceKeyDown = false;
	}

	if (GetAsyncKeyState('A'))
	{
		arrow += physx::PxVec2(0,-1);
	}
	if (GetAsyncKeyState('D'))
	{
		arrow += physx::PxVec2(0, 1);
	}
	if (GetAsyncKeyState('S'))
	{
		arrow += physx::PxVec2(-1, 0);
	}
	if (GetAsyncKeyState('W'))
	{
		arrow += physx::PxVec2(1, 0);
	}
	if (GetAsyncKeyState(VK_LSHIFT))
	{
		ShiftKeyEvent(true);
	}
	else
	{
		ShiftKeyEvent(false);
	}

	if (GetAsyncKeyState(VK_LBUTTON))
	{
		if (!isLeftButtonDown)
		{
			isLeftButtonDown = true;
			LeftButtonDownEvent();
		}
		LeftButtonEvent();
	}
	else
	{
		isLeftButtonDown = false;
	}
	if (GetAsyncKeyState('F'))
	{
		if (!isFKeyDown)
		{
			isFKeyDown = true;
			FKeyEvent();
		}
	}
	else
	{
		isFKeyDown = false;
	}
	if (GetAsyncKeyState(VK_RBUTTON))
	{
		//std::cout << "youjian" << std::endl;
	}
	arrowKey = arrow;
}

void CharacterActionMap::SetActionMap(physx::PxController* newController, Snippets::Camera* camera, float speed)
{
	m_controller = newController;
	m_camera = camera;
	moveSpeed = speed;
}


void VehicleActionMap::InputAction()
{
	ActionMap::InputAction();
	release();
	if (GetAsyncKeyState('W'))
	{
		WKeyEvent();
	}
	else 
	{
		ReleaseWKeyEvent();
	}

	if (GetAsyncKeyState('S'))
	{
		SKeyEvent();
	}

	if (GetAsyncKeyState('A'))
	{
		AKeyEvent();
	}

	if (GetAsyncKeyState('D'))
	{
		DKeyEvent();
	}

	if (GetAsyncKeyState('E'))
	{
		EKeyEvent();
	}
	else 
	{
		ReleaseEKeyEvent();
	}
	if (GetAsyncKeyState('R'))
	{
		RKeyEvent();
	}

	if (GetAsyncKeyState('V'))
	{
		if (!isVKeyDown)
		{
			VKeyEvent();
			isVKeyDown = true;
		}
	}
	else
	{
		isVKeyDown = false;
	}

	if (GetAsyncKeyState('F'))
	{
		if (!isFKeyDown)
		{
			isFKeyDown = true;
			FKeyEvent();
		}
	}
	else
	{
		isFKeyDown = false;
	}
}

void VehicleActionMap::SetActionMap(physx::PxVehicleDrive4W* newController)
{
	m_controller = newController;
}

void EditActionMap::InputAction()
{
	physx::PxVec2 arrow(0, 0);

	ActionMap::InputAction();

	if (GetAsyncKeyState('A'))
	{
		arrow += physx::PxVec2(0, -1);
	}
	if (GetAsyncKeyState('D'))
	{
		arrow += physx::PxVec2(0, 1);
	}
	if (GetAsyncKeyState('S'))
	{
		arrow += physx::PxVec2(-1, 0);
	}
	if (GetAsyncKeyState('W'))
	{
		arrow += physx::PxVec2(1, 0);
	}

	arrowKey = arrow;
}

physx::PxVec2 EditActionMap::GetArrowKeyValue()
{
	return arrowKey;
}
