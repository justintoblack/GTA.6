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
//
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

#include<iostream>

#include<vector>
#include<windows.h>

#include "../InputSystem/InputSystem.h"


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

using namespace physx;
using namespace snippetvehicle;
//默认的内存管理和错误报告器
PxDefaultAllocator		gAllocator;
PxDefaultErrorCallback	gErrorCallback;

PxFoundation*			gFoundation = NULL;
PxPhysics*				gPhysics	= NULL;

PxDefaultCpuDispatcher*	gDispatcher = NULL;
PxScene*				gScene		= NULL;

PxMaterial*				gMaterial	= NULL;
PxCooking* gCooking = NULL;
PxPvd*                  gPvd        = NULL;

//创建控制器管理员
PxControllerManager* manager = NULL;

PxReal stackZ = 10.0f;




std::vector<PxRigidDynamic*>			ballToDispear;
std::vector<PxRigidDynamic*>			ballBirdList;
std::vector<PxRigidDynamic*>			ballPigList;
PxRigidDynamic*                         ballBird;
PxRigidDynamic*                         ballPig;


const char* BirdName = "bird";
const char* PigName = "pig";


extern Snippets::Camera* sCamera;

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

//车辆相关的全局变量

VehicleSceneQueryData* gVehicleSceneQueryData = NULL;
PxBatchQuery* gBatchQuery = NULL;

PxVehicleDrivableSurfaceToTireFrictionPairs* gFrictionPairs = NULL;

PxRigidStatic* gGroundPlane = NULL;
PxVehicleDrive4W* gVehicle4W = NULL;

bool					gIsVehicleInAir = true;

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

PxF32					gVehicleModeLifetime = 4.0f;
PxF32					gVehicleModeTimer = 0.0f;
PxU32					gVehicleOrderProgress = 0;
bool					gVehicleOrderComplete = false;
bool					gMimicKeyInputs = false;





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
	PX_UNUSED(attributes0);
	PX_UNUSED(attributes1);
	PX_UNUSED(filterData0);
	PX_UNUSED(filterData1);
	PX_UNUSED(constantBlockSize);
	PX_UNUSED(constantBlock);

	//
	// Enable CCD for the pair, request contact reports for initial and CCD contacts.
	// Additionally, provide information per contact point and provide the actor
	// pose at the time of contact.
	//

	pairFlags = PxPairFlag::eCONTACT_DEFAULT
		| PxPairFlag::eDETECT_CCD_CONTACT
		| PxPairFlag::eNOTIFY_TOUCH_CCD
		| PxPairFlag::eNOTIFY_TOUCH_FOUND
		| PxPairFlag::eNOTIFY_CONTACT_POINTS
		| PxPairFlag::eCONTACT_EVENT_POSE;
	return PxFilterFlag::eDEFAULT;
}



class ContactReportCallback : public PxSimulationEventCallback
{
	void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) { PX_UNUSED(constraints); PX_UNUSED(count); }
	void onWake(PxActor** actors, PxU32 count) { PX_UNUSED(actors); PX_UNUSED(count); }
	void onSleep(PxActor** actors, PxU32 count) { PX_UNUSED(actors); PX_UNUSED(count); }
	void onTrigger(PxTriggerPair* pairs, PxU32 count) { PX_UNUSED(pairs); PX_UNUSED(count); }
	void onAdvance(const PxRigidBody*const*, const PxTransform*, const PxU32) {}
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
		for (PxU32 i = 0; i < nbPairs; i++)
		{
			const PxContactPair& cp = pairs[i];

			if (cp.events & PxPairFlag::eNOTIFY_TOUCH_FOUND)
			{
				if ((pairHeader.actors[0]->getName() == BirdName && pairHeader.actors[1]->getName() == PigName) || (pairHeader.actors[1]->getName() == BirdName && pairHeader.actors[0]->getName() == PigName))
				{
					PxActor* otherActor = (pairHeader.actors[0]->getName() == BirdName) ? pairHeader.actors[1] : pairHeader.actors[0];
					PxRigidDynamic* ballPig1 = (PxRigidDynamic*)(otherActor->userData);


					// insert only once
					if (std::find(ballToDispear.begin(), ballToDispear.end(), ballPig1) == ballToDispear.end())
						ballToDispear.push_back(ballPig1);

					std::vector<PxRigidDynamic*>::iterator ballIter = std::find(ballPigList.begin(), ballPigList.end(), ballPig1);
					if (ballIter != ballPigList.end())
					{
						//ballPigList.erase(ballIter);
						//gScene->removeActor(*ballPig1);
					}



					break;
				}
			}
		}
	}
};

