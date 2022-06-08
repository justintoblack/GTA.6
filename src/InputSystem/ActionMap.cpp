#include "ActionMap.h"
#include<windows.h>
#include<iostream>
#include "InputSystem.h"

physx::PxVec3 up(0, 1, 0);
extern InputSyetem inputSystem;

void ActionMap::InputAction()
{
	if (GetAsyncKeyState(VK_ESCAPE))
	{
		exit(0);
	}
}




void CharacterActionMap::InputAction()
{
	physx::PxVec3 moveDirection(0, 0, 0);
	physx::PxVec3 forward(m_camera->getDir().x, 0, m_camera->getDir().z);
	physx::PxVec3 right=forward.cross(physx::PxVec3(0,1,0));

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
		moveDirection -=right ;
	}
	if (GetAsyncKeyState('D'))
	{
		moveDirection += right;
	}
	if (GetAsyncKeyState('S'))
	{
		moveDirection -= forward;
	}
	if (GetAsyncKeyState('W'))
	{
		moveDirection +=forward	;
	}
	m_controller->move(moveDirection.getNormalized() * moveSpeed, 0.01f, 0.01f, NULL);
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
}

void VehicleActionMap::SetActionMap(physx::PxVehicleDrive4W* newController)
{
	m_controller = newController;
}
