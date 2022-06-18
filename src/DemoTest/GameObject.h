#pragma once
#ifndef _GameObject_H
#define _GameObject_H
#include "../ModelLoading/model.h"

using namespace physx;

extern	PxPhysics* gPhysics;
extern	PxMaterial* gMaterial;
extern	PxScene* gScene;


class GameObject
{
private:
public:
	char Name[32]="gameObject";
	PxTransform transform;
	PxTransform localTransform;
	GameObject* parent=nullptr;
	//PxVec3 scale;

	Model*  g_model=nullptr;
	PxRigidActor*  g_rigidBody=nullptr;

	GameObject()
	{
		transform = PxTransform(0,0,0);
		//scale = PxVec3(1, 1, 1);
	}

	//设置GameObject位置
	void SetTransform(PxTransform trans)
	{
		transform = trans;
		if (g_rigidBody != nullptr)
		{
			g_rigidBody->setGlobalPose(trans);
		}
	}

	//设置局部坐标
	void SetLocalTransform(PxTransform trans)
	{
		localTransform = trans;
	}

	void SetTransform(float gameObjectPosition[3])
	{
		transform = { gameObjectPosition[0], gameObjectPosition[1], gameObjectPosition[2] };
		g_rigidBody->setGlobalPose({ gameObjectPosition[0], gameObjectPosition[1], gameObjectPosition[2] });
	}

	//绑定模型
	void AddModel(Model &model)
	{
		g_model = &model;
	}

	//添加刚体
	void AddRigidbody(bool isDynamic)
	{
		if (isDynamic)
		{
			g_rigidBody =gPhysics->createRigidDynamic(transform);
		}
		else
		{
			g_rigidBody = gPhysics->createRigidStatic(transform);
		}
		g_rigidBody->userData=this;
	};

	void AddRigidbody(PxRigidDynamic* body)
	{
		g_rigidBody = body;
	}

	//添加BoxCollider
	void AddBoxCollider(float halfX,float halfY,float halfZ,PxTransform localPos)
	{
		PxShape* shape = gPhysics->createShape(PxBoxGeometry(halfX, halfY,
			halfZ), *gMaterial);
		shape->setLocalPose(localPos);
		g_rigidBody->attachShape(*shape);
	}

	///添加SphereCollider
	void AddSphereCollider(float radius,PxTransform localPos)
	{
		PxShape* shape = gPhysics->createShape(PxSphereGeometry(radius), 
			*gMaterial);
		shape->setLocalPose(localPos);
		g_rigidBody->attachShape(*shape);
	}

	//添加capsule
	void AddCapsuleCollider(float radius,float halfHeigght,PxTransform localPos)
	{
		PxShape* shape = gPhysics->createShape(PxCapsuleGeometry(radius, halfHeigght),
			*gMaterial);
		shape->setLocalPose(localPos);
		g_rigidBody->attachShape(*shape);
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
#endif // !GameObject
