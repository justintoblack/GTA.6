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

#include<iostream>
#include <vector>

#include <GL/glew.h>
#include "../Render/stb_image.h"
#include "PxPhysicsAPI.h"

#include "../Render/Render.h"
#include "../Render/Camera.h"
#include "../ModelLoading/model.h"
#include"../DemoTest/CarGameObject.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../InputSystem/InputSystem.h"
#include "../Utils/Mathf.h"
#include "../Utils/Mathf.cpp"
#include "../DemoTest/GameObject.h"

#include "irrKlang/irrKlang.h"  //audio
#include "../GameDemo/TheCreator.h"

using namespace irrklang;
using namespace physx;


GLuint              gCubeTexture;
Shader				gSkyboxShader;
unsigned int		gSkyboxVAO, gSkyboxVBO;
Model				gModel;
Model               gBodyModel, gWheelModel_fl, gWheelModel_fr, gWheelModel_bl, gWheelModel_br;
Shader				gModelShader;
glm::vec3			gLightPos = glm::vec3(10.0f, 50.0f, 50.0f);
glm::vec3			gLightDir = glm::vec3(2.0f, -3.0f, 1.0f);
glm::vec3			gLightAmbient = glm::vec3(0.6f, 0.6f, 0.6f);
glm::vec3			gLightDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
glm::vec3			gLightSpecular = glm::vec3(1.0f, 1.0f, 1.0f);

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

//extern GameObject carObject;
//extern GameObject wheelFLObj;
//extern GameObject wheelFRObj;
//extern GameObject wheelBLObj;
//extern GameObject wheelBRObj;

//extern GameObject testObject;
extern CarGameObject carObject;
extern TheCreator theCreator;
extern GameObject gameObject_00;

extern PxVec3 moveDir;
glm::vec3 forwardDir(0,0,1);
extern PxController* m_player;
extern InputSyetem inputSystem;
extern PxVehicleDrive4W* gVehicle4W;
Snippets::Camera*	sCamera;


/////////////////////////Imgui//////////////////////////////////

extern bool backgroundMusic;
extern float volume0;
bool isWireframe=false;


// /////////////////////////Imgui//////////////////////////////////
 



//鼠标
//POINT p;
//int lastX; int lastY;

//bool needToPass=false;

namespace 
{
	
	//void motionCallback(int x, int y)
	//{

	//	int dx=lastX - x;
	//	int dy= lastY - y;

	//	if (needToPass)
	//	{
	//		needToPass = false;
	//	}
	//	else
	//	{
	//		sCamera->handleMotion(dx, dy);
	//	}

	//	lastX = x;
	//	lastY = y;
	//	//std::cout << x << " " << GetSystemMetrics(SM_CXSCREEN) << std::endl;

	//	//到达窗口边界
	//	if (x <= 0 || x >= GetSystemMetrics(SM_CXSCREEN)-1 || y <= 0 || y >= GetSystemMetrics(SM_CYSCREEN)-1)
	//	{
	//		SetCursorPos(GetSystemMetrics(SM_CXSCREEN)/2, GetSystemMetrics(SM_CYSCREEN)/2);
	//		needToPass = true;
	//	}
	//}

	void keyboardCallback(unsigned char key, int x, int y)
	{
		if(key==27)
			exit(0);
	
		/*if(!sCamera->handleKey(key, x, y))
			keyPress(key, sCamera->getTransform());*/
	}

	void mouseCallback(int button, int state, int x, int y)
	{
		sCamera->handleMouse(button, state, x, y);
	}

	void idleCallback()
	{
		//必要的
		glutPostRedisplay();

		
	}

