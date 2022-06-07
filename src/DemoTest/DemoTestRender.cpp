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

#include "Shader.h"
#include "../Render/Render.h"
#include "../Render/Camera.h"
#include<iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../InputSystem/InputSystem.h"

GLuint              gCubeTexture;
Shader				gSkyboxShader;
unsigned int		gSkyboxVAO, gSkyboxVBO;

//��պ������������ͼƬ
const char* gSkyboxFaces[6] = {
	"../../assets/SkyboxImages/right.jpg",
	"../../assets/SkyboxImages/left.jpg",
	"../../assets/SkyboxImages/top.jpg",
	"../../assets/SkyboxImages/bottom.jpg",
	"../../assets/SkyboxImages/front.jpg",
	"../../assets/SkyboxImages/back.jpg"
};

//��պ���������
const GLenum  CUBEMAP_DIRECTION[6] = {
	GL_TEXTURE_CUBE_MAP_POSITIVE_X,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
};

//��պ�������Ķ���
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
extern void keyPress(unsigned char key, const PxTransform& camera);

static __int64 gTime, gLastTime;
__int64 freq;
PxReal deltaTime;
extern PxController* m_player;
extern InputSyetem inputSystem;

Snippets::Camera*	sCamera;

//���
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

	//���ﴰ�ڱ߽�
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

//����ƶ�
void OnMouseMove(int x,int y)
{
	motionCallback(x, y);
}

//����¼�����
void MouseEventCallBack()
{
	//��ȡ��ǰ���λ��
	GetCursorPos(&p);
	//δ�ƶ�
	if (lastX == p.x && lastY == p.y)
	{

	}
	//�ƶ�
	else
	{
		std::cout << "�ƶ�" << std::endl;
		OnMouseMove(p.x,p.y);
		lastX = p.x;
		lastY = p.y;
	}
}

void RenderSkybox(void)
{
	glDepthMask(GL_FALSE);
	gSkyboxShader.use();

	glm::mat4 projectionMat = glm::perspective(45.0f, (float)glutGet(GLUT_WINDOW_WIDTH) / (float)glutGet(GLUT_WINDOW_HEIGHT), 0.1f, 1000.0f);

	PxVec3 cameraPos = sCamera->getEye();
	PxVec3 cameraDir = sCamera->getDir();
	PxVec3 cameraUp = PxVec3(0.0f, 1.0f, 0.0f);
	PxVec3 center = (cameraPos.operator+(cameraDir));
	glm::mat4 viewMat = glm::lookAt(glm::vec3(cameraPos.x, cameraPos.y, cameraPos.z),
		glm::vec3(center.x, center.y, center.z),
		glm::vec3(cameraUp.x, cameraUp.y, cameraUp.z));
	viewMat = glm::mat4(glm::mat3(viewMat));
	glUniformMatrix4fv(glGetUniformLocation(gSkyboxShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projectionMat));
	glUniformMatrix4fv(glGetUniformLocation(gSkyboxShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(viewMat));
	//glUniformMatrix4fv(glGetUniformLocation(skyBoxShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(modelMat));

	glBindVertexArray(gSkyboxVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, gCubeTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	/*©�����󣬽������Ϊ����С�򣬽������м��Բ���岻����*/
	glDeleteBuffers(1, &gSkyboxVBO);
	glDepthMask(GL_TRUE);
	glUseProgram(0);
}

//��ʾ����
void renderCallback()
{
	stepPhysics(true);

	Snippets::startRender(sCamera->getEye(), sCamera->getDir());
	RenderSkybox();
	PxScene* scene;
	PxGetPhysics().getScenes(&scene,1);

	//��ȡ
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


	
}

void exitCallback(void)
{
	delete sCamera;
	cleanupPhysics(true);
}

void SetupSkybox()
{
	//������Ȳ���
	glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LEQUAL);

	//����SHADER
	gSkyboxShader =
		Shader("../../src/Render/SkyBox.vs",
			"../../src/Render/SkyBox.fs");

	// ��պ� VAO

	glGenVertexArrays(1, &gSkyboxVAO);
	glGenBuffers(1, &gSkyboxVBO);
	glBindVertexArray(gSkyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, gSkyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(gSkyboxVertices), &gSkyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glBindVertexArray(0);

	int width, height, nrChannels;

	// ���ö���α����ϵĹ��ա���Ӱ����ɫ���㼰����
	glCullFace(GL_BACK);
	//GL_CCW ��ʾ����������ͶӰ����εĶ���˳��Ϊ��ʱ�뷽��ı���Ϊ����
	glFrontFace(GL_CCW);
	//������Ȳ���
	glEnable(GL_DEPTH_TEST);
	// �����������
	glGenTextures(1, &gCubeTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, gCubeTexture);

	// �����������ģʽ
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//����������Ⱦģʽ
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	//���������ڴ���뷽ʽ
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	//��������ͼƬ
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

	//����mip��ͼ
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
}

}


//��Ⱦѭ��
void renderLoop()
{
	sCamera = new Snippets::Camera(PxVec3(50.0f, 50.0f, 50.0f), PxVec3(-0.6f,-0.2f,-0.7f));
	sCamera->SetConfig(10,PxVec3(0,3,0));

	//��ʼ�����λ��;
	GetCursorPos(&p);
	lastX = p.x;
	lastY = p.y;


	Snippets::setupDefaultWindow("PhysX Demo");
	Snippets::setupDefaultRenderState();
	glewInit();
	SetupSkybox();


	glutIdleFunc(idleCallback);
	glutDisplayFunc(renderCallback);

	//�����¼��ص�����
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
