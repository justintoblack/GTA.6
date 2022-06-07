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
#define RENDER_SNIPPET 1
#ifdef RENDER_SNIPPET

#include <vector>

#include "PxPhysicsAPI.h"

#include "../Render/Render.h"
#include "../Render/Camera.h"
#include<iostream>

using namespace physx;

extern void initPhysics(bool interactive);
extern void stepPhysics(bool interactive);	
extern void cleanupPhysics(bool interactive);
extern void keyPress(unsigned char key, const PxTransform& camera);

static __int64 gTime, gLastTime;
__int64 freq;
PxReal deltaTime;
extern PxController* m_player;

Snippets::Camera*	sCamera;

//鼠标
POINT p;
int lastX; int lastY;

bool needToPass=false;

namespace
{

void motionCallback(int x, int y)
{

	int dx=lastX - x;
	int dy= lastY - y;

	if (needToPass)
	{
		needToPass = false;
	}
	else
	{
		sCamera->handleMotion(dx, dy);
	}

	lastX = x;
	lastY = y;
	//std::cout << x << " " << GetSystemMetrics(SM_CXSCREEN) << std::endl;

	//到达窗口边界
	if (x <= 0 || x >= GetSystemMetrics(SM_CXSCREEN)-1 || y <= 0 || y >= GetSystemMetrics(SM_CYSCREEN)-1)
	{
		SetCursorPos(GetSystemMetrics(SM_CXSCREEN)/2, GetSystemMetrics(SM_CYSCREEN)/2);
		needToPass = true;
	}
}

void keyboardCallback(unsigned char key, int x, int y)
{
	if(key==27)
		exit(0);

	if(!sCamera->handleKey(key, x, y))
		keyPress(key, sCamera->getTransform());
}

void mouseCallback(int button, int state, int x, int y)
{
	sCamera->handleMouse(button, state, x, y);
}

void idleCallback()
{
	glutPostRedisplay();
}

//鼠标移动
void OnMouseMove(int x,int y)
{
	motionCallback(x, y);
}

//鼠标事件监听
void MouseEventCallBack()
{
	//获取当前鼠标位置
	GetCursorPos(&p);
	//未移动
	if (lastX == p.x && lastY == p.y)
	{

	}
	//移动
	else
	{
		std::cout << "移动" << std::endl;
		OnMouseMove(p.x,p.y);
		lastX = p.x;
		lastY = p.y;
	}
}

//显示窗口
void renderCallback()
{
	stepPhysics(true);


	//相机跟随
	PxVec3 pos = m_player->getPosition() - PxExtendedVec3(0, 0, 0);
	sCamera->Update(pos);

	Snippets::startRender(sCamera->getEye(), sCamera->getDir());

	PxScene* scene;
	PxGetPhysics().getScenes(&scene,1);

	//获取
	PxU32 nbActors = scene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC);
	if(nbActors)
	{
		std::vector<PxRigidActor*> actors(nbActors);
		scene->getActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC, reinterpret_cast<PxActor**>(&actors[0]), nbActors);
		Snippets::renderActors(&actors[0], static_cast<PxU32>(actors.size()), true);
	}

	Snippets::finishRender();

	QueryPerformanceCounter((LARGE_INTEGER*)&gTime); //get current count
	QueryPerformanceFrequency((LARGE_INTEGER*)&freq); //get processor freq
	deltaTime = (double)(gTime - gLastTime) / (double)freq;
	gLastTime = gTime;

	//std::cout << deltaTime;


//#pragma region ImGui
//	ImGui_ImplOpenGL3_NewFrame();
//	ImGui_ImplGlfw_NewFrame();
//#pragma endregion

	
}

void exitCallback(void)
{
	delete sCamera;
	cleanupPhysics(true);
}
}


//渲染循环
void renderLoop()
{
	sCamera = new Snippets::Camera(PxVec3(50.0f, 50.0f, 50.0f), PxVec3(-0.6f,-0.2f,-0.7f));
	sCamera->SetConfig(10,PxVec3(0,3,0));

	//初始化鼠标位置;
	GetCursorPos(&p);
	lastX = p.x;
	lastY = p.y;


	Snippets::setupDefaultWindow("PhysX Demo");
	Snippets::setupDefaultRenderState();

	glutIdleFunc(idleCallback);
	glutDisplayFunc(renderCallback);

	//键盘事件回调函数
	//glutKeyboardFunc(keyboardCallback);

	glutSetCursor(GLUT_CURSOR_NONE);

	//glutMouseFunc(mouseCallback);

	glutMotionFunc(motionCallback);
	glutPassiveMotionFunc(motionCallback);
	

	//motionCallback(0,0);

	atexit(exitCallback);

	initPhysics(true);
	glutMainLoop();

}
#endif