	void initImGUI()
	{
		//=======================
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

		// Setup Dear ImGui style
		//ImGui::StyleColorsDark();
		ImGui::StyleColorsClassic();

		// Setup Platform/Renderer backends
		// FIXME: Consider reworking this example to install our own GLUT funcs + forward calls ImGui_ImplGLUT_XXX ones, instead of using ImGui_ImplGLUT_InstallFuncs().
		ImGui_ImplGLUT_Init();
		ImGui_ImplGLUT_InstallFuncs();
		ImGui_ImplOpenGL2_Init();
		//============================
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
		//设置光源位置，光源的属性：环境光强度、漫反射强度、镜面反射强度
		shader.SetVector3f("lightPos", gLightPos);
		PxVec3 viewPos = sCamera->getEye();
		shader.SetVector3f("viewPos", viewPos.x, viewPos.y, viewPos.z);
		shader.SetVector3f("light.direction", gLightDir);
		shader.SetVector3f("light.ambient", gLightAmbient);
		shader.SetVector3f("light.diffuse", gLightDiffuse);
		shader.SetVector3f("light.specular", gLightSpecular);
		//shininess发光值，发光值越高，反射能力越强，散射越少，高光点越小
		shader.SetFloat("material.shininess", 1024.0f);

		glm::mat4 modelMat = glm::mat4(1.0f);
		modelMat = glm::translate(modelMat, model.getPos());
		//modelMat = glm::rotate(modelMat, 1.0f, glm::vec3(0, -1, 0));
		modelMat *= glm::mat4_cast(glm::quatLookAt(dir, glm::vec3(0, 1, 0)));
		modelMat = glm::scale(modelMat, glm::vec3(.1f, .1f, .1f));
		glm::mat4 viewMat = getViewMat();
		glm::mat4 projectionMat = glm::perspective(45.0f, (float)glutGet(GLUT_WINDOW_WIDTH) / (float)glutGet(GLUT_WINDOW_HEIGHT), 0.1f, 1000.0f);
		shader.SetMatrix4fv("projection", projectionMat);
		shader.SetMatrix4fv("view", viewMat);
		shader.SetMatrix4fv("model", modelMat);
		

		model.Draw(shader);

		glUseProgram(0);
	}



