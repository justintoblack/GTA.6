//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  * Neither the name of NVIDIA CORPORATION nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
// OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Copyright (c) 2008-2018 NVIDIA Corporation. All rights reserved.
// Copyright (c) 2004-2008 AGEIA Technologies, Inc. All rights reserved.
// Copyright (c) 2001-2004 NovodeX AG. All rights reserved.  


// ****************************************************************************
// This snippet illustrates simple use of physx
//
// It creates a number of box stacks on a plane, and if rendering, allows the
// user to create new stacks and fire a ball from the camera position
// ****************************************************************************



#include <ctype.h>

#include "PxPhysicsAPI.h"

#include "../Common/Print.h"
#include "../Common/PVD.h"
#include "../Utils/Utils.h"
#include"CarGameObject.h"
#include<iostream>
#include<string>
#include<vector>
#include<windows.h>
#include<typeinfo>
#include "../InputSystem/InputSystem.h"
#include "../GameDemo/TheCreator.h"
#include "../Utils/Mathf.h"

#include"../DemoTest/CarGameObject.h"
#include <GL/glut.h>
#include<time.h>
#include <ctype.h>
#include<iostream>
#include "PxPhysicsAPI.h"
#include<Windows.h>
#include <vector>
#include "vehicle/PxVehicleUtil.h"
#include "../SnippetVehicleCommon/SnippetVehicleSceneQuery.h"
#include "../SnippetVehicleCommon/SnippetVehicleFilterShader.h"
#include "../SnippetVehicleCommon/SnippetVehicleTireFriction.h"
#include "../SnippetVehicleCommon/SnippetVehicleCreate.h"
#include "irrKlang/irrKlang.h"
#include"MissionManager.h"
//#include"../GameDemo/JsonData.h"



using namespace irrklang;
using namespace physx;
using namespace snippetvehicle;

//默认的内存管理和错误报告器
PxDefaultAllocator		gAllocator;
PxDefaultErrorCallback	gErrorCallback;

PxFoundation* gFoundation = NULL;
PxPhysics* gPhysics = NULL;

PxDefaultCpuDispatcher* gDispatcher = NULL;
PxScene* gScene = NULL;

PxMaterial* gMaterial = NULL;
PxCooking* gCooking = NULL;
PxPvd* gPvd = NULL;


#define NUM_VEHICLES 1
#define BLOCKING_SWEEPS 0
#define MAX_ACCELERATION 50.0f
//Contact modification values.
#define WHEEL_TANGENT_VELOCITY_MULTIPLIER 0.1f
#define MAX_IMPULSE PX_MAX_F32
//Angle thresholds used to categorize contacts as suspension contacts or rigid body contacts.
#define POINT_REJECT_ANGLE PxPi/4.0f
#define NORMAL_REJECT_ANGLE PxPi/4.0f


//Blocking sweeps require sweep hit buffers for just 1 hit per wheel.
//Non-blocking sweeps require more hits per wheel because they return all touches on the swept shape.
#if BLOCKING_SWEEPS
PxU16					gNbQueryHitsPerWheel = 1;
#else
PxU16					gNbQueryHitsPerWheel = 8;
#endif
//创建控制器管理员
PxControllerManager* manager = NULL;

PxReal stackZ = 10.0f;




std::vector<PxRigidDynamic*>			ballToDispear;
std::vector<PxRigidDynamic*>			ballBirdList;
std::vector<PxRigidDynamic*>			ballPigList;
PxRigidDynamic* ballBird;
PxRigidDynamic* ballPig;


const char* BirdName = "bird";
const char* PigName = "pig";


extern Snippets::Camera* sCamera;
extern bool main_window;
extern bool show_another_window;
extern bool inspector_window;
extern bool isSimulation;
extern bool soundEffect;
extern float volume1;

//输入
InputSyetem inputSystem;
CharacterActionMap characterMap;
VehicleActionMap vehicleMap;
EditActionMap editMap;

//造物者
TheCreator theCreator;

//任务管理者
MissionManager missionManager;

//相机跟随位置
PxVec3 characterPos;
PxVec3 vehiclePos;
PxVec3* CameraFollowTarget;

//GameObject
//GameObject testObject;
CarGameObject carObject;

///载具
VehicleSceneQueryData* gVehicleSceneQueryData = NULL;
PxBatchQuery* gBatchQuery = NULL;

PxVehicleDrivableSurfaceToTireFrictionPairs* gFrictionPairs = NULL;

PxRigidStatic* gGroundPlane = NULL;
PxVehicleDrive4W* gVehicle4W = NULL;
PxRigidDynamic* gTreasureActor = NULL;
bool					gIsVehicleInAir = true;
PxVec3 vehicleUp = PxVec3(0, 1, 0);
PxVec3 vehicleForward = PxVec3(0, 0, 1);


#pragma region 角色属性
PxController* m_player;

PxVec3 velocity = PxVec3(0, 0, 0);
PxVec3 gravity = PxVec3(0, -9.8f * 2.0f, 0);
PxVec3 moveDir;
PxVec3 characterForward;
PxVec3 characterRight;
PxVec3 fireOffset(0, 0, 1);

PxTransform checkSphereTrans;
float jumpHeight = 1.0f;
float characterRadius = 0.5f;
float characterHeight = 1.0f;
float checkSphereRadius = 0.1f;
float carRayDis = 1.0f;		//载具射线检测距离

PxRigidDynamic* DriveCarTrigger = NULL;
PxVec3 currentTraceTarge;
float curSpeed;
float walkSpeed = 3.0f;
float sprintSpeed = 7.0f;
 
bool isGrounded;
bool isAiming; 
bool isJumping; 
bool startJumping;
bool isRunning;
bool hasVehicleToDrive = false;
double totalMoney = 0;

///跳跃
void Jump()
{
	if (isGrounded)
	{
		isJumping = true;
		startJumping = true;
		velocity.y = PxSqrt(jumpHeight * gravity.y * -2);
	}
	
}

//左键
void FireFirst()
{
	//PxRaycastBuffer hit;
	//PxVec3 firePoint = m_player->getActor()->getGlobalPose().p;
	//if (gScene->raycast(firePoint, sCamera->getDir(), 100, hit))
	//{
	//	if (hit.block.actor->getType() == PxActorType::eRIGID_DYNAMIC)
	//	{
	//		float force = 100;
	//		((PxRigidDynamic*)hit.block.actor)->addForce(sCamera->getDir()*force,PxForceMode::eIMPULSE);
	//	}
	//}
}

void Fire()
{
}
//乘坐载具
void GetInVehicle()
{
	//乘坐载具
	if (hasVehicleToDrive)
	{
		m_player->setPosition(PxExtendedVec3(1000, 3, 1000));
		inputSystem.SetVehicleMap(vehicleMap);
		CameraFollowTarget = &vehiclePos;
		inputSystem.isVehicle = true;
		sCamera->SetConfig(4, 3, 6, PxVec3(0, 0, 0));
	}
}
//离开载具
void GetOutVehicle()
{
	PxTransform pos = gVehicle4W->getRigidDynamicActor()->getGlobalPose();
	PxVec3 p = pos.p + pos.q.getBasisVector0() * 2.0f;
	m_player->setPosition(PxExtendedVec3(p.x, p.y, p.z));
	inputSystem.SetCharacterMap(characterMap);
	CameraFollowTarget = &characterPos;
	inputSystem.isVehicle = false;
	sCamera->SetConfig(2.5f, 2.25f, 3.0f, PxVec3(0, 0.5, 0));
}
//冲刺
void Sprint(bool isSprint)
{
	isRunning = isSprint;
	if (isSprint)
	{
		curSpeed = sprintSpeed;
	}
	else
	{
		curSpeed = walkSpeed;
	}
}

