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

	vector<GameObject> SceneGameObject;		//�����е�GameObject
	vector<GameObject> SpecialGameObject;	//��Ҫ�������ɵ����
	vector<string> ModelPath;	//ģ��·��
	vector<string> ModelName;	//�ļ���
	//const char** models;

	Model  poleModel;
	Model  stationModel;
	Model  stationModel_01;
	Model  road;
	Model  _carBody;
	Model  _carWheelLeft;
	Model  _carWheelRight;

	//��ʼ��
	void Init(PxPhysics* physics,PxScene* gScene);

	//����λ�ñ�־С��(static)
	void CreateAnchorBall(PxTransform pos, PxMaterial* gMaterial,float radius);

	//����λ�ñ�־������(static)
	void CreateAnchorBox(PxTransform pos, PxMaterial* gMaterial, float halfX,float halfY,float halfZ);

	//������̬����(Dynamic)
	void CreateDynamicSphere(PxTransform pos, PxMaterial* gMaterial, float radius);

	//��������դ��
	void CreateBanister(PxVec3 pos,PxVec3 dir, PxMaterial* gMaterial, float halfHeight, 
		float halfLength, float density, float force, float torque,BreakType type);
	
	//����һ��դ��
	void CreateBanisters(PxVec3 pos, PxVec3 dir, PxMaterial* gMaterial,float separate, 
		physx::PxU32 num, float halfHeight, float halfLength,
		float density, float force, float torque);

	//�����������߸�
	void CreatePole(PxVec3 pos, PxMaterial* gMaterial, float halfXZ, float halfHeight,
		float density, float force, float torque);

	//����һ�ŵ��߸�
	void CreatePoles(PxVec3 pos, PxVec3 dir,float separate,PxU32 num, PxMaterial* gMaterial, float halfXZ, float halfHeight,
		float density, float force, float torque);

	//�������ٴ�
	void createSlowArea(PxVec3 startPosition, PxF32 capsuleRadii, PxF32 interval, PxU32 num, PxMaterial* gMaterial);

	//����GameObject
	void CreateGameObject();

	//UI����GameObject
	void CreateNewGameObject();

	//����GameObject
	void DuplicateGameObject(GameObject* gameObject);

	//��ȡ�ض���ʽ���ļ���
	void GetAllFormatFiles(string path, vector<string>& files, string format)
	{
		//�ļ����  
		intptr_t hFile = 0;
		//�ļ���Ϣ  
		struct _finddata_t fileinfo;
		string p;
		if ((hFile = _findfirst(p.assign(path).append("\\*" + format).c_str(), &fileinfo)) != -1)  //�и��ļ��ͽ���
		{
			do
			{
				if ((fileinfo.attrib & _A_SUBDIR)) // /*�Ƚ��ļ������Ƿ����ļ��� attrib��
				//�����������ļ������ԣ�__A_ARCH���浵����_A_HIDDEN�����أ���_A_NORMAL����������_A_RDONLY��ֻ������ _A_SUBDIR���ļ��У���_A_SYSTEM��ϵͳ����**/
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

