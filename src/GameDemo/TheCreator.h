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

	//初始化
	void Init(PxPhysics* physics,PxScene* gScene);

	//创建位置标志小球(static)
	void CreateAnchorBall(PxTransform pos, PxMaterial* gMaterial,float radius);

	//创建位置标志立方体(static)
	void CreateAnchorBox(PxTransform pos, PxMaterial* gMaterial, float halfX,float halfY,float halfZ);

	//创建动态球体(Dynamic)
	void CreateDynamicSphere(PxTransform pos, PxMaterial* gMaterial, float radius);

	//创建单个栅栏
	void CreateBanister(PxVec3 pos,PxVec3 dir, PxMaterial* gMaterial, float halfHeight, 
		float halfLength, float density, float force, float torque,BreakType type);
	
	//创建一排栅栏
	void CreateBanisters(PxVec3 pos, PxVec3 dir, PxMaterial* gMaterial,float separate, 
		physx::PxU32 num, float halfHeight, float halfLength,
		float density, float force, float torque);

	//创建单个电线杆
	void CreatePole(PxVec3 pos, PxMaterial* gMaterial, float halfXZ, float halfHeight,
		float density, float force, float torque);

	//创建一排电线杆
	void CreatePoles(PxVec3 pos, PxVec3 dir,float separate,PxU32 num, PxMaterial* gMaterial, float halfXZ, float halfHeight,
		float density, float force, float torque);

	//创建减速带
	void createSlowArea(PxVec3 startPosition, PxF32 capsuleRadii, PxF32 interval, PxU32 num, PxMaterial* gMaterial);

};

#endif // !_THECREATOR_H

