#pragma once
#ifndef _CARNPC_H
#define _CARNPC_H
#include "GameObject.h"

extern float deltaTime;

class CarNPC:public GameObject
{
private:
	int idx;
	float moveSpeed=10;
	float rotateFactor=2.5;
	vector<PxTransform> TargetPoints;
	PxVec3 offset;
public:
	CarNPC(Model& model,PxTransform pos,vector<PxTransform>& points,
		int idx,PxVec3 off)
	{
		this->SetName("carTest");
		ModelComponent* newModel = new ModelComponent(this);
		RigidBody* rig = new RigidBody(this, false);
		BoxCollider* box = new BoxCollider(this);
		box->Size = PxVec3(0.9,0.9,2.4);
		box->SetShape();
		newModel->SetModel(model);
		this->SetTransform(PxTransform(pos.p+off,pos.q));
		TargetPoints = points;
		this->idx = idx;
		offset = off;
	}
	void Awake()
	{
		cout << "i was born!" << endl;
	}
	void Update() 
	{
		Move();
	}
	void Move()
	{
		//this->SetTransform(PxTransform(transform.p + PxVec3(0, 0, 1)*moveSpeed*deltaTime));
		PxVec3 dir = TargetPoints[idx].p-transform.p+offset;
		if (dir.magnitudeSquared()<0.1f)
		{
			idx++;
			if (idx >= TargetPoints.size())
			{
				idx = 0;
			}
			//if (TargetPoints[idx].q.getAngle() < 0.1f)
			//{
			//	this->SetTransform(PxTransform(transform.p, PxQuat(PxIdentity)));
			//}
			dir = TargetPoints[idx].p - transform.p+offset;
		}

		dir = dir.getNormalized() * moveSpeed * deltaTime;
		glm::quat qq=glm::quatLookAt(Mathf::P3ToV3(-dir), glm::vec3(0, 1, 0));
		PxQuat q = Mathf::ToPxQ(qq);
		q = Mathf::Slerp(transform.q, q, rotateFactor*deltaTime);
		this->SetTransform(PxTransform(transform.p + dir,q));
	}

};

#endif // !_CARNPC_H
