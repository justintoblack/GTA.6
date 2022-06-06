#pragma once
#ifndef _ACTIONMAP_H
#define _ACTIONMAP_H

#include<PxPhysicsAPI.h>
#include "../Render/Camera.h"

class ActionMap
{
private:
public:
	void InputAction();
};

class CharacterActionMap :public ActionMap
{
private:
	physx::PxController* m_controller;
	Snippets::Camera* m_camera;
	float moveSpeed;
public:
	void InputAction();
	void SetController(physx::PxController* newController);
	void SetCamera(Snippets::Camera* camera);
	void SetSpeed(float speed);
};

class VehicleActionMap : public ActionMap
{
private:
public:
};

#endif // !_ACTIONMAP_H
