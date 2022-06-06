#include "ActionMap.h"
#include<windows.h>
#include<iostream>

physx::PxVec3 up(0, 1, 0);

void CharacterActionMap::InputAction()
{
	physx::PxVec3 moveDirection(0, 0, 0);
	physx::PxVec3 forward(m_camera->getDir().x, 0, m_camera->getDir().z);
	physx::PxVec3 right=forward.cross(physx::PxVec3(0,1,0));
	if (GetAsyncKeyState(VK_ESCAPE))
	{
		exit(0);
	}

	if (GetAsyncKeyState(VK_SPACE))
	{
		
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
	m_controller->move(physx::PxVec3(0, -0.05f, 0) + moveDirection.getNormalized() * moveSpeed, 0.01f, 0.01f, NULL);
}

void CharacterActionMap::SetController(physx::PxController* newController)
{
	m_controller = newController;
}

void CharacterActionMap::SetCamera(Snippets::Camera* camera)
{
	m_camera = camera;
}

void CharacterActionMap::SetSpeed(float speed)
{
	moveSpeed = speed;
}

void ActionMap::InputAction()
{
	std::cout << 1;
}