void InitDriveCarTrigger()
{
	PxMaterial* gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.0f);
	PxShape* TriggerShape;
	DriveCarTrigger = PxCreateDynamic(*gPhysics, PxTransform(m_player->getActor()->getGlobalPose()), PxBoxGeometry(PxVec3(0.5, 1.1, 1.1)), *gMaterial, 1.0f);
	DriveCarTrigger->getShapes(&TriggerShape, 1);
	DriveCarTrigger->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
	TriggerShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
	TriggerShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
	gScene->addActor(*DriveCarTrigger);
}



#pragma endregion

//时间
extern float deltaTime;
extern float gameTime;
extern float timeSpeed;

#pragma region 全局按键事件
bool isInGameMode = true;
bool isQKeyDown;
bool is4KeyDown;
bool is6KeyDown;
bool is2KeyDown;
bool is8KeyDown;
bool is5KeyDown;
bool isAddKeyDown;
bool isReduceKeyDown;
bool isEscKeyDown;
bool isEnterKeyDown;

bool vehicleUseSpotLight = false;

bool leftKey;
bool rightKey;
bool downKey;
bool upKey;
bool midKey;
bool addKey;
bool reduceKey;
bool escKey;
bool enterKey;

extern bool isInBegin;
extern bool isEndBegin;
extern bool gotoSwitchMode;



void GlobalKeyEvent()
{
	/*if (GetAsyncKeyState('Q'))
	{
		if (!isQKeyDown)
		{
			isQKeyDown = true;
			if (inputSystem.isVehicle)
			{
				inputSystem.SetCharacterMap(characterMap);
				CameraFollowTarget = &characterPos;
				inputSystem.isVehicle = false;
			}
			else
			{
				inputSystem.SetVehicleMap(vehicleMap);
				CameraFollowTarget = &vehiclePos;
				inputSystem.isVehicle = true;
			}
		}
	}
	else
	{
		isQKeyDown = false;
	}*/
	if (GetAsyncKeyState(100) || GetAsyncKeyState(52))
	{
		if (!is4KeyDown)
		{
			is4KeyDown = true;

			//执行想要执行的动作，该动作只会被执行一次
			leftKey = true;
		}
		else
		{
			leftKey = false;
		}
	}
	else
	{
		is4KeyDown = false;
	}

	if (GetAsyncKeyState(102) || GetAsyncKeyState(54))
	{
		if (!is6KeyDown)
		{
			is6KeyDown = true;

			//执行想要执行的动作，该动作只会被执行一次
			rightKey = true;
		}
		else
		{
			rightKey = false;
		}
	}
	else
	{
		is6KeyDown = false;
	}

	if (GetAsyncKeyState(98) || GetAsyncKeyState(50))
	{
		if (!is2KeyDown)
		{
			is2KeyDown = true;

			//执行想要执行的动作，该动作只会被执行一次
			downKey = true;
		}
		else
		{
			downKey = false;
		}
	}
	else
	{
		is2KeyDown = false;
	}

	if (GetAsyncKeyState(104) || GetAsyncKeyState(56))
	{
		if (!is8KeyDown)
		{
			is8KeyDown = true;

			//执行想要执行的动作，该动作只会被执行一次
			upKey = true;
		}
		else
		{
			upKey = false;
		}
	}
	else
	{
		is8KeyDown = false;
	}

	if (GetAsyncKeyState(101) || GetAsyncKeyState(53))
	{
		if (!is5KeyDown)
		{
			is5KeyDown = true;

			//执行想要执行的动作，该动作只会被执行一次
			midKey = true;
		}
		else
		{
			midKey = false;
		}
	}
	else
	{
		is5KeyDown = false;
	}

	if (GetAsyncKeyState(107) || GetAsyncKeyState(57))
	{
		if (!isAddKeyDown)
		{
			isAddKeyDown = true;

			//执行想要执行的动作，该动作只会被执行一次
			addKey = true;
		}
		else
		{
			addKey = false;
		}
	}
	else
	{
		isAddKeyDown = false;
	}

	if (GetAsyncKeyState(109) || GetAsyncKeyState(48))
	{
		if (!isReduceKeyDown)
		{
			isReduceKeyDown = true;

			//执行想要执行的动作，该动作只会被执行一次
			reduceKey = true;
		}
		else
		{
			reduceKey = false;
		}
	}
	else
	{
		isReduceKeyDown = false;
	}

	if (GetAsyncKeyState(VK_ESCAPE))
	{
		if (!isEscKeyDown)
		{
			isEscKeyDown = true;

			//执行想要执行的动作，该动作只会被执行一次
			escKey = true;
		}
		else
		{
			escKey = false;
		}
	}
	else
	{
		isEscKeyDown = false;
	}

	if (GetAsyncKeyState(13))
	{
		if (!isEnterKeyDown)
		{
			isEnterKeyDown = true;

			//执行想要执行的动作，该动作只会被执行一次
			enterKey = true;
		}
		else
		{
			enterKey = false;
		}
	}
	else
	{
		isEnterKeyDown = false;
	}
}

//切换游戏模式
//戴灿贤在设计开始游戏的UI时对此进行了一些改动，你无需知道这些改动会带来什么
//你只需要知道在点击开始游戏前，切换游戏模式由UI状态机接管；
//点击进入游戏后，那些由你不理解的信号量所驱动的if语句块将永远不会执行，改动后的效果与改动前的效果没有任何区别。
void SwitchMode()
{
	isInGameMode = !isInGameMode;

	if (isInBegin == true) {
		isInGameMode = false;
		gotoSwitchMode = false;
	}
	if (isEndBegin == true) {
		isInGameMode = true;
	}
	if (isInGameMode)
	{
		main_window = false;
		show_another_window = false;
		inspector_window = false;
		isSimulation = true;
		inputSystem.SetCharacterMap(characterMap);
		glutSetCursor(GLUT_CURSOR_NONE);
	}
	else
	{
		if (isInBegin == true) {
			main_window = false;
			show_another_window = false;
			inspector_window = false;
		}
		else {
			main_window = true;
			show_another_window = true;
			inspector_window = true;
		}
		isSimulation = false;
		inputSystem.SetEditMap(editMap);
		glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
	}
};
#pragma endregion


struct FilterGroup
{
	enum Enum
	{
		eBIRD = (1 << 0),
		ePIG = (1 << 1),
		eMINE_LINK = (1 << 2),
		eCRAB = (1 << 3),
		eHEIGHTFIELD = (1 << 4),
	};
};

//触发器相关变量

//PxVec3 triggerPos = PxVec3(30, 1, 100);


//车辆相关的全局变量
//GameObject carObject;
//GameObject wheelFLObj;
//GameObject wheelFRObj;
//GameObject wheelBLObj;
//GameObject wheelBRObj;
//PxTransform wheelLF;
//PxTransform wheelRF;
//PxTransform wheelLB;
//PxTransform wheelRB;
//PxShape* wheels[5];

//PxRigidDynamic* carBody;


//VehicleSceneQueryData* gVehicleSceneQueryData = NULL;
//PxBatchQuery* gBatchQuery = NULL;
//
//PxVehicleDrivableSurfaceToTireFrictionPairs* gFrictionPairs = NULL;
//
//PxRigidStatic* gGroundPlane = NULL;
//PxVehicleDrive4W* gVehicle4W = NULL;
//bool					gIsVehicleInAir = true;
//PxVec3 vehicleUp = PxVec3(0, 1, 0);
//PxVec3 vehicleForward = PxVec3(0, 0, 1);

PxF32 gSteerVsForwardSpeedData[2 * 8] =
{
	0.0f,		0.75f,
	5.0f,		0.75f,
	30.0f,		0.125f,
	120.0f,		0.1f,
	PX_MAX_F32, PX_MAX_F32,
	PX_MAX_F32, PX_MAX_F32,
	PX_MAX_F32, PX_MAX_F32,
	PX_MAX_F32, PX_MAX_F32
};
PxFixedSizeLookupTable<8> gSteerVsForwardSpeedTable(gSteerVsForwardSpeedData, 4);

