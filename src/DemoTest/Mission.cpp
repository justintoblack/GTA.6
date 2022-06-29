#include"Mission.h"

Mission::Mission(PxVec3 StartPos, PxVec3 EndPos, std::string Description, int _ID, MissionType _Type, double _reward)
{

	PxMaterial* gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.0f);
	PxShape* TriggerShape;
	StartTrigger = PxCreateDynamic(*gPhysics, PxTransform(StartPos), PxBoxGeometry(PxVec3(1, 1, 1)), *gMaterial, 1.0f);
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
	reward = _reward;
	//cout << StartTrigger->userData<<"   " <<this<<endl;
}

Mission::~Mission()
{

}

 void Mission::StartMission()
{

	if (IsActive == false && State == false)
	{
		IsActive = true;
		ChangeLock = true;
		TimerLock = true;
		cout << "start mission" << endl;
	}
}
 void Mission::FinishMission()
{
	if (IsActive == true && State == false)
	{
		cout << "finish mission" << endl;
		IsActive = false;
		State = true;
		TimerLock = false;
		ChangeLock = true;
		missionFinish = true;
		missionSuccess = true;
		IsTracing = false;

	}

}

 void Mission::CancelMission()
{
	if (IsActive == true && State == false)
	{
		cout << "cancel mission" << endl;
		IsActive = false;
		ChangeLock = true;
		TimerLock = false;
		Timer = 0;
		missionFinish = true;
		missionSuccess = false;
		IsTracing = false;
	}
}

 void Mission::UpdateTrigger()
{
	if (ChangeLock)
	{
		if (!State)
		{
			if (!IsActive)
			{
				gScene->addActor(*StartTrigger);
				gScene->removeActor(*EndTrigger);

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

 void Mission::UpdateTimer(double deltatime)
{
	if (TimerLock)
	{
		Timer += deltatime;
		if (Timer > TimeLimit&&Type==MissionType::FIND)
		{
			CancelMission();
		}
		//cout << MissionDescription << ": " << Timer<<endl;
	}
}
