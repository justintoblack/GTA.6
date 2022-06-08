#pragma once

#ifndef _THECREATOR_H
#define _THECREATOR_H
#include <foundation\PxTransform.h>
#include <PxPhysicsAPI.h>

using namespace physx;

class TheCreator
{
private:
	PxPhysics* m_gPhysics;
	PxScene* m_gScene;

	//创建单个栅栏
	void CreateBanister(PxTransform pos, PxMaterial* gMaterial, float halfHeight, float halfLength, float density, float force, float torque);
public:

	//初始化
	void Init(PxPhysics* physics,PxScene* gScene);

	//创建一排栅栏
	void CreateBanisters(PxTransform pos, PxMaterial* gMaterial,float separate, physx::PxU32 num, float halfHeight, float halfLength,
		float density, float force, float torque);
};

#endif // !_THECREATOR_H
