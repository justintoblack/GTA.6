#pragma once

#ifndef _MISSIONMANAGER_H
#define _MISSIONMANAGER_H

#include"Mission.h"
#include<vector>




class MissionManager
{
public:
	vector<PxVec3> triggerPos = { PxVec3(-142, 1, -117), PxVec3(-103, 1,-117) ,PxVec3(-113, 1,92) , PxVec3(-110, 1,-58) ,PxVec3(-110, 1,-15) , PxVec3(-143, 1,-24)  ,PxVec3(-142, 1,-68) , PxVec3(-79, 1,-81) ,PxVec3(-72, 1,-29) ,
 PxVec3(-72, 1,-10) ,PxVec3(-72, 1,37) , PxVec3(-72, 1,76) ,PxVec3(-18, 1,78) , PxVec3(-18, 1,42) ,PxVec3(-7, 1,-8) , PxVec3(-7, 1,-60) ,PxVec3(-7, 1,-90) , PxVec3(-46, 1,-92) ,PxVec3(65, 1,-38) , PxVec3(27, 1,-23) ,PxVec3(36, 1,-6) , PxVec3(24, 1,41) ,PxVec3(24, 1,77) , PxVec3(97, 1,76) ,PxVec3(96, 1,-6) ,PxVec3(133, 1,-22) ,PxVec3(137, 1,73) };
	MissionManager();
	~MissionManager();
	void AddMission(PxVec3 StartPos, PxVec3 EndPos,MissionType _Type, std::string Description,double _reward);
	void AddTaxiMission(double _reward);
	void DeleteMission(int _ID);
	void UpdateAllMission(double deltatime);

	std::vector<Mission *> MissionList;
	int TotalNum=0;




private:

};






#endif // !_MISSIONMANAGER_H

