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



#include "Camera.h"
#include <ctype.h>
#include<iostream>
#include "foundation/PxMat33.h"
#include <PxPhysicsAPI.h>
#include "../Utils/Mathf.h"

using namespace physx;
using namespace std;

extern float deltaTime;

namespace Snippets
{

Camera::Camera(const PxVec3& eye, const PxVec3& dir)/*定义眼睛位置、视线方向*/
{
	mEye = eye;
	mDir = dir.getNormalized();
	mMouseX = 0;
	mMouseY = 0;
}

//target表示跟随的对象
void Camera::Update(physx::PxVec3 targetPos)
{
	float temp = (mEye - (targetPos + m_offset)).magnitude();

	if (temp > mMaxDistance)
	{
		temp = mMaxDistance-0.01f;
		mEye = targetPos + m_offset - mDir * temp;
		return;
	}
	else if(temp <mMinDistance)
	{
		temp = mMinDistance+0.01f;
		mEye = targetPos + m_offset - mDir * temp;
		return;
	}

	float curDistance = Mathf::Lerp(temp, mDistanceToTarget, deltaTime * mDamp);
	mDistanceToTarget = curDistance;
	mEye = targetPos + m_offset - mDir * curDistance;
}

void Camera::SetConfig(float dis,float minDis,float maxDis,physx::PxVec3 offset)
{
	mDistanceToTarget = dis;
	mMinDistance = minDis;
	mMaxDistance = maxDis;
	m_offset = offset;
}

void Camera::SetEye(physx::PxVec3 pos)
{
	mEye = pos;
}

void Camera::SetDir(physx::PxVec3 dir)
{
	mDir = dir.getNormalized();
}

physx::PxVec3 Camera::getOffset() const
{
	return m_offset;
}

void Camera::handleMouse(int button, int state, int x, int y)
{
	PX_UNUSED(state);
	PX_UNUSED(button);
	mMouseX = x;
	mMouseY = y;
}

//移动
void Camera::goFront(physx::PxVec2 arrow)
{
	PxVec3 distance=mDir* arrow.x + mDir.cross(PxVec3(0, 1, 0)) * arrow.y;
	mEye = mEye + distance *EditMoveSpeed*deltaTime;
}

	//控制前进后退
	bool Camera::handleKey(unsigned char key, int x, int y, float speed)
	{
		PX_UNUSED(x);
		PX_UNUSED(y);

	PxVec3 viewY = mDir.cross(PxVec3(0,1,0)).getNormalized();
	switch(toupper(key))
	{
	case 'W':	mEye += mDir*2.0f*speed;		break;
	case 'S':	mEye -= mDir*2.0f*speed;		break;
	case 'A':	mEye -= viewY*2.0f*speed;		break;
	case 'D':	mEye += viewY*2.0f*speed;		break;
	default:							return false;
	}
	return true;
}

void Camera::handleAnalogMove(float x, float y)
{
	PxVec3 viewY = mDir.cross(PxVec3(0,1,0)).getNormalized();
	mEye += mDir*y;
	mEye += viewY*x;
}

void Camera::handleMotion(int dx, int dy)
{
	//int dx = mMouseX - x;/*鼠标每次移动带来的视角变换*/
	//int dy = mMouseY - y;
	float sensitivity = 0.3;/*鼠标灵敏度[0,1]即可*/

	PxVec3 viewY = mDir.cross(PxVec3(0,1,0)).getNormalized();

	PxQuat qx(PxPi * dx*sensitivity / 180.0f, PxVec3(0, 1, 0));
	mDir = qx.rotate(mDir);
	PxQuat qy(PxPi * dy*sensitivity / 180.0f, viewY);
	mDir = qy.rotate(mDir);

	mDir.normalize();

	//mMouseX = x;
	//mMouseY = y;

}

PxTransform Camera::getTransform() const
{
	PxVec3 viewY = mDir.cross(PxVec3(0,1,0));

	if(viewY.normalize()<1e-6f) 
		return PxTransform(mEye);

	PxMat33 m(mDir.cross(viewY), viewY, -mDir);
	return PxTransform(mEye, PxQuat(m));
}

PxVec3 Camera::getEye() const
{ 
	return mEye; 
}

PxVec3 Camera::getDir() const
{ 
	return mDir; 
}


}