ContactReportCallback gContactReportCallback;



/*
PxBase->PxActor->PxRigidActor->PxRigidBody->PxRigidDynamic
*/
//创造动态刚体
PxRigidDynamic* createDynamic( PxReal radius, const PxTransform& t, const PxVec3& velocity=PxVec3(0))
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


	for(PxU32 i=0; i<size;i++)
	{
		for(PxU32 j=0;j<size;j++)
		{
			//指定位置(-10/-7..9,1,0)(-7..,3,0)(-4..,5,0)...
			PxTransform localTm(PxVec3(PxReal(j*2) - PxReal(size), PxReal(i*2+1), 0) * halfExtent);
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
InputSyetem m_inputSystem;
CharacterActionMap characterMap;



///创建坐标轴
///
///
void CreateCoordinateAxis(PxTransform origin,float xLength,float yLength, float zLength)
{
	shape = gPhysics->createShape(PxSphereGeometry(2), *gMaterial);
	tm = origin;
	staticbody = PxCreateStatic(*gPhysics, tm, *shape);
	staticbody->attachShape(*shape);
	gScene->addActor(*staticbody);

	shape = gPhysics->createShape(PxBoxGeometry(xLength, 1, 1), *gMaterial);
	tm = PxTransform(xLength, 0, 0);
	staticbody = PxCreateStatic(*gPhysics, tm, *shape);
	staticbody->attachShape(*shape);
	gScene->addActor(*staticbody);

	shape = gPhysics->createShape(PxBoxGeometry(1, yLength, 1), *gMaterial);
	tm = PxTransform(0, yLength, 0);
	staticbody = PxCreateStatic(*gPhysics, tm, *shape);
	staticbody->attachShape(*shape);
	gScene->addActor(*staticbody);

	shape = gPhysics->createShape(PxBoxGeometry(1, 1, zLength), *gMaterial);
	tm = PxTransform(0, 0, zLength);
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

void CreateChain(const PxTransform& t,PxU32 length,const PxGeometry& g,PxReal separation,JointCreateFunction createJoint)
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

PxController* m_player;

//创建角色控制器
PxController* CreateCharacterController(PxExtendedVec3 initPos)
{
	manager = PxCreateControllerManager(*gScene);

	PxCapsuleControllerDesc desc;

	desc.material = gMaterial;
	desc.position = initPos;
	desc.radius = 2.0f;
	desc.height = 4.0f;

	//坡度限制
	desc.slopeLimit = 0.7f;
	desc.contactOffset = 0.1f;
	desc.maxJumpHeight = 1.0f;
	desc.stepOffset = 0.5f;

	PxController* ctrl = manager->createController(desc);

	return ctrl;

}

//车辆相关的函数
VehicleDesc initVehicleDesc()
{
	//Set up the chassis mass, dimensions, moment of inertia, and center of mass offset.
	//The moment of inertia is just the moment of inertia of a cuboid but modified for easier steering.
	//Center of mass offset is 0.65m above the base of the chassis and 0.25m towards the front.
	const PxF32 chassisMass = 1500.0f;
	const PxVec3 chassisDims(2.5f, 2.0f, 5.0f);
	const PxVec3 chassisMOI
	((chassisDims.y * chassisDims.y + chassisDims.z * chassisDims.z) * chassisMass / 12.0f,
		(chassisDims.x * chassisDims.x + chassisDims.z * chassisDims.z) * 0.8f * chassisMass / 12.0f,
		(chassisDims.x * chassisDims.x + chassisDims.y * chassisDims.y) * chassisMass / 12.0f);
	const PxVec3 chassisCMOffset(0.0f, -chassisDims.y * 0.5f + 0.65f, 0.25f);

	//Set up the wheel mass, radius, width, moment of inertia, and number of wheels.
	//Moment of inertia is just the moment of inertia of a cylinder.
	const PxF32 wheelMass = 20.0f;
	const PxF32 wheelRadius = 0.5f;
	const PxF32 wheelWidth = 0.4f;
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


//自定义
void MyCode()
{
	CreateCoordinateAxis(PxTransform(0,0,0),100,200,300);

	CreateChain(PxTransform(PxVec3(0.0f, 20.0f, -10.0f)), 5, PxCapsuleGeometry(1.0f,1.0f), 4.0f, createBreakableFixed);
	CreateChain(PxTransform(PxVec3(0.0f, 20.0f, -20.0f)), 5, PxBoxGeometry(2.0f, 0.5f, 0.5f), 4.0f, createDampedD6);

	m_player = CreateCharacterController(PxExtendedVec3(5,50,5));

	characterMap.SetController(m_player);
	characterMap.SetCamera(sCamera);
	characterMap.SetSpeed(0.1f);

}


//实例化物理
void initPhysics(bool interactive)
{
	//PxFoundation(版本号,内存回调,错误回调)
	gFoundation = PxCreateFoundation(PX_FOUNDATION_VERSION, gAllocator, gErrorCallback);
	//PVD
	gPvd = PxCreatePvd(*gFoundation);
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
	gPvd->connect(*transport,PxPvdInstrumentationFlag::eALL);
	//创建顶级PxPhysics对象
	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(),true,gPvd);

	//?缩放
	PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	//重力
	sceneDesc.gravity = PxVec3(0.0f, -9.8f, 0.0f);
	gDispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher	= gDispatcher;
	sceneDesc.filterShader	= PxDefaultSimulationFilterShader;
	sceneDesc.filterShader = contactReportFilterShader;
	sceneDesc.simulationEventCallback = &gContactReportCallback;
	gScene = gPhysics->createScene(sceneDesc);



	PxPvdSceneClient* pvdClient = gScene->getScenePvdClient();
	if(pvdClient)
	{
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}
	//静摩擦，动摩擦，restitution恢复原状(弹性)
	gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.0f);
	gCooking = PxCreateCooking(PX_PHYSICS_VERSION, *gFoundation, PxCookingParams(PxTolerancesScale()));

	PxRigidStatic* groundPlane = PxCreatePlane(*gPhysics, PxPlane(0,1,0,0), *gMaterial);
	gScene->addActor(*groundPlane);

	
	/////////////////////////////////////////////创建车辆


	PxInitVehicleSDK(*gPhysics);
	PxVehicleSetBasisVectors(PxVec3(0, 1, 0), PxVec3(0, 0, 1));
	PxVehicleSetUpdateMode(PxVehicleUpdateMode::eVELOCITY_CHANGE);

	//Create the batched scene queries for the suspension raycasts.
	gVehicleSceneQueryData = VehicleSceneQueryData::allocate(1, PX_MAX_NB_WHEELS, 1, 1, WheelSceneQueryPreFilterBlocking, NULL, gAllocator);
	gBatchQuery = VehicleSceneQueryData::setUpBatchedSceneQuery(0, *gVehicleSceneQueryData, gScene);

	//Create the friction table for each combination of tire and surface type.
	gFrictionPairs = createFrictionPairs(gMaterial);

	////Create a plane to drive on.
	//PxFilterData groundPlaneSimFilterData(COLLISION_FLAG_GROUND, COLLISION_FLAG_GROUND_AGAINST, 0, 0);
	//gGroundPlane = createDrivablePlane(groundPlaneSimFilterData, gMaterial, gPhysics);
	//gScene->addActor(*gGroundPlane);

	//Create a vehicle that will drive on the plane.
	VehicleDesc vehicleDesc = initVehicleDesc();
	gVehicle4W = createVehicle4W(vehicleDesc, gPhysics, gCooking);
	PxTransform startTransform(PxVec3(30, (vehicleDesc.chassisDims.y * 0.5f + vehicleDesc.wheelRadius + 1.0f), 30), PxQuat(PxIdentity));
	gVehicle4W->getRigidDynamicActor()->setGlobalPose(startTransform);
	gScene->addActor(*gVehicle4W->getRigidDynamicActor());

	//Set the vehicle to rest in first gear.
	//Set the vehicle to use auto-gears.
	gVehicle4W->setToRestState();
	gVehicle4W->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
	gVehicle4W->mDriveDynData.setUseAutoGears(true);

	gVehicleModeTimer = 0.0f;
	gVehicleOrderProgress = 0;
	startBrakeMode();

	/////////////////////////////////////////////
	///自定义
	MyCode();

	//if (不交互)，在render中把交互设成false就有一个默认的球滚过去撞击堆。
	if(!interactive)
		//PxSphereGeometry Transform,geometry,velocity（速度）
		createDynamic(10,PxTransform(PxVec3(0,40,100)), PxVec3(0,-50,-100));
}


//控制车辆运动
//void controlPress(unsigned char key)
//{
//	releaseAllControls();
//	if (GetAsyncKeyState(VK_UP))
//	{
//		startAccelerateForwardsMode();
//
//	}
//	if (GetAsyncKeyState(VK_DOWN))
//	{
//		startAccelerateReverseMode();
//	}
//	if (GetAsyncKeyState(VK_LEFT))
//	{
//		startTurnHardRightMode();
//	}
//	if (GetAsyncKeyState(VK_RIGHT))
//	{
//		startTurnHardLeftMode();
//	}
//
//}


//（在render中调用）
void stepPhysics(bool interactive)
{
	PX_UNUSED(interactive);
	//时间

	characterMap.InputAction();

	//PxVec3 moveDirection(0,0,0);
	//float speed=0.1f;
	//if (GetAsyncKeyState(0x41))
	//{
	//	moveDirection+= (PxVec3(-1, 0, 0));
	//}
	//if (GetAsyncKeyState(0x44))
	//{
	//	moveDirection += (PxVec3(1, 0, 0));
	//}
	//if (GetAsyncKeyState(0x53))
	//{
	//	moveDirection += (PxVec3(0, 0, 1));
	//}
	//if (GetAsyncKeyState(0x57))
	//{
	//	moveDirection += (PxVec3(0, 0, -1));
	//}


	//m_player->move(PxVec3(0, -0.05f, 0)+moveDirection.getNormalized()*speed, 0.01f, 0.01f, NULL);


	gScene->simulate(1.0f/60.0f);
	gScene->fetchResults(true);
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

//按键设置
void keyPress(unsigned char key, const PxTransform& camera)
{
	switch(toupper(key))
	{
	case 'B':	createStack(PxTransform(PxVec3(0,0,stackZ-=10.0f)), 10, 2.0f);						break;
	//PxSphereGeometry Transform,geometry,velocity（速度）
	case ' ':	createDynamic(2,camera,camera.rotate(PxVec3(0,0,-1))*200);	break;
	}
}

#define RENDER_SNIPPET 1
//main
int snippetMain(int, const char*const*)
{
#ifdef RENDER_SNIPPET
	extern void renderLoop();
	renderLoop();
#else
	static const PxU32 frameCount = 100;
	initPhysics(false);
	for(PxU32 i=0; i<frameCount; i++)
		stepPhysics(false);
	cleanupPhysics(false);
#endif

	return 0;
}
