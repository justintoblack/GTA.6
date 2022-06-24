#pragma once

#ifndef _THECREATOR_H
#define _THECREATOR_H
#include <foundation\PxTransform.h>
#include "../Utils/Mathf.h"
#include <PxPhysicsAPI.h>
#include<ctime>
#include "../SnippetVehicleCommon/SnippetVehicleFilterShader.h"
#include "../SnippetVehicleCommon/SnippetVehicleSceneQuery.h"
#include "../DemoTest/GameObject.h"
#include <list>
#include<io.h>


using namespace physx;

enum BreakType
{
	SomeInPiece,
	AllInPiece,
};

class TheCreator
{
private:
	PxPhysics* m_gPhysics;
	PxScene* m_gScene;
	int _modelsCount;

public:

	vector<GameObject> SceneGameObject;		//场景中的GameObject
	vector<GameObject> SpecialGameObject;	//需要代码生成的物件
	vector<string> ModelPath;	//模型路径
	vector<string> ModelName;	//文件名
	//const char** models;

	Model  poleModel;
	Model  stationModel;
	Model  stationModel_01;
	Model  road;
	Model  _carBody;
	Model  _carWheelLeft;
	Model  _carWheelRight;

	//初始化
	void Init(PxPhysics* physics,PxScene* gScene);

	//创建位置标志小球(static)
	void CreateAnchorBall(PxTransform pos, PxMaterial* gMaterial,float radius);

	//创建位置标志立方体(static)
	void CreateAnchorBox(PxTransform pos, PxMaterial* gMaterial, float halfX,float halfY,float halfZ);

	//创建动态球体(Dynamic)
	void CreateDynamicSphere(PxTransform pos, PxMaterial* gMaterial, float radius);

	//创建单个栅栏
	void CreateBanister(PxVec3 pos,PxVec3 dir, PxMaterial* gMaterial, float halfHeight, 
		float halfLength, float density, float force, float torque,BreakType type);
	
	//创建一排栅栏
	void CreateBanisters(PxVec3 pos, PxVec3 dir, PxMaterial* gMaterial,float separate, 
		physx::PxU32 num, float halfHeight, float halfLength,
		float density, float force, float torque);

	//创建单个电线杆
	void CreatePole(PxVec3 pos, PxMaterial* gMaterial, float halfXZ, float halfHeight,
		float density, float force, float torque);

	//创建一排电线杆
	void CreatePoles(PxVec3 pos, PxVec3 dir,float separate,PxU32 num, PxMaterial* gMaterial, float halfXZ, float halfHeight,
		float density, float force, float torque);

	//创建减速带
	void createSlowArea(PxVec3 startPosition, PxF32 capsuleRadii, PxF32 interval, PxU32 num, PxMaterial* gMaterial);

	//创建GameObject
	void CreateGameObject();

	//UI创建GameObject
	void CreateNewGameObject();

	//复制GameObject
	void DuplicateGameObject(GameObject* gameObject);

	//获取特定格式的文件名
	void GetAllFormatFiles(string path, vector<string>& files, string format)
	{
		//文件句柄  
		intptr_t hFile = 0;
		//文件信息  
		struct _finddata_t fileinfo;
		string p;
		if ((hFile = _findfirst(p.assign(path).append("\\*" + format).c_str(), &fileinfo)) != -1)  //有该文件就进入
		{
			do
			{
				if ((fileinfo.attrib & _A_SUBDIR)) // /*比较文件类型是否是文件夹 attrib，
				//就是所查找文件的属性：__A_ARCH（存档）、_A_HIDDEN（隐藏）、_A_NORMAL（正常）、_A_RDONLY（只读）、 _A_SUBDIR（文件夹）、_A_SYSTEM（系统）。**/
				{
					if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
					{
						//files.push_back(p.assign(path).append("\\").append(fileinfo.name) );
						GetAllFormatFiles(p.assign(path).append("\\").append(fileinfo.name), files, format);
					}
				}
				else
				{
					files.push_back(p.assign(path).append("/").append(fileinfo.name));
				}
			} while (_findnext(hFile, &fileinfo) == 0);

			_findclose(hFile);
		}
	}

};

#endif // !_THECREATOR_H

