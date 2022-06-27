#pragma once
#ifndef _CARNPC_H
#define _CARNPC_H
#include "GameObject.h"

extern float deltaTime;

class CarNPC:public GameObject
{
private:
	int idx;
	float moveSpeed=1;
	float rotateSpeed=1;
	vector<PxTransform> TargetPoints;
public:
	CarNPC(Model& model,PxTransform pos,vector<PxTransform>& points,
		int idx)
	{
		this->SetName("carTest");
		ModelComponent* newModel = new ModelComponent(this);
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
		this->SetTransform(PxTransform(transform.p + PxVec3(0, 0, 1)*moveSpeed*deltaTime));
		//PxVec3 dir = TargetPoints[idx].p-transform.p;
		//if (dir.isZero())
		//{
		//	idx++;
		//	if (idx >= TargetPoints.size())
		//	{
		//		//idx = 0;
		//	}
		//	return;
		//}
		//dir = dir.getNormalized() * moveSpeed * deltaTime;
		//this->SetTransform(PxTransform(transform.p + dir));
		//cout << "move to "<< endl;
	}

};

#endif // !_CARNPC_H
