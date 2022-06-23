#pragma once
#include <string>
#include <foundation\PxTransform.h>
#include "../Utils/Mathf.h"
#include <PxPhysicsAPI.h>
#include <list>
#ifndef _MISSION_H
#define _MISSION_H

extern PxPhysics* gPhysics;
extern PxScene* gScene ;

enum MissionType
{
	FIND,
	KILL,
};


class Mission
{
public:
	Mission(PxVec3 StartPos, PxVec3 EndPos,std::string Description,int _ID, MissionType _Type);
	~Mission();
	void StartMission();
	void FinishMission();
	void CancelMission();
	void UpdateTrigger();

	bool State = false;
	bool IsActive = false;
	bool ChangeLock = false;
	std::string MissionDescription;
	int ID;
	PxRigidDynamic* StartTrigger = NULL;
	PxRigidDynamic* EndTrigger = NULL;
	double Timer = 0;
	MissionType Type;
};



Mission::Mission(PxVec3 StartPos, PxVec3 EndPos, std::string Description, int _ID, MissionType _Type)
{

	PxMaterial* gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.0f);
	PxShape* TriggerShape;
	StartTrigger = PxCreateDynamic(*gPhysics, PxTransform(StartPos),PxBoxGeometry(PxVec3(1, 1, 1)), *gMaterial, 1.0f);
	StartTrigger->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
	StartTrigger->getShapes(&TriggerShape, 1);
	TriggerShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
	TriggerShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
	gScene->addActor(*StartTrigger);


	EndTrigger = PxCreateDynamic(*gPhysics, PxTransform(EndPos), PxBoxGeometry(PxVec3(1, 1, 1)), *gMaterial, 1.0f);
	EndTrigger->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
	EndTrigger->getShapes(&TriggerShape, 1);
	TriggerShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
	TriggerShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);


	

	MissionDescription = Description;
	ID = _ID;
	Type = _Type;
	StartTrigger->userData = this;
	EndTrigger->userData = this;
	//cout << StartTrigger->userData<<"   " <<this<<endl;
}

Mission::~Mission()
{

}

inline void Mission::StartMission()
{

	if (IsActive == false&&State==false)
	{
		IsActive = true;
		ChangeLock = true;
		cout << "start mission" << endl;
	}
}
inline void Mission::FinishMission()
{
	if (IsActive == true && State == false)
	{
		cout << "finish mission" << endl;
		IsActive = false;
		State = true;
		ChangeLock = true;
	}

}

inline void Mission::CancelMission()
{
	if (IsActive == true && State == false)
	{
		cout << "cancel mission" << endl;
		IsActive = false;
		ChangeLock = true;
	}
}

inline void Mission::UpdateTrigger()
{
	if (ChangeLock)
	{
		if (!State)
		{
			if (!IsActive)
			{
				gScene->addActor(*StartTrigger);
				gScene->removeActor(*StartTrigger);

			}
			else
			{
				gScene->removeActor(*StartTrigger);
				gScene->addActor(*EndTrigger);
			}
		}
		else
		{
			gScene->removeActor(*EndTrigger);
		}
		ChangeLock = false;
	}
}


#endif // !_MISSION_H
