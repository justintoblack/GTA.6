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
#define GLEW_STATIC

#include <vector>

#include <GL/glew.h>
#include "stb_image.h"
#include "PxPhysicsAPI.h"

//#include "Shader.h"
#include "../Render/Render.h"
#include "../Render/Camera.h"
#include<iostream>
#include "model.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../InputSystem/InputSystem.h"
#include "../Utils/Mathf.h"
#include "../Utils/Mathf.cpp"

#include "irrKlang/irrKlang.h"  //audio

using namespace irrklang;
ISoundEngine* BackgroundSoundEngine = createIrrKlangDevice();

//ISoundEngine* SoundEngine2 = createIrrKlangDevice();




GLuint              gCubeTexture;
Shader				gSkyboxShader;
unsigned int		gSkyboxVAO, gSkyboxVBO;
Model				gModel, gModel2;
Shader				gModelShader;

//天空盒六个面的纹理图片
const char* gSkyboxFaces[6] = {
	"../../assets/SkyboxImages/right.jpg",
	"../../assets/SkyboxImages/left.jpg",
	"../../assets/SkyboxImages/top.jpg",
	"../../assets/SkyboxImages/bottom.jpg",
	"../../assets/SkyboxImages/front.jpg",
	"../../assets/SkyboxImages/back.jpg"
};

//天空盒六个方向
const GLenum  CUBEMAP_DIRECTION[6] = {
	GL_TEXTURE_CUBE_MAP_POSITIVE_X,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
};

//天空盒六个面的顶点
float gSkyboxVertices[] = {
	//right
	1.0f, -1.0f, -1.0f,		1.0f, -1.0f, 1.0f,		1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,		1.0f,  1.0f, -1.0f,		1.0f, -1.0f, -1.0f,
	//left
	-1.0f, -1.0f,  1.0f,	-1.0f, -1.0f, -1.0f,	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,	-1.0f,  1.0f,  1.0f,	-1.0f, -1.0f,  1.0f,
	//top
	-1.0f,  1.0f, -1.0f,	1.0f,  1.0f, -1.0f,		1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,		-1.0f,  1.0f,  1.0f,	-1.0f,  1.0f, -1.0f,
	//bottom
	-1.0f, -1.0f,  1.0f,	1.0f, -1.0f,  1.0f,		1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,		-1.0f, -1.0f, -1.0f,	-1.0f, -1.0f,  1.0f,
	//back
	1.0f, -1.0f,  1.0f,		-1.0f, -1.0f,  1.0f,	-1.0f,  1.0f,  1.0f,
	-1.0f, 1.0f,  1.0f,		1.0f,  1.0f,  1.0f,		1.0f,  -1.0f,  1.0f,
	//front
	-1.0f, -1.0f, -1.0f,	1.0f, -1.0f, -1.0f,		1.0f,  1.0f, -1.0f,
	1.0f,  1.0f, -1.0f,		-1.0f,  1.0f, -1.0f,	-1.0f, -1.0f, -1.0f
};

using namespace physx;

extern void initPhysics(bool interactive);
extern void stepPhysics(bool interactive);	
extern void cleanupPhysics(bool interactive);
//extern void keyPress(unsigned char key, const PxTransform& camera);

//时间
float deltaTime;		//时间插值
float gameTime;		//当前程序执行时间
__int64 firstCount;
__int64 freq;
static __int64 gTime, gLastTime;