PxVehicleKeySmoothingData gKeySmoothingData =
{
	{
		6.0f,	//rise rate eANALOG_INPUT_ACCEL
		6.0f,	//rise rate eANALOG_INPUT_BRAKE		
		6.0f,	//rise rate eANALOG_INPUT_HANDBRAKE	
		2.5f,	//rise rate eANALOG_INPUT_STEER_LEFT
		2.5f,	//rise rate eANALOG_INPUT_STEER_RIGHT
	},
	{
		10.0f,	//fall rate eANALOG_INPUT_ACCEL
		10.0f,	//fall rate eANALOG_INPUT_BRAKE		
		10.0f,	//fall rate eANALOG_INPUT_HANDBRAKE	
		5.0f,	//fall rate eANALOG_INPUT_STEER_LEFT
		5.0f	//fall rate eANALOG_INPUT_STEER_RIGHT
	}
};

PxVehiclePadSmoothingData gPadSmoothingData =
{
	{
		6.0f,	//rise rate eANALOG_INPUT_ACCEL
		6.0f,	//rise rate eANALOG_INPUT_BRAKE		
		6.0f,	//rise rate eANALOG_INPUT_HANDBRAKE	
		2.5f,	//rise rate eANALOG_INPUT_STEER_LEFT
		2.5f,	//rise rate eANALOG_INPUT_STEER_RIGHT
	},
	{
		10.0f,	//fall rate eANALOG_INPUT_ACCEL
		10.0f,	//fall rate eANALOG_INPUT_BRAKE		
		10.0f,	//fall rate eANALOG_INPUT_HANDBRAKE	
		5.0f,	//fall rate eANALOG_INPUT_STEER_LEFT
		5.0f	//fall rate eANALOG_INPUT_STEER_RIGHT
	}
};

PxVehicleDrive4WRawInputData gVehicleInputData;

enum DriveMode
{
	eDRIVE_MODE_ACCEL_FORWARDS = 0,
	eDRIVE_MODE_ACCEL_REVERSE,
	eDRIVE_MODE_HARD_TURN_LEFT,
	eDRIVE_MODE_HANDBRAKE_TURN_LEFT,
	eDRIVE_MODE_HARD_TURN_RIGHT,
	eDRIVE_MODE_HANDBRAKE_TURN_RIGHT,
	eDRIVE_MODE_BRAKE,
	eDRIVE_MODE_NONE
};

DriveMode gDriveModeOrder[] =
{
	eDRIVE_MODE_BRAKE,
	eDRIVE_MODE_ACCEL_FORWARDS,
	eDRIVE_MODE_BRAKE,
	eDRIVE_MODE_ACCEL_REVERSE,
	eDRIVE_MODE_BRAKE,
	eDRIVE_MODE_HARD_TURN_LEFT,
	eDRIVE_MODE_BRAKE,
	eDRIVE_MODE_HARD_TURN_RIGHT,
	eDRIVE_MODE_ACCEL_FORWARDS,
	eDRIVE_MODE_HANDBRAKE_TURN_LEFT,
	eDRIVE_MODE_ACCEL_FORWARDS,
	eDRIVE_MODE_HANDBRAKE_TURN_RIGHT,
	eDRIVE_MODE_NONE
};


bool					gVehicleOrderComplete = false;
bool					gMimicKeyInputs = false;
PxTransform startTransform;





void setupFiltering(PxShape* shape, PxU32 filterGroup, PxU32 filterMask)
{
	PxFilterData filterData;
	filterData.word0 = filterGroup; // word0 = own ID
	filterData.word1 = filterMask;	// word1 = ID mask to filter pairs that trigger a contact callback;
	shape->setSimulationFilterData(filterData);
}


PxFilterFlags contactReportFilterShader(PxFilterObjectAttributes attributes0, PxFilterData filterData0,
	PxFilterObjectAttributes attributes1, PxFilterData filterData1,
	PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{
	//PX_UNUSED(attributes0);
	//PX_UNUSED(attributes1);
	//PX_UNUSED(filterData0);
	//PX_UNUSED(filterData1);
	//PX_UNUSED(constantBlockSize);
	//PX_UNUSED(constantBlock);

	//
	// Enable CCD for the pair, request contact reports for initial and CCD contacts.
	// Additionally, provide information per contact point and provide the actor
	// pose at the time of contact.
	//

	if (PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1))
	{
		pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
		return PxFilterFlag::eDEFAULT;
	}

	pairFlags = PxPairFlag::eCONTACT_DEFAULT
		| PxPairFlag::eNOTIFY_TOUCH_FOUND
		| PxPairFlag::eNOTIFY_CONTACT_POINTS
		| PxPairFlag::eCONTACT_EVENT_POSE;
	return PxFilterFlag::eDEFAULT;
}


//音乐
class MusicEvent
{
public:

	MusicEvent(char path[]) {
		ISoundEngine* PlayEngine = createIrrKlangDevice();
		ISoundSource* source = PlayEngine->addSoundSourceFromFile(path, ESM_AUTO_DETECT, true);
		if (source) {
			cout << "source is created!!!" << endl;
		}
		else {
			cout << "source is failed to be created!!!" << endl;
		}
		this->PlayEngine = PlayEngine;
		this->source = source;
		this->isPlay = false;
	}

	ISoundEngine* PlayEngine;
	ISoundSource* source;
	ISound* snd = nullptr;
	ISound* sndQuick = nullptr;
	bool isPlay;
	bool isDroped;
	float totalTime = 0;
	


	//bool soundEffect代表是否开启音效，float volume1代表音效的声音
	void play(bool soundEffect, float volume1)
	{
		if (soundEffect == true){
			if (isPlay == false) {
				if (source) {
					cout << "source is ready!!!" << endl;
				}
				else {
					cout << "source is failed to be found!!!" << endl;
				}
				snd = PlayEngine->play2D(source, true, false, true);
				if (snd) {
					cout << "snd is created!!!" << endl;
				}
				else {
					cout << "snd is failed to be created!!!" << endl;
				}
				this->snd = snd;
				this->isPlay = true;
			}
			else {
				if (totalTime < volume1) {
					totalTime += deltaTime * timeSpeed * volume1 / 3.0f;
					if (snd) {
						snd->setVolume(totalTime);
					}
					else
					{
						cout << "snd is not found!!!" << endl;
					}
				}
				else
				{
					if(snd){
						snd->setVolume(volume1);
					}
					else
					{
						cout << "snd is not found!!!" << endl;
					}
				}

			}
		}
	}

	void playQuick(bool soundEffect, float volume1) 
	{
		if (soundEffect == true && isPlay == false) {
			if (source) {
				cout << "source is ready!!!" << endl;
			}
			else {
				cout << "source is failed to be found!!!" << endl;
			}
			sndQuick = PlayEngine->play2D(source, true, false, true, true);
			if (sndQuick) {
				cout << "snd is created!!!" << endl;
			}
			else {
				cout << "snd is failed to be created!!!" << endl;
			}
			sndQuick->setVolume(volume1);
			//this->sndQuick = sndQuick;
			this->isPlay = true;
		}
	}

	void stop(float volume1)
	{
		if (isPlay == true) {
			if (totalTime > 0) {
				totalTime -= deltaTime * timeSpeed * volume1 / 3.0f;
				if (totalTime <= 0) {
					totalTime = 0.0f;
					if (snd) {
						snd->setVolume(0.0f);
						snd->setIsPaused(true);
						this->isDroped = snd->drop();
						cout << "snd is droped!!!" << endl;
						cout << "========================================" << endl;
					}
					else
					{
						cout << "snd is not found!!!" << endl;
					}
					this->isPlay = false;
				}
				if (totalTime > 0) {
					if (snd) {
						snd->setVolume(totalTime);
					}
					else
					{
						cout << "snd is not found!!!" << endl;
					}
				}
			}
		}
		
		
	}

