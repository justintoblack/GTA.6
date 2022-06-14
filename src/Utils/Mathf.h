#pragma once
#ifndef _MATHF_H
#define _MATHF_H

#include<iostream>
#include <foundation\PxMath.h>
#include <foundation\PxVec3.h>
#include <foundation\PxQuat.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace physx;
//同向向量叉乘为(0,0,0)

/*四元数 
RotationAngle is in radians
x = RotationAxis.x * sin(RotationAngle / 2)
y = RotationAxis.y * sin(RotationAngle / 2)
z = RotationAxis.z * sin(RotationAngle / 2)
w = cos(RotationAngle / 2)*/


//右手坐标系
	class Mathf
	{
	private:
	public:
		static float DegToRad(float deg);
		static float RadToDeg(float rad);

		//废弃
		static PxVec3 QuatToEuler(PxQuat q)=delete;
		static PxQuat EulerToQuat(PxVec3 vec)=delete;

		static PxQuat VectorToQuat(PxVec3 vec)=delete;

		static void Debug(PxVec3 vec);
		static void Debug(PxQuat q);

		static float Angle(PxVec3 from, PxVec3 to);
		static float Distance(PxVec3 vec);

		static float Lerp(float a, float b, float t);
		static PxVec3 Lerp(PxVec3 a, PxVec3 b, float t);
		static PxQuat Lerp(PxQuat a, PxQuat b, float t);

		static glm::vec3 Slerp(glm::vec3 a, glm::vec3 b, float t);
		static PxVec3 Slerp(PxVec3 a, PxVec3 b, float t);
		static PxQuat Slerp(PxQuat a, PxQuat b, float t);
	};



#endif // !_MATHF_H
