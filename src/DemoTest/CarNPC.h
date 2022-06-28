#pragma once
#ifndef _CARNPC_H
#define _CARNPC_H
#include "GameObject.h"

extern float deltaTime;

class CarNPC:public GameObject
{
private:
	int idx;
	float moveSpeed=5;
	float rotateFactor=2.5;
	vector<PxTransform> TargetPoints;
public:
	CarNPC(Model& model,PxTransform pos,vector<PxTransform>& points,
		int idx)
	{
		this->SetName("carTest");
		ModelComponent* newModel = new ModelComponent(this);
		RigidBody* rig = new RigidBody(this, false);
		BoxCollider* box = new BoxCollider(this);
		box->Size = PxVec3(0.9,0.9,2.4);
		box->SetShape();
		newModel->SetModel(model);
		this->SetTransform(pos);
		TargetPoints = points;
		this->idx = idx;
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
		PxVec3 dir = TargetPoints[idx].p-transform.p;
		if (dir.magnitudeSquared()<0.01f)
		{
			idx++;
			cout << "arrive";
			if (idx >= TargetPoints.size())
			{
				idx = 0;
			}
			dir = TargetPoints[idx].p - transform.p;
		}
		dir = dir.getNormalized() * moveSpeed * deltaTime;
		PxQuat q = Mathf::Slerp(transform.q, TargetPoints[idx].q, rotateFactor*deltaTime);
		this->SetTransform(PxTransform(transform.p + dir,q));
		//cout << "move to "<< endl;
	}

};

#endif // !_CARNPC_H