	//渲染GameObject
	void RenderGameObject(GameObject &gameObject)
	{
		//是否有父物体
		if (gameObject.parent != nullptr)
		{
			gameObject.transform = gameObject.parent->transform.transform(gameObject.localTransform);
			gameObject.transform.q= gameObject.parent->transform.q;
			gameObject.transform.q *=gameObject.localTransform.q;
			//gameObject.transform = gameObject.parent->transform.transform
			//(gameObject.localTransform);

			//gameObject.transform.q *= gameObject.localTransform.q;
		}
		//需要跟踪物理模拟
		else if (gameObject.g_rigidBody&&gameObject.g_rigidBody->getType()==
			PxActorType::eRIGID_DYNAMIC)
		{
			gameObject.transform = gameObject.g_rigidBody->getGlobalPose();
		}

		if (gameObject.hasComponent("ModelComponent"))
		{
			ModelComponent* mod = (ModelComponent*)gameObject.GetComponent("ModelComponent");
			if (mod->MyModel == nullptr)
			{
				return;
			}
			gModelShader.use();
			glm::mat4 modelMat = glm::mat4(1.0f);
			modelMat = glm::translate(modelMat, Mathf::P3ToV3(gameObject.transform.p));
			modelMat *= glm::mat4_cast(Mathf::Toquat(gameObject.transform.q));
			//modelMat = glm::scale(modelMat, gameObject.g_model->getScale());
			glm::mat4 viewMat = getViewMat();
			glm::mat4 projectionMat = glm::perspective(45.0f, (float)glutGet(GLUT_WINDOW_WIDTH) / (float)glutGet(GLUT_WINDOW_HEIGHT), 0.1f, 1000.0f);
			glUniformMatrix4fv(glGetUniformLocation(gModelShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projectionMat));
			glUniformMatrix4fv(glGetUniformLocation(gModelShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(viewMat));
			glUniformMatrix4fv(glGetUniformLocation(gModelShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(modelMat));

			mod->MyModel->Draw(gModelShader);

			glUseProgram(0);
		}
	}

	//渲染车辆
	void RenderCarObject(CarGameObject& gameObject)
	{
		//需要跟踪物理模拟
		if (gameObject.g_rigidBody && gameObject.g_rigidBody->getType() ==
			PxActorType::eRIGID_DYNAMIC)
		{
			gameObject.transform = gameObject.g_rigidBody->getGlobalPose();
		}

		PxShape* vehicleshapes[5];  //4个车轮，1个车体的shape
		gVehicle4W->getRigidDynamicActor()->getShapes(vehicleshapes, 5);


		//渲染车体
		gModelShader.use();
		glm::mat4 modelMat = glm::mat4(1.0f);
		modelMat = glm::translate(modelMat, Mathf::P3ToV3(gameObject.transform.p));
		modelMat *= glm::mat4_cast(Mathf::Toquat(gameObject.transform.q));
		modelMat = glm::scale(modelMat, gameObject.g_body->getScale());
		glm::mat4 viewMat = getViewMat();
		glm::mat4 projectionMat = glm::perspective(45.0f, (float)glutGet(GLUT_WINDOW_WIDTH) / (float)glutGet(GLUT_WINDOW_HEIGHT), 0.1f, 1000.0f);
		glUniformMatrix4fv(glGetUniformLocation(gModelShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projectionMat));
		glUniformMatrix4fv(glGetUniformLocation(gModelShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(viewMat));
		glUniformMatrix4fv(glGetUniformLocation(gModelShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(modelMat));
		gameObject.g_body->Draw(gModelShader);
		//渲染车轮
		Model* wheels[4] = { gameObject.g_wheel_fl,gameObject.g_wheel_fr,gameObject.g_wheel_bl,gameObject.g_wheel_br};
		PxVec3 offset[4] = { PxVec3(0.88467, -0.7733, 1.6328) , PxVec3(-0.88467, -0.7733, 1.6328) ,PxVec3(0.88467, -0.7733, -1.2502),PxVec3(-0.88467, -0.7733, -1.2502)};

		//应该对每个车轮应用不同转换矩阵
		for (size_t i = 0; i < 4; i++)
		{
			modelMat = glm::mat4(1.0f);
			modelMat = glm::translate(modelMat, Mathf::P3ToV3(gameObject.transform.p));
			modelMat *= glm::mat4_cast(Mathf::Toquat(gameObject.transform.q));
			modelMat = glm::translate(modelMat, Mathf::P3ToV3(offset[i]));
			modelMat *= glm::mat4_cast(Mathf::Toquat(vehicleshapes[i]->getLocalPose().q));
			modelMat = glm::translate(modelMat, Mathf::P3ToV3(-offset[i]));
			modelMat = glm::scale(modelMat, gameObject.g_body->getScale());
			viewMat = getViewMat();
			projectionMat = glm::perspective(45.0f, (float)glutGet(GLUT_WINDOW_WIDTH) / (float)glutGet(GLUT_WINDOW_HEIGHT), 0.1f, 1000.0f);
			glUniformMatrix4fv(glGetUniformLocation(gModelShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projectionMat));
			glUniformMatrix4fv(glGetUniformLocation(gModelShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(viewMat));
			glUniformMatrix4fv(glGetUniformLocation(gModelShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(modelMat));
			wheels[i]->Draw(gModelShader);
		}
		glUseProgram(0);
	}


	
	bool engineState = false;
	ISoundEngine* backgroundMusicEngine = nullptr;
	ISound* snd = nullptr;
	//显示窗口
	void renderCallback()
	{
		//背景音乐播放状态机
		if (backgroundMusic == true)
		{
			if (engineState == false)
			{
			backgroundMusicEngine = createIrrKlangDevice();
			char path[] = "../../assets/audio/owu12-u5eaj.wav";
			snd = backgroundMusicEngine->play2D(path, true, false, true);
			if (snd)
				snd->setVolume(volume0);
			engineState = true;
			}
			else
			{
				snd->setVolume(volume0);
			}
		}
		else
		{
			if (engineState == true)
			{
				backgroundMusicEngine->drop();
				engineState = false;
			}
		}

		//Imgui中需要加入渲染回调的函数
		Snippets::glut_display_func();
		
		//物理模拟
		stepPhysics(true);

		//渲染相机场景
		Snippets::startRender(sCamera->getEye(), sCamera->getDir(),0.1f, 1000.0f);

		RenderSkybox();


		/////////////////////角色渲染//////////////////////////
		RenderCarObject(carObject);

		//渲染场景物体
		for (int i = 0; i < theCreator.SceneGameObject.size(); i++)
		{
			RenderGameObject(theCreator.SceneGameObject[i]);
		}

		//渲染特殊物体
		for (int i = 0; i < theCreator.SpecialGameObject.size(); i++)
		{
			RenderGameObject(theCreator.SpecialGameObject[i]);
		}

		//RenderGameObject(carObject);
		//RenderGameObject(wheelFLObj);
		//RenderGameObject(wheelFRObj);
		//RenderGameObject(wheelBLObj);
		//RenderGameObject(wheelBRObj);
		
		float rotateSpeed = 5;
		//表示正在移动
		PxExtendedVec3 haha= m_player->getFootPosition();
		if (!moveDir.isZero())
		{
			glm::vec3 targetDir = glm::vec3(moveDir.x, 0, moveDir.z);
			forwardDir = glm::normalize( Mathf::Slerp(forwardDir, targetDir, deltaTime * rotateSpeed));
		}
		RenderModel(gModel, glm::vec3(haha.x, haha.y, haha.z),-forwardDir, gModelShader);
			//RenderModel(gModel, glm::vec3(-20.0f, 10.0f, -45.0f), gModelShader);
		//RenderModel(gModel2, glm::vec3(10.0, 1.0f, 10.0f),glm::vec3(0,0,1),
		//	gModelShader);



		

		/////////////////////EndTest////////////////////////////


		PxScene* scene;
		PxGetPhysics().getScenes(&scene,1);

		//获取场景中的Actor并用OpenGL渲染
		PxU32 nbActors = scene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC);
		if(nbActors)
		{
			std::vector<PxRigidActor*> actors(nbActors);
			scene->getActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC, reinterpret_cast<PxActor**>(&actors[0]), nbActors);
			Snippets::renderActors(&actors[0], static_cast<PxU32>(actors.size()), false, isWireframe);
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

		//===========================================
		ImGui_ImplOpenGL2_Shutdown();
		ImGui_ImplGLUT_Shutdown();
		ImGui::DestroyContext();
		//===========================================
	}
}



	//渲染流程
	void renderLoop()
	{

		sCamera = new Snippets::Camera(PxVec3(50.0f, 50.0f, 50.0f), PxVec3(-0.6f,-0.2f,-0.7f));
		sCamera->SetConfig(4,PxVec3(0,0,0));

		//初始化鼠标位置;
		//GetCursorPos(&p);
		//lastX = p.x;
		//lastY = p.y;

		//Snippets::setupDefaultGLFWWindows();

		Snippets::setupDefaultWindow("Nayeon Studio");
		Snippets::setupDefaultRenderState();

		glewInit();





		//----------Render Model----------
		gBodyModel = Model("../../assets/objects/car/body.obj");
		gWheelModel_fl = Model("../../assets/objects/car/wheel_fl.obj");
		gWheelModel_fr = Model("../../assets/objects/car/wheel_fr.obj");
		gWheelModel_bl = Model("../../assets/objects/car/wheel_bl.obj");
		gWheelModel_br= Model("../../assets/objects/car/wheel_br.obj");

		gModel = Model("../../assets/objects/nanosuit/nanosuit.obj");
		//gModel2 = Model("../../assets/objects/Models/house.fbx");
		//最初的shader
		gModelShader = Shader("../../src/ModelLoading/model_loading.vs",
								"../../src/ModelLoading/model_loading.fs");
		////使用带光照的shader
		//gModelShader = Shader("../../src/Light/light.vs", "../../src/Light/light.fs");

		//----------Render Model----------
		SetupSkybox();

		//这个idle函数意为空闲函数，将在事件队列的最后（即完成鼠标键盘事件响应，准备下一个渲染帧，渲染当前帧）进行，具有最低的优先级
		glutIdleFunc(idleCallback);

		glutDisplayFunc(renderCallback);

		initImGUI();
		

		//键盘事件回调函数
		//glutKeyboardFunc(keyboardCallback);

		glutSetCursor(GLUT_CURSOR_NONE);

		//glutMouseFunc(mouseCallback);

		//glutMotionFunc(motionCallback);

		//glutPassiveMotionFunc(motionCallback);
	
		//motionCallback(0,0);

		atexit(exitCallback);

		initPhysics(true);

		//记录游戏第一帧时间

		QueryPerformanceCounter((LARGE_INTEGER*)&gTime);
		QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
		gLastTime = gTime;
		firstCount = gTime;

		glutMainLoop();
	}
#endif
