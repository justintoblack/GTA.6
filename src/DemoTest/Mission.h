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



class Mission
{
public:
	Mission(PxVec3 StartPos,std::string Description,int _ID );
	~Mission();

	bool State = false;
	std::string MissionDescription;
	int ID;
	PxRigidDynamic* StartTrigger = NULL;

};



Mission::Mission(PxVec3 StartPos, std::string Description, int _ID)
{

	PxMaterial* gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.0f);
	StartTrigger = PxCreateDynamic(*gPhysics, PxTransform(StartPos),PxBoxGeometry(PxVec3(1, 1, 1)), *gMaterial, 1.0f);
	StartTrigger->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
	PxShape* TriggerShape;
	StartTrigger->getShapes(&TriggerShape, 1);
	TriggerShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
	TriggerShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
	gScene->addActor(*StartTrigger);
	MissionDescription = Description;
	ID = _ID;
	StartTrigger->userData = this;
	//cout << StartTrigger->userData<<"   " <<this<<endl;
}

Mission::~Mission()
{
}


#endif // !_MISSION_H
