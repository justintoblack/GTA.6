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
	void AddMission(PxVec3 StartPos, std::string Description);
	void DeleteMission(int _ID);


	std::vector<Mission *> MissionList;
	int TotalNum=0;




private:

};

MissionManager::MissionManager()
{
}

MissionManager::~MissionManager()
{
}

void MissionManager::AddMission(PxVec3 StartPos, std::string Description)
{
	MissionList.push_back(new Mission(StartPos, Description, TotalNum));
	TotalNum += 1;
}

void MissionManager::DeleteMission(int _ID)
{

	for (vector<Mission*>::iterator iter=MissionList.begin();iter!=MissionList.end();iter++)
	{
		if ((*iter)->ID== _ID)
		{
			MissionList.erase(iter);
			return;
		}
	}
}




#endif // !_MISSIONMANAGER_H