///////////////////////DemoTest///////////////////////////////
extern PxVec3 moveDir;
glm::vec3 forwardDir(0,0,1);
extern PxController* m_player;
extern InputSyetem inputSystem;

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

	//void keyboardCallback(unsigned char key, int x, int y)
	//{
	//	if(key==27)
	//		exit(0);
	//
	//	if(!sCamera->handleKey(key, x, y))
	//		//keyPress(key, sCamera->getTransform());
	//}

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

	glm::mat4 getViewMat() {
		PxVec3 cameraPos = sCamera->getEye();
		PxVec3 cameraDir = sCamera->getDir();
		PxVec3 cameraUp = PxVec3(0.0f, 1.0f, 0.0f);
		PxVec3 center = (cameraPos.operator+(cameraDir));
		glm::mat4 viewMat = glm::lookAt(glm::vec3(cameraPos.x, cameraPos.y, cameraPos.z),
			glm::vec3(center.x, center.y, center.z),
			glm::vec3(cameraUp.x, cameraUp.y, cameraUp.z));
		return viewMat;
	}

	void SetupSkybox()
	{
		//开启深度测试
		glEnable(GL_DEPTH_TEST);
		//glDepthFunc(GL_LEQUAL);

		//加载SHADER
		gSkyboxShader = Shader("../../src/Render/SkyBox.vs",
								"../../src/Render/SkyBox.fs");

		// 天空盒 VAO
		glGenVertexArrays(1, &gSkyboxVAO);
		glGenBuffers(1, &gSkyboxVBO);
		glBindVertexArray(gSkyboxVAO);
		glBindBuffer(GL_ARRAY_BUFFER, gSkyboxVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(gSkyboxVertices), &gSkyboxVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glBindVertexArray(0);

		int width, height, nrChannels;

		// 禁用多边形背面上的光照、阴影和颜色计算及操作
		glCullFace(GL_BACK);
		//GL_CCW 表示窗口坐标上投影多边形的顶点顺序为逆时针方向的表面为正面
		glFrontFace(GL_CCW);
		//开启深度测试
		glEnable(GL_DEPTH_TEST);
		// 创建纹理对象
		glGenTextures(1, &gCubeTexture);
		glBindTexture(GL_TEXTURE_CUBE_MAP, gCubeTexture);

		// 设置纹理过滤模式
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		//设置纹理渲染模式
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		//设置数据内存对齐方式
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		//加载纹理图片
		for (GLuint i = 0; i < 6; i++)
		{
			unsigned char* data = stbi_load(gSkyboxFaces[i], &width, &height, &nrChannels, 0);
			if (data)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB,
					width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			}
			else
			{
				std::cout << "Failed to load : " << gSkyboxFaces[i] << std::endl;
			}
			stbi_image_free(data);
		}

		//开启mip贴图
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	}
	void RenderSkybox(void)
	{
		glDepthMask(GL_FALSE);
		gSkyboxShader.use();

		glm::mat4 projectionMat = glm::perspective(45.0f, (float)glutGet(GLUT_WINDOW_WIDTH) / (float)glutGet(GLUT_WINDOW_HEIGHT), 0.1f, 1000.0f);
		glm::mat4 viewMat = getViewMat();
		viewMat = glm::mat4(glm::mat3(viewMat));
		glUniformMatrix4fv(glGetUniformLocation(gSkyboxShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projectionMat));
		glUniformMatrix4fv(glGetUniformLocation(gSkyboxShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(viewMat));
		//glUniformMatrix4fv(glGetUniformLocation(skyBoxShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(modelMat));

		glBindVertexArray(gSkyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, gCubeTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		/*漏掉这句后，胶囊体变为两颗小球，胶囊体中间的圆柱体不见了*/
		glDeleteBuffers(1, &gSkyboxVBO);
		glDepthMask(GL_TRUE);
		glUseProgram(0);
	}

	//传入模型对象model，以及模型的位置pos
	void RenderModel(Model& model, glm::vec3 pos, glm::vec3 dir , Shader& shader)
	{
		//glEnable(GL_DEPTH_TEST);
		model.setPos(pos);
		shader.use();
		glm::mat4 modelMat = glm::mat4(1.0f);
		modelMat = glm::translate(modelMat, model.getPos());

		//modelMat = glm::rotate(modelMat, 1.0f, glm::vec3(0, -1, 0));
		modelMat *= glm::mat4_cast(glm::quatLookAt(dir, glm::vec3(0, 1, 0)));

		modelMat = glm::scale(modelMat, glm::vec3(0.1f, 0.1f, 0.1f));
		
		


		glm::mat4 viewMat = getViewMat();
		glm::mat4 projectionMat = glm::perspective(45.0f, (float)glutGet(GLUT_WINDOW_WIDTH) / (float)glutGet(GLUT_WINDOW_HEIGHT), 0.1f, 1000.0f);
		glUniformMatrix4fv(glGetUniformLocation(gModelShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projectionMat));
		glUniformMatrix4fv(glGetUniformLocation(gModelShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(viewMat));
		glUniformMatrix4fv(glGetUniformLocation(gModelShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(modelMat));
		model.Draw(shader);

		glUseProgram(0);
	}

	//显示窗口
	void renderCallback()
	{
		//物理模拟
		stepPhysics(true);

		//渲染相机场景
		Snippets::startRender(sCamera->getEye(), sCamera->getDir(),0.1f);
		RenderSkybox();


		/////////////////////Test//////////////////////////


		float rotateSpeed = 10;
		//表示正在移动
		PxExtendedVec3 haha= m_player->getFootPosition();
		if (!moveDir.isZero())
		{
			glm::vec3 targetDir = glm::vec3(moveDir.x, 0, moveDir.z);
			forwardDir = glm::normalize( Mathf::Slerp(forwardDir, targetDir, deltaTime * rotateSpeed));
		}
			RenderModel(gModel, glm::vec3(haha.x, haha.y, haha.z),-forwardDir, gModelShader);
			//RenderModel(gModel, glm::vec3(-20.0f, 10.0f, -45.0f), gModelShader);
		//RenderModel(gModel2, glm::vec3(-28.0f, 15.0f, -47.0f), gModelShader);


		///////////////EndTest////////////////////////////




		PxScene* scene;
		PxGetPhysics().getScenes(&scene,1);

		//获取场景中的Actor并用OpenGL渲染
		PxU32 nbActors = scene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC);
		if(nbActors)
		{
			std::vector<PxRigidActor*> actors(nbActors);
			scene->getActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC, reinterpret_cast<PxActor**>(&actors[0]), nbActors);
			Snippets::renderActors(&actors[0], static_cast<PxU32>(actors.size()), true);
		}

		Snippets::finishRender();

		//关于时间
		QueryPerformanceCounter((LARGE_INTEGER*)&gTime); //get current count
		QueryPerformanceFrequency((LARGE_INTEGER*)&freq); //get processor freq
		deltaTime = (float)(gTime - gLastTime)/ (float)freq;
		gLastTime = gTime;
		gameTime = (float)(gTime - firstCount)/(float)pow(10,7);
		//std::cout  << deltaTime << std::endl;
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
		sCamera->SetConfig(4,PxVec3(0,0,0));

		//初始化鼠标位置;
		GetCursorPos(&p);
		lastX = p.x;
		lastY = p.y;


		Snippets::setupDefaultWindow("PhysX Demo");
		Snippets::setupDefaultRenderState();
		glewInit();


		//----------Render Model----------
		gModel = Model("../../assets/objects/nanosuit/nanosuit.obj");
		gModel2 = Model("../../assets/objects/backpack/backpack.obj");
		gModelShader = Shader("../../src/ModelLoading/model_loading.vs",
								"../../src/ModelLoading/model_loading.fs");
		//----------Render Model----------
		SetupSkybox();

		glutIdleFunc(idleCallback);
		//注册好回调函数后
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
		//BackgroundSoundEngine->play2D("../../assets/audio/owu12-u5eaj.wav", GL_TRUE);
		ISound* snd = BackgroundSoundEngine->play2D("../../assets/audio/owu12-u5eaj.wav", true, false, true);
		if (snd)
			snd->setVolume(0.4);
		//SoundEngine2->play2D("../../assets/audio/bell.wav", GL_TRUE);

		//记录游戏第一帧时间

		QueryPerformanceCounter((LARGE_INTEGER*)&gTime);
		QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
		gLastTime = gTime;
		firstCount = gTime;

		glutMainLoop();


	}
#endif
