#include "ActionMap.h"
#include<windows.h>
#include<iostream>
#include "InputSystem.h"

physx::PxVec3 up(0, 1, 0);
extern InputSyetem inputSystem;
extern float deltaTime;

void ActionMap::InputAction()
{
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

}

void VehicleActionMap::SetActionMap(physx::PxVehicleDrive4W* newController)
{
	m_controller = newController;
}
