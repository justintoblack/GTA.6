#pragma once
#ifndef _GameObject_H
#define _GameObject_H
#include "../ModelLoading/model.h"
#include "../Render/Render.h"
using namespace physx;

extern	PxPhysics* gPhysics;
extern	PxMaterial* gMaterial;
extern	PxScene* gScene;

class Component;

class GameObject
{
private:
public:
	char Name[16]="gameObject";
	bool isStatic;
	PxTransform transform;
	PxTransform localTransform;
	
	GameObject* parent=nullptr;
	//PxVec3 scale;

	Model*  g_model=nullptr;
	PxRigidActor*  g_rigidBody=nullptr;

	//组件
	vector<Component*> components ;

	GameObject()
	{
		transform = PxTransform(0,0,0);
		//scale = PxVec3(1, 1, 1);
	}

	void SetName(const char name[])
	{
		strcpy(Name, name);
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
			isStatic = false;
		}
		else
		{
			g_rigidBody = gPhysics->createRigidStatic(transform);
			isStatic = true;
		}
		g_rigidBody->userData=this;
	};

	void AddRigidbody(PxRigidDynamic* body)
	{
		g_rigidBody = body;
		isStatic = false;
		g_rigidBody->userData = this;
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
		if (g_rigidBody != nullptr)
		{
			gScene->addActor(*g_rigidBody);
		}
	}

	void SetRigidbody(PxRigidActor* rigidBody)
	{
		rigidBody->userData = this;
		g_rigidBody = rigidBody;
	}

	void AddComponent(Component* component)
	{
		components.push_back(component);
	}

	//Component* GetComponent(Component* component)
	//{
	//	for (int i = 0; i < components.size(); i++)
	//	{
	//		if (typeid(components[i]) == typeid(component))
	//		{
	//			return components[i];
	//		}
	//	}
	//	return nullptr;
	//}
};


class Component
{
private:
public:
	GameObject* _parent;
	virtual void ShowParameter()
	{
	};
};

class RigidBody:public Component
{
private:
	
public:
	bool IsStatic;

	RigidBody(GameObject* parent)
	{
		_parent = parent;
		SetRigidBodyStatic();
		_parent->AddComponent(this);
	}

	void SetRigidBodyStatic()
	{
		_parent->g_rigidBody = gPhysics->createRigidStatic(_parent->transform);
	}
	void SetRigidBodyDynamic()
	{
		_parent->g_rigidBody = gPhysics->createRigidDynamic(_parent->transform);
	}
	void ShowParameter()
	{
		//ImGui::Spacing();
		//ImGui::Text("RigidBody:");
	}
};

class BoxCollider :public Component
{
private:
public:
	PxTransform Transform=PxTransform(0,0,0);
	PxVec3 Size=PxVec3(1,1,1);
	PxShape* BoxShape=nullptr;

	BoxCollider(GameObject* parent)
	{
		if (parent->g_rigidBody == nullptr)return;

		_parent = parent;
		BoxShape = gPhysics->createShape(PxBoxGeometry(Size.x, Size.y, Size.z),
			*gMaterial);
		BoxShape->setLocalPose(Transform);
		parent->g_rigidBody->attachShape(*BoxShape);

		_parent->AddComponent(this);
	}

	void SetShape()
	{
		_parent->g_rigidBody->detachShape(*BoxShape);
		PxBoxGeometry box(Size.x, Size.y, Size.z);
		BoxShape->setLocalPose(Transform);
		BoxShape->setGeometry(box);
		_parent->g_rigidBody->attachShape(*BoxShape);
	}
	void ShowParameter()
	{
		ImGui::Spacing();
		ImGui::Text("BoxCollider");
		ImGui::PushID(3);
		ImGui::Text("LocalTransform");
		ImGui::DragFloat("x", &Transform.p.x,0.1);
		ImGui::SameLine();
		ImGui::DragFloat("y", &Transform.p.y,0.1);
		ImGui::SameLine();
		ImGui::DragFloat("z", &Transform.p.z,0.1);
		ImGui::Text("BoxSize");
		ImGui::PopID();
		ImGui::PushID(4);
		ImGui::DragFloat("x", &Size.x,0.1);
		ImGui::SameLine();
		ImGui::DragFloat("y", &Size.y,0.1);
		ImGui::SameLine();
		ImGui::DragFloat("z", &Size.z,0.1);
		ImGui::PopID();

		SetShape();
	}
};

class ModelComponent :public Component
{
private:
public:
	Model* MyModel;
	ModelComponent(GameObject* parent)
	{
		_parent = parent;
		_parent->AddComponent(this);
	}

	void SetModel(Model &model)
	{
		MyModel = &model;
		_parent->g_model = MyModel;
	}

	void ShowParameter()
	{
		ImGui::Button("Modle");
	}
};
#endif // !GameObject
