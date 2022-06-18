#pragma once
#ifndef _CarGameObject_H
#define _CarGameObject_H
#include "../ModelLoading/model.h"

using namespace physx;

extern	PxPhysics* gPhysics;
extern	PxMaterial* gMaterial;
extern	PxScene* gScene;

class CarGameObject
{
private:
public:
	string Name;
	PxTransform transform;
	//PxVec3 scale;

	Model* g_body;
	Model* g_wheel_fl;
	Model* g_wheel_fr;
	Model* g_wheel_bl;
	Model* g_wheel_br;

	PxRigidActor* g_rigidBody;

	CarGameObject()
	{
		transform = PxTransform(0, 0, 0);
		//scale = PxVec3(1, 1, 1);
	}


	//设置GameObject位置
	void SetTransform(PxTransform trans)
	{
		transform = trans;
		g_rigidBody->setGlobalPose(trans);
	}

	//绑定模型
	void AddModel(Model& body,Model& wheel_fl, Model& wheel_fr, Model& wheel_bl, Model& wheel_br )
	{
		g_body = & body;
		g_wheel_fl=&wheel_fl;
		g_wheel_fr=&wheel_fr;
		g_wheel_bl=&wheel_bl;
		g_wheel_br=&wheel_br;

	}


	//添加到场景中
	void AddToScene()
	{
		gScene->addActor(*g_rigidBody);
	}

	void SetRigidbody(PxRigidActor* rigidBody)
	{
		rigidBody->userData = this;
		g_rigidBody = rigidBody;

	}
};
#endif // !CarGameObject
