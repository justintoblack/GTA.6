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

	//��������դ��
	void CreateBanister(PxTransform pos, PxMaterial* gMaterial, float halfHeight, float halfLength, float density, float force, float torque);
public:

	//��ʼ��
	void Init(PxPhysics* physics,PxScene* gScene);

	//����һ��դ��
	void CreateBanisters(PxTransform pos, PxMaterial* gMaterial,float separate, physx::PxU32 num, float halfHeight, float halfLength,
		float density, float force, float torque);
};

#endif // !_THECREATOR_H