	void stopQuick()
	{
		if (isPlay == true) {
			if (sndQuick) {
				sndQuick->setIsPaused(true);
				this->isDroped = this->sndQuick->drop();
				
				this->isPlay = false;
				
				cout << "snd is droped!!!" << endl;
				
				cout << "========================================" << endl;
			}
			else
			{
				cout << "snd is not found!!!" << endl;
			}
		}
		
	}

	~MusicEvent()
	{
		PlayEngine->drop();
	}
};

char carEnginePath[] = "../../assets/audio/carEngine.wav";
MusicEvent carEngine = MusicEvent(carEnginePath);
char bellPath[] = "../../assets/audio/bell1.wav";
MusicEvent bell = MusicEvent(bellPath);

class ContactReportCallback : public PxSimulationEventCallback
{
	void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) { PX_UNUSED(constraints); PX_UNUSED(count); }
	void onWake(PxActor** actors, PxU32 count) { PX_UNUSED(actors); PX_UNUSED(count); }
	void onSleep(PxActor** actors, PxU32 count) { PX_UNUSED(actors); PX_UNUSED(count); }
	void onTrigger(PxTriggerPair* pairs, PxU32 count) {
		for (PxU32 i = 0; i < count; i++)
		{
			// ignore pairs when shapes have been deleted
			if (pairs[i].flags & (PxTriggerPairFlag::eREMOVED_SHAPE_TRIGGER | PxTriggerPairFlag::eREMOVED_SHAPE_OTHER))
				continue;


			if ((pairs[i].otherActor == gVehicle4W->getRigidDynamicActor() && pairs[i].triggerActor == DriveCarTrigger))
			{
				if (pairs[i].status == PxPairFlag::eNOTIFY_TOUCH_FOUND)
					hasVehicleToDrive = true;
				else if (pairs[i].status == PxPairFlag::eNOTIFY_TOUCH_LOST)
					hasVehicleToDrive = false;

			}



			if ((pairs[i].otherActor == gVehicle4W->getRigidDynamicActor() && (Mission*)pairs[i].triggerActor->userData != NULL))
			{
				Mission* triggerMission = (Mission*)pairs[i].triggerActor->userData;

				if (triggerMission->IsTracing)
				{
					cout << triggerMission->MissionDescription << endl;
					if (pairs[i].triggerActor == triggerMission->StartTrigger)
					{
						currentTraceTarge = triggerMission->EndTrigger->getGlobalPose().p;
						triggerMission->StartMission();
						continue;
					}
					if (pairs[i].triggerActor == triggerMission->EndTrigger)
					{
						triggerMission->IsTracing = false;
						triggerMission->FinishMission();
						continue;
					}
				}

			}
		}
	}
	void onAdvance(const PxRigidBody* const*, const PxTransform*, const PxU32) {}
	void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs)
	{
		//std::vector<PxContactPairPoint> contactPoints;

		//PxTransform spherePose(PxIdentity);
		//PxU32 nextPairIndex = 0xffffffff;

		//PxContactPairExtraDataIterator iter(pairHeader.extraDataStream, pairHeader.extraDataStreamSize);
		//bool hasItemSet = iter.nextItemSet();
		//if (hasItemSet)
		//	nextPairIndex = iter.contactPairIndex;

		//for (PxU32 i = 0; i < nbPairs; i++)
		//{
		//	//
		//	// Get the pose of the dynamic object at time of impact.
		//	//
		//	if (nextPairIndex == i)
		//	{
		//		if (pairHeader.actors[0]->is<PxRigidDynamic>())
		//			spherePose = iter.eventPose->globalPose[0];
		//		else
		//			spherePose = iter.eventPose->globalPose[1];

		//		gContactSphereActorPositions.push_back(spherePose.p);

		//		hasItemSet = iter.nextItemSet();
		//		if (hasItemSet)
		//			nextPairIndex = iter.contactPairIndex;
		//	}

		//	//
		//	// Get the contact points for the pair.
		//	//
		//	const PxContactPair& cPair = pairs[i];
		//	if (cPair.events & (PxPairFlag::eNOTIFY_TOUCH_FOUND | PxPairFlag::eNOTIFY_TOUCH_CCD))
		//	{
		//		PxU32 contactCount = cPair.contactCount;
		//		contactPoints.resize(contactCount);
		//		cPair.extractContacts(&contactPoints[0], contactCount);

		//		for (PxU32 j = 0; j < contactCount; j++)
		//		{
		//			gContactPositions.push_back(contactPoints[j].position);
		//			gContactImpulses.push_back(contactPoints[j].impulse);
		//		}
		//	}
		//}

		//PxActorFlag
		//for (PxU32 i = 0; i < nbPairs; i++)
		//{
		//	const PxContactPair& cp = pairs[i];

		//	if (cp.events & PxPairFlag::eNOTIFY_TOUCH_FOUND)
		//	{
		//		if ((pairHeader.actors[0]->getName() == BirdName && pairHeader.actors[1]->getName() == PigName) || (pairHeader.actors[1]->getName() == BirdName && pairHeader.actors[0]->getName() == PigName))
		//		{
		//			PxActor* otherActor = (pairHeader.actors[0]->getName() == BirdName) ? pairHeader.actors[1] : pairHeader.actors[0];
		//			PxRigidDynamic* ballPig1 = (PxRigidDynamic*)(otherActor->userData);


		//			// insert only once
		//			if (std::find(ballToDispear.begin(), ballToDispear.end(), ballPig1) == ballToDispear.end())
		//				ballToDispear.push_back(ballPig1);

		//			std::vector<PxRigidDynamic*>::iterator ballIter = std::find(ballPigList.begin(), ballPigList.end(), ballPig1);
		//			if (ballIter != ballPigList.end())
		//			{
		//				//ballPigList.erase(ballIter);
		//				//gScene->removeActor(*ballPig1);
		//			}



		//			break;
		//		}
		//	}
		//}
	}
};

ContactReportCallback gContactReportCallback;



/*
PxBase->PxActor->PxRigidActor->PxRigidBody->PxRigidDynamic
*/
//创造动态刚体
PxRigidDynamic* createDynamic(PxReal radius, const PxTransform& t, const PxVec3& velocity = PxVec3(0))
{


	PxShape* shape = gPhysics->createShape(PxSphereGeometry(radius), *gMaterial);
	setupFiltering(shape, FilterGroup::eBIRD, FilterGroup::ePIG);


	//PxPhysics object，transform of the new object ，shape of the new object ，the density of the new object(>0)
	PxRigidDynamic* dynamic = gPhysics->createRigidDynamic(t);
	dynamic->attachShape(*shape);




	//设置角阻尼系数，还有线性阻尼linearDamping；线性阻尼控制物理形体或约束抵抗平移的量,而角阻尼控制其抵抗旋转的量。如果设置为0，物体会一直旋转/平移
	dynamic->setAngularDamping(10.0f);
	//设置线性速度 
	dynamic->setLinearVelocity(velocity);

	PxRigidBodyExt::updateMassAndInertia(*dynamic, 1.0f);
	//加入鸟队列
	ballBirdList.push_back(dynamic);

	dynamic->setName(BirdName);

	gScene->addActor(*dynamic);
	return dynamic;
}

