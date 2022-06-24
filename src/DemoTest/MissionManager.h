#pragma once

#ifndef _MISSIONMANAGER_H
#define _MISSIONMANAGER_H

#include"Mission.h"
#include<vector>
class MissionManager
{
public:
	MissionManager();
	~MissionManager();
	void AddMission(PxVec3 StartPos, PxVec3 EndPos,MissionType _Type, std::string Description);
	void DeleteMission(int _ID);
	void UpdateAllMission(double deltatime);

	std::vector<Mission *> MissionList;
	int TotalNum=0;




private:

};






#endif // !_MISSIONMANAGER_H

