#pragma once
#ifndef _MATHF_H
#define _MATHF_H

#include<iostream>
#include <foundation\PxMath.h>
#include <foundation\PxVec3.h>
#include <foundation\PxQuat.h>

using namespace std;
using namespace physx;

//ͬ���������Ϊ(0,0,0)

//��������ϵ
	class Mathf
	{
	private:
	public:
		static float DegToRad(float deg);
		static float RadToDeg(float rad);

		//����
		static PxVec3 QuatToEuler(PxQuat q)=delete;
		static PxQuat EulerToQuat(PxVec3 vec)=delete;

		static void Debug(PxVec3 vec);
		static void Debug(PxQuat q);

		static float Angle(PxVec3 from, PxVec3 to);
		static float Distance(PxVec3 vec);
	};



#endif // !_MATHF_H