//创建立方体堆
void createStack(const PxTransform& t, PxU32 size, PxReal halfExtent)
{
	/*
	PxShape 碰撞形状的抽象类;形状是共享的，引用计数的对象。
	可以通过调用PxRigidActor类的createShape()方法或PxPhysics类的createShape()方法来创建实例。
	Box,Sphere,Capsule,Plane,ConvexMesh,TriangleMesh,HeightField
	可以设置Material,offset,flags,name
	*/
	//createShape构建形状;(halfExtent x,y,z)
	PxShape* shape = gPhysics->createShape(PxBoxGeometry(halfExtent, halfExtent, halfExtent), *gMaterial);

	setupFiltering(shape, FilterGroup::ePIG, FilterGroup::eBIRD);


	for (PxU32 i = 0; i < size; i++)
	{
		for (PxU32 j = 0; j < size; j++)
		{
			//指定位置(-10/-7..9,1,0)(-7..,3,0)(-4..,5,0)...
			PxTransform localTm(PxVec3(PxReal(j * 2) - PxReal(size), PxReal(i * 2 + 1), 0) * halfExtent);
			//createRigidDynamic构建刚体
			PxRigidDynamic* body = gPhysics->createRigidDynamic(t.transform(localTm));
			//attachShape绑定形状到刚体上;
			body->attachShape(*shape);
			//更新质量和惯性（数值表示密度）
			PxRigidBodyExt::updateMassAndInertia(*body, 1.0f);

			body->setName(PigName);

			body->userData = body;

			//加入猪队列
			ballPigList.push_back(body);


			//把Actor添加到场景中,注释掉这一句之后所有立方体变得不可见且没有碰撞体积
			gScene->addActor(*body);
			//gScene->removeActor(*body);

		}
	}
	//释放
	shape->release();
}




//自定义

PxShape* shape;
PxRigidDynamic* dynamicbody;
PxRigidStatic* staticbody;
PxTransform tm(0, 0, 0);

bool isDriving;




///创建坐标轴
///
///
void CreateCoordinateAxis(PxTransform origin, float xLength, float yLength, float zLength)
{
	shape = gPhysics->createShape(PxSphereGeometry(2), *gMaterial);
	tm = origin;
	staticbody = PxCreateStatic(*gPhysics, tm, *shape);
	staticbody->attachShape(*shape);
	gScene->addActor(*staticbody);

	shape = gPhysics->createShape(PxBoxGeometry(xLength, 1, 1), *gMaterial);
	tm = PxTransform(PxVec3(xLength, 0, 0) + origin.p);
	staticbody = PxCreateStatic(*gPhysics, tm, *shape);
	staticbody->attachShape(*shape);
	gScene->addActor(*staticbody);

	shape = gPhysics->createShape(PxBoxGeometry(1, yLength, 1), *gMaterial);
	tm = PxTransform(PxVec3(0, yLength, 0) + origin.p);
	staticbody = PxCreateStatic(*gPhysics, tm, *shape);
	staticbody->attachShape(*shape);
	gScene->addActor(*staticbody);

	shape = gPhysics->createShape(PxBoxGeometry(1, 1, zLength), *gMaterial);
	tm = PxTransform(PxVec3(0, 0, zLength) + origin.p);
	staticbody = PxCreateStatic(*gPhysics, tm, *shape);
	staticbody->attachShape(*shape);
	gScene->addActor(*staticbody);
}

///生成链条
///
///
PxJoint* createLimitedSpherical(PxRigidActor* a0, const PxTransform& t0, PxRigidActor* a1, const PxTransform& t1)
{
	PxSphericalJoint* j = PxSphericalJointCreate(*gPhysics, a0, t0, a1, t1);
	j->setLimitCone(PxJointLimitCone(PxPi / 4, PxPi / 4, 0.05f));
	j->setSphericalJointFlag(PxSphericalJointFlag::eLIMIT_ENABLED, true);
	return j;
}
PxJoint* createBreakableFixed(PxRigidActor* a0, const PxTransform& t0, PxRigidActor* a1, const PxTransform& t1)
{
	PxFixedJoint* j = PxFixedJointCreate(*gPhysics, a0, t0, a1, t1);
	j->setBreakForce(1000, 100000);
	j->setConstraintFlag(PxConstraintFlag::eDRIVE_LIMITS_ARE_FORCES, true);
	j->setConstraintFlag(PxConstraintFlag::eDISABLE_PREPROCESSING, true);
	return j;
}
PxJoint* createDampedD6(PxRigidActor* a0, const PxTransform& t0, PxRigidActor* a1, const PxTransform& t1)
{
	PxD6Joint* j = PxD6JointCreate(*gPhysics, a0, t0, a1, t1);
	j->setMotion(PxD6Axis::eSWING1, PxD6Motion::eFREE);
	j->setMotion(PxD6Axis::eSWING2, PxD6Motion::eFREE);
	j->setMotion(PxD6Axis::eTWIST, PxD6Motion::eFREE);
	j->setDrive(PxD6Drive::eSLERP, PxD6JointDrive(0, 1000, FLT_MAX, true));
	return j;
}

typedef PxJoint* (*JointCreateFunction)(PxRigidActor* a0, const PxTransform& t0, PxRigidActor* a1, const PxTransform& t1);



void CreateChain(const PxTransform& t, PxU32 length, const PxGeometry& g, PxReal separation, JointCreateFunction createJoint)
{
	PxVec3 offset(separation / 2, 0, 0);
	PxTransform localTm(offset);
	PxRigidDynamic* prev = NULL;

	for (PxU32 i = 0; i < length; i++)
	{
		PxRigidDynamic* current = PxCreateDynamic(*gPhysics, t * localTm, g, *gMaterial, 1.0f);
		(*createJoint)(prev, prev ? PxTransform(offset) : t, current, PxTransform(-offset));
		gScene->addActor(*current);
		prev = current;
		localTm.p.x += separation;
	}
}



//创建角色控制器
PxController* CreateCharacterController(PxExtendedVec3 initPos)
{
	manager = PxCreateControllerManager(*gScene);

	PxCapsuleControllerDesc desc;


	desc.material = gMaterial;
	desc.position = initPos;
	desc.radius = characterRadius;
	desc.height = characterHeight;

	//坡度限制
	desc.slopeLimit = 0.7f;
	desc.contactOffset = 0.1f;
	desc.maxJumpHeight = 1.0f;
	desc.stepOffset = 0.3f;

	PxController* ctrl = manager->createController(desc);

	PxShape* haha;
	ctrl->getActor()->getShapes(&haha, 1);
	haha->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, false);

	return ctrl;
}

//车辆相关的函数
class WheelContactModifyCallback : public PxContactModifyCallback
{
public:

	WheelContactModifyCallback()
		: PxContactModifyCallback()
	{
	}

	~WheelContactModifyCallback() {}

	void onContactModify(PxContactModifyPair* const pairs, PxU32 count)
	{
		for (PxU32 i = 0; i < count; i++)
		{
			const PxRigidActor** actors = pairs[i].actor;
			const PxShape** shapes = pairs[i].shape;

			//Search for actors that represent vehicles and shapes that represent wheels.
			for (PxU32 j = 0; j < 2; j++)
			{
				const PxActor* actor = actors[j];
				if (actor->userData && (static_cast<ActorUserData*>(actor->userData))->vehicle)
				{
					const PxVehicleWheels* vehicle = (static_cast<ActorUserData*>(actor->userData))->vehicle;
					PX_ASSERT(vehicle->getRigidDynamicActor() == actors[j]);

					const PxShape* shape = shapes[j];
					if (shape->userData && (static_cast<ShapeUserData*>(shape->userData))->isWheel)
					{
						const PxU32 wheelId = (static_cast<ShapeUserData*>(shape->userData))->wheelId;
						PX_ASSERT(wheelId < vehicle->mWheelsSimData.getNbWheels());

						//Modify wheel contacts.
						PxVehicleModifyWheelContacts(*vehicle, wheelId, WHEEL_TANGENT_VELOCITY_MULTIPLIER, MAX_IMPULSE, pairs[i]);
					}
				}
			}
		}
	}

private:

};
WheelContactModifyCallback gWheelContactModifyCallback;
//The class WheelCCDContactModifyCallback identifies and modifies ccd contacts
//that involve a wheel.  Contacts that can be identified and managed by the suspension
//system are ignored.  Any contacts that remain are modified to account for the rotation
//speed of the wheel around the rolling axis.
class WheelCCDContactModifyCallback : public PxCCDContactModifyCallback
{
public:

