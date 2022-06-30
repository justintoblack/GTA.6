#pragma once
#include <string>
#include <foundation\PxTransform.h>
#include "../Utils/Mathf.h"
#include <PxPhysicsAPI.h>
#include <list>
#include "../ModelLoading/model.h"
#ifndef _MISSION_H
#define _MISSION_H

extern PxPhysics* gPhysics;
extern PxScene* gScene ;
extern bool missionFinish, missionSuccess;

enum MissionType
{
	FIND,
	KILL,
};



class Mission
{
public:
	Mission(PxVec3 StartPos, PxVec3 EndPos,std::string Description,int _ID, MissionType _Type,double _reward);
	~Mission();
	void StartMission();
	void FinishMission();
	void CancelMission();
	void UpdateTrigger();
	void UpdateTimer(double deltatime);

	bool State = false;
	bool IsActive = false;
	bool ChangeLock = false;
	bool TimerLock = false;
	bool IsTracing = false;
	std::string MissionDescription;
	int ID;
	PxRigidDynamic* StartTrigger = NULL;
	PxRigidDynamic* EndTrigger = NULL;
	double Timer = 0;
	double TimeLimit =40;
	MissionType Type;
	double reward=0;

	
};





#endif // !_MISSION_H
