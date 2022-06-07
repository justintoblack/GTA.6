#pragma once
#ifndef _ACTIONMAP_H
#define _ACTIONMAP_H

#include<PxPhysicsAPI.h>
#include "../Render/Camera.h"


class ActionMap
{
private:
	bool _qKeyDown;
public:
	virtual void InputAction();
};

class CharacterActionMap :public ActionMap
{
private:
	physx::PxController* m_controller;
	Snippets::Camera* m_camera;
	float moveSpeed;
public:
	void InputAction();
	void SetActionMap(physx::PxController* newController, Snippets::Camera* camera, float speed);
};

class VehicleActionMap : public ActionMap
{
private:
	physx::PxVehicleDrive4W* m_controller;
public:
	//º¯ÊýÖ¸Õë
	void (*release)();
	void (*WKeyEvent)();
	void (*SKeyEvent)();
	void (*AKeyEvent)();
	void (*DKeyEvent)();

	void InputAction();
	void SetActionMap(physx::PxVehicleDrive4W* newController);
};

#endif // !_ACTIONMAP_H