	WheelCCDContactModifyCallback()
		: PxCCDContactModifyCallback()
	{
	}

	~WheelCCDContactModifyCallback() {}

	void onCCDContactModify(PxContactModifyPair* const pairs, PxU32 count)
	{
		for (PxU32 i = 0; i < count; i++)
		{
			const PxRigidActor** actors = pairs[i].actor;
			const PxShape** shapes = pairs[i].shape;

			//Search for actors that represent vehicles and shapes that represent wheels.
			for (PxU32 j = 0; j < 2; j++)
			{
				const PxActor* actor = actors[j];
				if (actor->userData && (static_cast<ActorUserData*>(actor->userData))->vehicle)
				{
					const PxVehicleWheels* vehicle = (static_cast<ActorUserData*>(actor->userData))->vehicle;
					PX_ASSERT(vehicle->getRigidDynamicActor() == actors[j]);

					const PxShape* shape = shapes[j];
					if (shape->userData && (static_cast<ShapeUserData*>(shape->userData))->isWheel)
					{
						const PxU32 wheelId = (static_cast<ShapeUserData*>(shape->userData))->wheelId;
						PX_ASSERT(wheelId < vehicle->mWheelsSimData.getNbWheels());

						//Modify wheel contacts.
						PxVehicleModifyWheelContacts(*vehicle, wheelId, WHEEL_TANGENT_VELOCITY_MULTIPLIER, MAX_IMPULSE, pairs[i]);
					}
				}
			}
		}
	}
};
WheelCCDContactModifyCallback gWheelCCDContactModifyCallback;


VehicleDesc initVehicleDesc()
{
	//Set up the chassis mass, dimensions, moment of inertia, and center of mass offset.
	//The moment of inertia is just the moment of inertia of a cuboid but modified for easier steering.
	//Center of mass offset is 0.65m above the base of the chassis and 0.25m towards the front.
	const PxF32 chassisMass = 1500.f;
	const PxVec3 chassisDims(2, 1, 5.4); //w,h,l
	const PxVec3 chassisMOI
	((chassisDims.y * chassisDims.y + chassisDims.z * chassisDims.z) * chassisMass / 12.0f,
		(chassisDims.x * chassisDims.x + chassisDims.z * chassisDims.z) * 0.8f * chassisMass / 12.0f,
		(chassisDims.x * chassisDims.x + chassisDims.y * chassisDims.y) * chassisMass / 12.0f);
	const PxVec3 chassisCMOffset(0.0f, -chassisDims.y * 0.5f + 0.65f, 0.25f);

	//Set up the wheel mass, radius, width, moment of inertia, and number of wheels.
	//Moment of inertia is just the moment of inertia of a cylinder.
	const PxF32 wheelMass = 200.0f;
	const PxF32 wheelRadius = 0.3f;
	const PxF32 wheelWidth = 0.3f;
	const PxF32 wheelMOI = 0.5f * wheelMass * wheelRadius * wheelRadius;
	const PxU32 nbWheels = 4;

	VehicleDesc vehicleDesc;

	vehicleDesc.chassisMass = chassisMass;
	vehicleDesc.chassisDims = chassisDims;
	vehicleDesc.chassisMOI = chassisMOI;
	vehicleDesc.chassisCMOffset = chassisCMOffset;
	vehicleDesc.chassisMaterial = gMaterial;
	vehicleDesc.chassisSimFilterData = PxFilterData(COLLISION_FLAG_CHASSIS, COLLISION_FLAG_CHASSIS_AGAINST, 0, 0);

	vehicleDesc.wheelMass = wheelMass;
	vehicleDesc.wheelRadius = wheelRadius;
	vehicleDesc.wheelWidth = wheelWidth;
	vehicleDesc.wheelMOI = wheelMOI;
	vehicleDesc.numWheels = nbWheels;
	vehicleDesc.wheelMaterial = gMaterial;
	vehicleDesc.chassisSimFilterData = PxFilterData(COLLISION_FLAG_WHEEL, COLLISION_FLAG_WHEEL_AGAINST, 0, 0);

	return vehicleDesc;
}

void startAccelerateForwardsMode()
{
	//play music
	carEngine.play(soundEffect, volume1);

	isDriving = true;


	if (gVehicle4W->mDriveDynData.getCurrentGear() == PxVehicleGearsData::eREVERSE)
	{
		gVehicle4W->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
	}

	if (gMimicKeyInputs)
	{
		gVehicleInputData.setDigitalAccel(true);
	}
	else
	{
		gVehicleInputData.setAnalogAccel(1.0f);
	}
}

void startAccelerateReverseMode()
{
	gVehicle4W->mDriveDynData.forceGearChange(PxVehicleGearsData::eREVERSE);

	if (gMimicKeyInputs)
	{
		gVehicleInputData.setDigitalAccel(true);
	}
	else
	{
		gVehicleInputData.setAnalogAccel(1.0f);
	}
}

void startBrakeMode()
{
	if (gMimicKeyInputs)
	{
		gVehicleInputData.setDigitalBrake(true);
	}
	else
	{
		gVehicleInputData.setAnalogBrake(1.0f);
	}
}

void startTurnHardLeftMode()
{
	if (gMimicKeyInputs)
	{
		gVehicleInputData.setDigitalAccel(true);
		gVehicleInputData.setDigitalSteerLeft(true);
	}
	else
	{
		//gVehicleInputData.setAnalogAccel(true);
		gVehicleInputData.setAnalogSteer(-0.5f);
	}
}

void startTurnHardRightMode()
{
	if (gMimicKeyInputs)
	{
		gVehicleInputData.setDigitalAccel(true);
		gVehicleInputData.setDigitalSteerRight(true);
	}
	else
	{
		//gVehicleInputData.setAnalogAccel(1.0f);
		gVehicleInputData.setAnalogSteer(0.5f);
	}
}

void startHandbrakeTurnLeftMode()
{
	if (gMimicKeyInputs)
	{
		gVehicleInputData.setDigitalSteerLeft(true);
		gVehicleInputData.setDigitalHandbrake(true);
	}
	else
	{
		gVehicleInputData.setAnalogSteer(-1.0f);
		gVehicleInputData.setAnalogHandbrake(1.0f);
	}
}

void startHandbrakeTurnRightMode()
{
	if (gMimicKeyInputs)
	{
		gVehicleInputData.setDigitalSteerRight(true);
		gVehicleInputData.setDigitalHandbrake(true);
	}
	else
	{
		gVehicleInputData.setAnalogSteer(1.0f);
		gVehicleInputData.setAnalogHandbrake(1.0f);
	}
}

void releaseAllControls()
{
	if (gMimicKeyInputs)
	{
		gVehicleInputData.setDigitalAccel(false);
		gVehicleInputData.setDigitalSteerLeft(false);
		gVehicleInputData.setDigitalSteerRight(false);
		gVehicleInputData.setDigitalBrake(false);
		gVehicleInputData.setDigitalHandbrake(false);
	}
	else
	{
		gVehicleInputData.setAnalogAccel(0.0f);
		gVehicleInputData.setAnalogSteer(0.0f);
		gVehicleInputData.setAnalogBrake(0.0f);
		gVehicleInputData.setAnalogHandbrake(0.0f);
	}
}

void resetVehicle()
{
	gVehicle4W->setToRestState();
	releaseAllControls();
	gVehicle4W->getRigidDynamicActor()->setGlobalPose(startTransform);
}

void stopEngine()
{
	carEngine.stop(volume1);

	isDriving = false;
};
void startBell()
{
	bell.playQuick(soundEffect, volume1);
};
void stopBell()
{
	bell.stopQuick();
};

void switchSpotlightStatus()
{
	vehicleUseSpotLight ^= true;
}

