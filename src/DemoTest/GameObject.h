#pragma once
#ifndef _GameObject_H
#define _GameObject_H
#include <model.h>
#include <PxActor.h>

using namespace physx;

class GameObject
{
private:
public:
	PxVec3 position;
	PxQuat rotation;

	Model* g_model;
	PxActor* g_actor;
};
#endif // !GameObject
