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

enum MissionType
{
	FIND,
	KILL,
};

//static Model S_Star= Model("../../assets/objects/mission/SM_Icon_Star_01.fbx");
//static Model S_Arrow = Model("../../assets/objects/mission/SM_Icon_Arrow_Small_01.fbx");
//static Model S_Exclamation = Model("../../assets/objects/mission/SM_Icon_Letter_Exclamation_01.fbx");

class Mission
{
public:
	Mission(PxVec3 StartPos, PxVec3 EndPos,std::string Description,int _ID, MissionType _Type);
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
	std::string MissionDescription;
	int ID;
	PxRigidDynamic* StartTrigger = NULL;
	PxRigidDynamic* EndTrigger = NULL;
	double Timer = 0;
	double TimeLimit = 10;
	MissionType Type;
	//Model Star = S_Star;
	//Model Arrow = S_Arrow;
	//Model Exclamation = S_Exclamation;

	
};





#endif // !_MISSION_H