extern Model gBodyModel, gWheelModel_fl, gWheelModel_fr, gWheelModel_bl, gWheelModel_br;
PxRigidStatic* ballBody;

//自定义
void MyCode()
{
	//初始化造物者
	theCreator.Init(gPhysics, gScene);

	//CreateCoordinateAxis(PxTransform(0, 0, 0), 100, 200, 300);
	//CreateChain(PxTransform(PxVec3(0.0f, 20.0f, -10.0f)), 5, PxCapsuleGeometry(1.0f, 1.0f), 4.0f, createBreakableFixed);
	//CreateChain(PxTransform(PxVec3(0.0f, 25.0f, -20.0f)), 5, PxBoxGeometry(2.0f, 0.5f, 0.5f), 4.0f, createDampedD6);

	m_player = CreateCharacterController(PxExtendedVec3(25, 10, 25));
	PxRigidDynamic* playerActor = m_player->getActor();
	PxShape* playerShape;
	playerActor->getShapes(&playerShape, 1);
	gScene->addActor(*playerActor);
	InitDriveCarTrigger();
	//角色Input函数注册
	characterMap.SetActionMap(m_player, sCamera, 5.0f);
	characterMap.SpaceKeyEvent = Jump;
	characterMap.ShiftKeyEvent = Sprint;
	characterMap.FKeyEvent = GetInVehicle;
	characterMap.LeftButtonDownEvent = FireFirst;
	characterMap.LeftButtonEvent = Fire;

	//载具Input函数注册
	vehicleMap.release = releaseAllControls;
	vehicleMap.WKeyEvent = startAccelerateForwardsMode;
	vehicleMap.SKeyEvent = startAccelerateReverseMode;
	vehicleMap.AKeyEvent = startTurnHardRightMode;
	vehicleMap.DKeyEvent = startTurnHardLeftMode;
	vehicleMap.EKeyEvent = startBell;
	vehicleMap.FKeyEvent = GetOutVehicle;
	vehicleMap.RKeyEvent = resetVehicle;
	vehicleMap.VKeyEvent = switchSpotlightStatus;
	vehicleMap.ReleaseWKeyEvent = stopEngine;
	vehicleMap.ReleaseEKeyEvent = stopBell;

	inputSystem.SetCharacterMap(characterMap);
	CameraFollowTarget = &characterPos;

	//创建物体
	//theCreator.CreateBanister(PxVec3(-50, 0.0f, -50), PxVec3(1, 1, 1), gMaterial, 3, 5, 1, 100000, 50000);
	//theCreator.CreateBanisters(PxVec3(60, 0.0f, 20), PxVec3(1,0,1),gMaterial,1, 20, 0.5f, 1.0f, 10, 10000, 1000);
	//theCreator.CreateBanisters(PxVec3(50, 0.0f, 20), PxVec3(1,0,2),gMaterial, 1, 20, 0.5f, 1.0f, 10, 10000, 1000);
	//theCreator.CreateBanisters(PxVec3(10, 0.0f, 20), PxVec3(0,0,1),gMaterial, 1, 20, 0.5f, 1.0f, 10, 10000, 1000);
	//theCreator.CreateBanisters(PxVec3(10, 0.0f, 300), PxVec3(1,0,0),gMaterial, 1, 20, 0.5f, 1.0f, 10, 10000, 1000);
	//theCreator.CreatePoles(PxVec3(5, 0, 20), PxVec3(0,0,1),10,10, gMaterial, 0.15f, 2.5f, 10, 10000, 100);
	//theCreator.CreatePoles(PxVec3(55, 0, 20), PxVec3(0,0,1),50,10, gMaterial, 0.15f, 2.5f, 10, 10000, 100);
	//theCreator.createSlowArea(PxVec3(0, 0, 0), PxF32(0.01), PxF32(0.2), 30, gMaterial);
	//垃圾桶
	//theCreator.CreatePoles(PxVec3(50, 0.0f, 50), PxVec3(0, 0, 1), 20, 10, gMaterial, 0.3f, 0.7f, 10, 10000, 10000);

	theCreator.CreateGameObject();

	carObject.Name = "car";
	carObject.SetRigidbody(gVehicle4W->getRigidDynamicActor());
	carObject.AddModel(gBodyModel, gWheelModel_fl, gWheelModel_fr, gWheelModel_bl, gWheelModel_br);

	for (size_t i = 0; i < 100; i++)
	{
		missionManager.AddTaxiMission(100);
	}



}



//实例化物理 
void initPhysics(bool interactive)
{
	//PxFoundation(版本号,内存回调,错误回调)
	gFoundation = PxCreateFoundation(PX_FOUNDATION_VERSION, gAllocator, gErrorCallback);
	//PVD
	gPvd = PxCreatePvd(*gFoundation);
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
	gPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);
	//创建顶级PxPhysics对象
	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true, gPvd);

	//?缩放
	PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	//重力
	sceneDesc.gravity = PxVec3(0.0f, -9.8f, 0.0f);
	gDispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = gDispatcher;
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	sceneDesc.filterShader = contactReportFilterShader;
	sceneDesc.simulationEventCallback = &gContactReportCallback;
	gScene = gPhysics->createScene(sceneDesc);


	PxPvdSceneClient* pvdClient = gScene->getScenePvdClient();
	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}

	//静摩擦，动摩擦，restitution恢复原状(弹性)
	gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.0f);
	gCooking = PxCreateCooking(PX_PHYSICS_VERSION, *gFoundation, PxCookingParams(PxTolerancesScale()));

	PxRigidStatic* groundPlane = PxCreatePlane(*gPhysics, PxPlane(0, 1, 0, 0), *gMaterial);
	gScene->addActor(*groundPlane);


	/////////////////////////////////////////////创建车辆


	PxInitVehicleSDK(*gPhysics);
	PxVehicleSetBasisVectors(vehicleUp, vehicleForward);
	PxVehicleSetUpdateMode(PxVehicleUpdateMode::eVELOCITY_CHANGE);
	PxVehicleSetSweepHitRejectionAngles(POINT_REJECT_ANGLE, NORMAL_REJECT_ANGLE);
	PxVehicleSetMaxHitActorAcceleration(MAX_ACCELERATION);


	//Create the batched scene queries for the suspension sweeps.
//Use the post-filter shader to reject hit shapes that overlap the swept wheel at the start pose of the sweep.
	PxQueryHitType::Enum(*sceneQueryPreFilter)(PxFilterData, PxFilterData, const void*, PxU32, PxHitFlags&);
	PxQueryHitType::Enum(*sceneQueryPostFilter)(PxFilterData, PxFilterData, const void*, PxU32, const PxQueryHit&);
#if BLOCKING_SWEEPS
	sceneQueryPreFilter = &WheelSceneQueryPreFilterBlocking;
	sceneQueryPostFilter = &WheelSceneQueryPostFilterBlocking;
#else
	sceneQueryPreFilter = &WheelSceneQueryPreFilterNonBlocking;
	sceneQueryPostFilter = &WheelSceneQueryPostFilterNonBlocking;
