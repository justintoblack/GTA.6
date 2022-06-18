#pragma once

#ifndef _THECREATOR_H
#define _THECREATOR_H
#include <foundation\PxTransform.h>
#include "../Utils/Mathf.h"
#include <PxPhysicsAPI.h>
#include<ctime>
#include "../SnippetVehicleCommon/SnippetVehicleFilterShader.h"
#include "../SnippetVehicleCommon/SnippetVehicleSceneQuery.h"
#include "../DemoTest/GameObject.h"
#include <list>

using namespace physx;

enum BreakType
{
	SomeInPiece,
	AllInPiece,
};

class TheCreator
{
private:
	PxPhysics* m_gPhysics;
	PxScene* m_gScene;

public:

	//��ʼ��
	void Init(PxPhysics* physics,PxScene* gScene);

	//����λ�ñ�־С��(static)
	void CreateAnchorBall(PxTransform pos, PxMaterial* gMaterial,float radius);

	//����λ�ñ�־������(static)
	void CreateAnchorBox(PxTransform pos, PxMaterial* gMaterial, float halfX,float halfY,float halfZ);

	//������̬����(Dynamic)
	void CreateDynamicSphere(PxTransform pos, PxMaterial* gMaterial, float radius);

	//��������դ��
	void CreateBanister(PxVec3 pos,PxVec3 dir, PxMaterial* gMaterial, float halfHeight, 
		float halfLength, float density, float force, float torque,BreakType type);
	
	//����һ��դ��
	void CreateBanisters(PxVec3 pos, PxVec3 dir, PxMaterial* gMaterial,float separate, 
		physx::PxU32 num, float halfHeight, float halfLength,
		float density, float force, float torque);

	//�����������߸�
	void CreatePole(PxVec3 pos, PxMaterial* gMaterial, float halfXZ, float halfHeight,
		float density, float force, float torque);

	//����һ�ŵ��߸�
	void CreatePoles(PxVec3 pos, PxVec3 dir,float separate,PxU32 num, PxMaterial* gMaterial, float halfXZ, float halfHeight,
		float density, float force, float torque);

	//�������ٴ�
	void createSlowArea(PxVec3 startPosition, PxF32 capsuleRadii, PxF32 interval, PxU32 num, PxMaterial* gMaterial);

};

#endif // !_THECREATOR_H

