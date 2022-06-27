#pragma once
#ifndef _GameObject_H
#define _GameObject_H
#include "../ModelLoading/model.h"
#include "../Render/Render.h"
#include	"MonoBehaviour.h"
using namespace physx;

extern	PxPhysics* gPhysics;
extern	PxMaterial* gMaterial;
extern	PxScene* gScene;
extern const char** _allModelsName;
extern vector<Model> Models;			//模型
extern void makeObjectDrivable(PxShape*& shape);

class GameObject;
class ModelComponent;

class Component
{
private:
public:
	GameObject* _parent = nullptr;
	virtual void ShowParameter()
	{
		return;
	};
	Component& operator=(Component& obj)
	{
		return *this;
	}
};

class GameObject
{
private:
public:
	char Name[16]="gameObject";
	bool isStatic;
	PxTransform transform=PxTransform(0,0,0);
	PxTransform localTransform=PxTransform(0,0,0);
	
	GameObject* parent=nullptr;
	PxVec3 scale;

	Model*  g_model=nullptr;
	PxRigidActor*  g_rigidBody=nullptr;

	//组件
	vector<Component*> components;

	GameObject()
	{
		transform = PxTransform(0,0,0);
		scale = PxVec3(1, 1, 1);
		components.clear();
	}
	virtual void Awake() {};
	virtual void Update() {};
	//GameObject& operator=(const GameObject& obj)
	//{
	//	const char* newName = obj.Name;
	//	strcpy(Name, newName);
	//	transform = obj.transform;
	//	localTransform = obj.localTransform;
	//	parent = obj.parent;
	//	for (int i = 0; i < obj.components.size(); i++)
	//	{
	//		Component* newComponent=new Component();
	//		*newComponent = *obj.components[i];
	//		components.push_back(newComponent);
	//		//cout << i<<"OBj:  ";
	//		//cout << &obj.components[i] << endl;
	//		//cout << "this" << &*this << endl;
	//	}
	//	return *this;
	//}

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

	//查询是否存在Component
	bool hasComponent(string typeName)
	{
		string a = "class " + typeName;
		for (int i = 0; i < components.size(); i++)
		{
			if (typeid(*(components[i])).name() == a)
			{
				return true;
			}
		}
		return false;
	}

	//获取组件
	Component* GetComponent(string typeName)
	{
		string a = "class " + typeName;

		for (int i = 0; i < components.size(); i++)
		{
			if (typeid(*(components[i])).name() == a)
			{
				return components[i];
			}
		}
		return nullptr;
	}

	////删除GameObject
	//void Delete()
	//{
	//	cout << "Delete" << endl;
	//	delete this;
	//}
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
		IsStatic = true;
		_parent->AddComponent(this);
	}
	//RigidBody& operator=(RigidBody& obj)
	//{
	//	return *this;
	//}
	RigidBody(GameObject* parent, bool isStatic)
	{
		_parent = parent;
		_parent->AddComponent(this);
		if (isStatic)
		{
			SetRigidBodyStatic();
		}
		else
		{
			SetRigidBodyDynamic();
		}
	}

	void SetRigidBodyStatic()
	{
		_parent->g_rigidBody = gPhysics->createRigidStatic(_parent->transform);
		_parent->g_rigidBody->userData = _parent;
		gScene->addActor(*_parent->g_rigidBody);
		IsStatic = true;
	}
	void SetRigidBodyDynamic()
	{
		_parent->g_rigidBody = gPhysics->createRigidDynamic(_parent->transform);
		_parent->g_rigidBody->userData = _parent;
		gScene->addActor(*_parent->g_rigidBody);
		IsStatic = false;
	}
	void ShowParameter()
	{
		ImGui::Spacing();
		ImGui::Text("RigidBody:");
		ImGui::SameLine();
		if (IsStatic)
		{
			ImGui::Text("isStatic");
		}
		else
		{
			ImGui::Text("isDynamic");
		}
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

		//makeObjectDrivable(BoxShape);

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
		ImGui::DragFloat("x", &Transform.p.x,0.01);
		ImGui::SameLine();
		ImGui::DragFloat("y", &Transform.p.y,0.01);
		ImGui::SameLine();
		ImGui::DragFloat("z", &Transform.p.z,0.01);
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
	int item_current =0;
	Model* MyModel=nullptr;

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
		ImGui::Spacing();
		ImGui::Text("Model");
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 100);
		if (ImGui::Combo("Models", &item_current, _allModelsName,Models.size()))
		{
			SetModel(Models[item_current]);
		}
	}
};


#endif // !GameObject