#endif 
	gVehicleSceneQueryData = VehicleSceneQueryData::allocate(NUM_VEHICLES, PX_MAX_NB_WHEELS, gNbQueryHitsPerWheel, NUM_VEHICLES, sceneQueryPreFilter, sceneQueryPostFilter, gAllocator);
	gBatchQuery = VehicleSceneQueryData::setUpBatchedSceneQuery(0, *gVehicleSceneQueryData, gScene);


	//Create the friction table for each combination of tire and surface type.
	gFrictionPairs = createFrictionPairs(gMaterial);

	//Create a plane to drive on.
	PxFilterData groundPlaneSimFilterData(COLLISION_FLAG_GROUND, COLLISION_FLAG_GROUND_AGAINST, 0, 0);
	gGroundPlane = createDrivablePlane(groundPlaneSimFilterData, gMaterial, gPhysics);
	gScene->addActor(*gGroundPlane);

	//Create a vehicle that will drive on the plane.
	VehicleDesc vehicleDesc = initVehicleDesc();
	gVehicle4W = createVehicle4W(vehicleDesc, gPhysics, gCooking);
	startTransform = PxTransform(PxVec3(30, (vehicleDesc.chassisDims.y * 0.5f + vehicleDesc.wheelRadius + 1.0f), 30), PxQuat(PxIdentity));
	gVehicle4W->getRigidDynamicActor()->setGlobalPose(startTransform);
	gScene->addActor(*gVehicle4W->getRigidDynamicActor());

	//Set the vehicle to rest in first gear.
	//Set the vehicle to use auto-gears.
	gVehicle4W->setToRestState();
	gVehicle4W->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
	gVehicle4W->mDriveDynData.setUseAutoGears(true);


	startBrakeMode();


	/////////////////////////////////////////////
	///自定义
	MyCode();

	//if (不交互)，在render中把交互设成false就有一个默认的球滚过去撞击堆。
	if (!interactive)
		//PxSphereGeometry Transform,geometry,velocity（速度）
		createDynamic(10, PxTransform(PxVec3(0, 40, 100)), PxVec3(0, -50, -100));
}


//（在render中调用）
void stepPhysics(bool interactive)
{

	PX_UNUSED(interactive);
	//时间

	GlobalKeyEvent();
	inputSystem.InputAction();

	DriveCarTrigger->setGlobalPose(m_player->getActor()->getGlobalPose()); //更新上车触发器的位置
	const PxF32 timestep = deltaTime;
	if (gMimicKeyInputs)
	{
		PxVehicleDrive4WSmoothDigitalRawInputsAndSetAnalogInputs(gKeySmoothingData, gSteerVsForwardSpeedTable, gVehicleInputData, timestep, gIsVehicleInAir, *gVehicle4W);
	}
	else
	{
		PxVehicleDrive4WSmoothAnalogRawInputsAndSetAnalogInputs(gPadSmoothingData, gSteerVsForwardSpeedTable, gVehicleInputData, timestep, gIsVehicleInAir, *gVehicle4W);
	}

	//Suspension sweeps (instead of raycasts).
//Sweeps provide more information about the geometry under the wheel.
	PxVehicleWheels* vehicles[NUM_VEHICLES] = { gVehicle4W };
	PxSweepQueryResult* sweepResults = gVehicleSceneQueryData->getSweepQueryResultBuffer(0);
	const PxU32 sweepResultsSize = gVehicleSceneQueryData->getQueryResultBufferSize();
	PxVehicleSuspensionSweeps(gBatchQuery, NUM_VEHICLES, vehicles, sweepResultsSize, sweepResults, gNbQueryHitsPerWheel, NULL, 1.0f, 1.01f);

	//Vehicle update.
	const PxVec3 grav = gScene->getGravity();
	PxWheelQueryResult wheelQueryResults[PX_MAX_NB_WHEELS][NUM_VEHICLES];
	PxVehicleWheelQueryResult vehicleQueryResults[NUM_VEHICLES] =
	{
		{ wheelQueryResults[0], gVehicle4W->mWheelsSimData.getNbWheels() },
	};
	PxVehicleUpdates(timestep, grav, *gFrictionPairs, NUM_VEHICLES, vehicles, vehicleQueryResults);


	//Work out if the vehicle is in the air.
	gIsVehicleInAir = gVehicle4W->getRigidDynamicActor()->isSleeping() ? false : PxVehicleIsInAir(vehicleQueryResults[0]);


	///////////////////////////角色移动///////////////////////
	characterForward = PxVec3(sCamera->getDir().x, 0, sCamera->getDir().z);
	characterRight = characterForward.cross(PxVec3(0, 1, 0));
	moveDir = characterForward * characterMap.GetArrowKeyValue().x
		+ characterRight * characterMap.GetArrowKeyValue().y;
	moveDir = moveDir.getNormalized();

	PxOverlapHit hit;
	checkSphereTrans = PxTransform(m_player->getFootPosition() -
		PxExtendedVec3(0, 0, 0));
	isGrounded = PxSceneQueryExt::overlapAny(*gScene, PxSphereGeometry(checkSphereRadius),
		checkSphereTrans, hit);

	if (isGrounded && velocity.y < 0)
	{
		velocity.y = -0.0f;
		isJumping = false;
	}

	velocity += gravity * deltaTime;

	m_player->move(moveDir * curSpeed * deltaTime, 0.001f, 0.01f, NULL);
	m_player->move(velocity * deltaTime, 0.001f, 0.01f, NULL);

	////////////////////////////移动结束////////////////////////////////

	/////////////////////////////GameObject Update/////////////////////
	for (int i = 0; i < theCreator.SpecialGameObject.size(); i++)
	{
		theCreator.SpecialGameObject[i]->Update();
	}
	/////////////////////////////GameObject Update/////////////////////

	////////////////////////////////////////////////////////////
	//相机跟随
	characterPos = m_player->getPosition() - PxExtendedVec3(0, 0, 0);
	vehiclePos = gVehicle4W->getRigidDynamicActor()->getGlobalPose().p;

	if (isInGameMode)
	{
		sCamera->Update(*CameraFollowTarget);
		//相机碰撞检测
		if (!inputSystem.isVehicle)
		{
			PxRaycastHit hit[2];
			PxRaycastBuffer buffer(hit, 2);
			if (gScene->raycast(*CameraFollowTarget + sCamera->getOffset(),
				-sCamera->getDir(), sCamera->mDistanceToTarget, buffer))
			{
				if (buffer.nbTouches > 1)
				{
					PxVec3 newPos;
					if (buffer.getTouch(0).distance < 0.1)
					{
						newPos = buffer.getTouch(1).position;
					}
					else
					{
						newPos = buffer.getTouch(0).position;
					}
					sCamera->SetEye(newPos);
				}
			}
		}
	}
	else
	{
		sCamera->goFront(editMap.GetArrowKeyValue());
	}

	////////////////////////////Test////////////////////////////
	//PxTransform playerTrans = m_player->getActor()->getGlobalPose();
	//PxRaycastBuffer raycastHit;
	//if (gScene->raycast(playerTrans.p, sCamera->getDir().multiply(PxVec3(1,0,1)).getNormalized(), 
	//	carRayDis, raycastHit))
	//{
	//		if (raycastHit.block.actor == gVehicle4W->getRigidDynamicActor())
	//		{
	//			hasVehicleToDrive = true;
	//		}
	//		else
	//		{
	//			hasVehicleToDrive = false;
	//		}
	//}
	//else
	//{
	//	hasVehicleToDrive = false;
	//}

	/////////////////////////////物理模拟////////////////////////////
	if (isSimulation)
	{
		gScene->simulate(1.0f / 60.0f);
		gScene->fetchResults(true);
	}
	/////////////////////////////物理模拟////////////////////////////

	/////////////////////////////任务系统更新////////////////////////////////////

	missionManager.UpdateAllMission(deltaTime);

	/////////////////////////////任务系统更新////////////////////////////////////
}

//清空物理（在render中调用）
void cleanupPhysics(bool interactive)
{
	//release()销毁对象以及包含的所有对象
	PX_UNUSED(interactive);
	gScene->release();
	gDispatcher->release();
	gPhysics->release();
	PxPvdTransport* transport = gPvd->getTransport();
	gPvd->release();
	transport->release();

	gFoundation->release();

	printf("HelloWorld done.\n");
}


#define RENDER_SNIPPET 1
//main
int snippetMain(int, const char* const*)
{

#ifdef RENDER_SNIPPET
	extern void renderLoop();
	renderLoop();
#else
	static const PxU32 frameCount = 100;
	initPhysics(false);
	for (PxU32 i = 0; i < frameCount; i++)
		stepPhysics(false);
	cleanupPhysics(false);
#endif

	return 0;
}
