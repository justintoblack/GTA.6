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
#include "../DemoTest/MissionManager.h"
#include "irrKlang/irrKlang.h" 
#include "../GameDemo/TheCreator.h"

using namespace irrklang;
using namespace physx;


GLuint              gCubeTexture;
GLuint              gClockTexture;
GLuint				gClockMainTexture;
unsigned int		gSkyboxVAO, gSkyboxVBO;
unsigned int		gClockVAO, gClockVBO, gClockEBO;
unsigned int		gClockMainVAO, gClockMainVBO, gClockMainEBO;
Model				gModel;
Model               gBodyModel, gWheelModel_fl, gWheelModel_fr, gWheelModel_bl, gWheelModel_br;
Model               gStar, gArrow, gExclamation;
Shader				gSkyboxShader;
Shader				gModelShader;
Shader				gShadowShader;
Shader				gClockShader;
Shader				gClockMainShader;
glm::vec3			gLightPos = glm::vec3(-1200.0f, 600.0f, -1200.0f);
glm::vec3			gLightDir;
float				gLightAmbientBasis = 0.3f;//6点、18点左右的亮度
float				gLightDiffuseBasis = 0.3f;
float				gLightSpecularBasis = 0.6f;
glm::vec3			gLightAmbient = glm::vec3(gLightAmbientBasis);
glm::vec3			gLightDiffuse = glm::vec3(gLightDiffuseBasis);
glm::vec3			gLightSpecular = glm::vec3(gLightSpecularBasis);
glm::vec3			gSpotLightAmbient = glm::vec3(0.2f, 0.2f, 0.2f);
glm::vec3			gSpotLightDiffuse = glm::vec3(0.3f, 0.3f, 0.3f);
glm::vec3			gSpotLightSpecular = glm::vec3(0.5f, 0.5f, 0.5f);
float				gSpotLightCutOff = 25.0f;
float				gVehicleShininess = 64.0f;
float				gOthersShininess = 512.0f;
extern	bool		vehicleUseSpotLight;

//时钟顶点位置
float gClockVertices[] = {
	// positions          // colors           // texture coords
	 0.26667f,  0.7f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
	 0.26667f, 0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
	-0.26667f, 0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
	-0.26667f,  0.7f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
};

//时钟顶点数组
unsigned int gClockIndices[] = {
	   0, 1, 3, // first triangle
	   1, 2, 3  // second triangle
};

//时钟顶点位置
float gClockMainVertices[] = {
	// positions          // colors           // texture coords
	 0.1125f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
	 0.1125f, 0.6f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
	-0.1125f, 0.6f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
	-0.1125f,  1.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
};

//时钟顶点数组
unsigned int gClockMainIndices[] = {
	   0, 1, 3, // first triangle
	   1, 2, 3  // second triangle
};

//天空盒的纹理图片
const char* gSkyboxFaces[12] = {
	"../../assets/SkyboxImages/right.jpg",
	"../../assets/SkyboxImages/left.jpg",
	"../../assets/SkyboxImages/top.jpg",
	"../../assets/SkyboxImages/bottom.jpg",
	"../../assets/SkyboxImages/front.jpg",
	"../../assets/SkyboxImages/back.jpg",
	"../../assets/SkyboxImages/right1.png",
	"../../assets/SkyboxImages/left1.png",
	"../../assets/SkyboxImages/top1.png",
	"../../assets/SkyboxImages/bottom1.png",
	"../../assets/SkyboxImages/front1.png",
	"../../assets/SkyboxImages/back1.png"
};

//天空盒渲染相关参数
int widthDay, heightDay, nrChannelsDay;
int widthNight, heightNight, nrChannelsNight;
unsigned char* dataDay[6];
unsigned char* dataNight[6];


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
extern MissionManager missionManager;
//extern void keyPress(unsigned char key, const PxTransform& camera);

//时间
float deltaTime;		//时间插值
float gameTime;		//当前程序执行时间
float currentTime = 0.0f;			//游戏世界现在已经进行到多少秒了
float compareTimeSpeed;
bool day;                          //当前帧是否为白天
bool former;					   //当前帧是否为（白天或者夜晚）的前半段
int calendarDay = 0;				
int calendarHour = 0;
int calendarMinute = 0;
int calendarDayDisplay = 0;			//游戏现在已经进行到第几天了
int calendarHourDisplay = 0;		//这是游戏内那一天的几点
int calendarMinuteDisplay = 0;		//这是游戏内那一个小时内的第几分钟
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
extern bool scenario;
extern bool scenarioChange;
extern float timeSpeed;
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

	void SetupClock()
	{
		//时钟
		glGenVertexArrays(1, &gClockVAO);
		glGenBuffers(1, &gClockVBO);
		glGenBuffers(1, &gClockEBO);

		glBindVertexArray(gClockVAO);

		glBindBuffer(GL_ARRAY_BUFFER, gClockVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(gClockVertices), gClockVertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gClockEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(gClockIndices), gClockIndices, GL_STATIC_DRAW);

		// position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		// color attribute
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		// texture coord attribute
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);

		// texture 1
		// ---------
		glGenTextures(1, &gClockTexture);
		glBindTexture(GL_TEXTURE_2D, gClockTexture);
		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		//float borderColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };
		//glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// load image, create texture and generate mipmaps
		int width, height, nrChannels;
		stbi_set_flip_vertically_on_load(true);

		unsigned char* data = stbi_load("../../assets/Textures/clock.png", &width, &height, &nrChannels, STBI_rgb_alpha);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
		}
		stbi_image_free(data);
	}


	void RenderClock()
	{
		
		//glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		// bind Texture
		glBindTexture(GL_TEXTURE_2D, gClockTexture);

		gClockShader.use(); // don't forget to activate/use the shader before setting uniforms!
		
		glBindVertexArray(gClockVAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


		glBindVertexArray(0);
		/*漏掉这句后，胶囊体变为两颗小球，胶囊体中间的圆柱体不见了*/
		glDeleteBuffers(1, &gClockVBO);
		glUseProgram(0);

	}

	void SetupClockMain()
	{
		//时钟
		glGenVertexArrays(1, &gClockMainVAO);
		glGenBuffers(1, &gClockMainVBO);
		glGenBuffers(1, &gClockMainEBO);

		glBindVertexArray(gClockMainVAO);

		glBindBuffer(GL_ARRAY_BUFFER, gClockMainVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(gClockMainVertices), gClockMainVertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gClockMainEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(gClockMainIndices), gClockMainIndices, GL_STATIC_DRAW);

		// position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		// color attribute
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		// texture coord attribute
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);

		// texture 1
		// ---------
		glGenTextures(1, &gClockMainTexture);
		glBindTexture(GL_TEXTURE_2D, gClockMainTexture);
		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		//float borderColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };
		//glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// load image, create texture and generate mipmaps
		int width, height, nrChannels;
		stbi_set_flip_vertically_on_load(true);

		unsigned char* data = stbi_load("../../assets/Textures/a.jpg", &width, &height, &nrChannels, STBI_rgb_alpha);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
		}
		stbi_image_free(data);
	}

	void RenderClockMain()
	{

		//glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		// bind Texture
		glBindTexture(GL_TEXTURE_2D, gClockMainTexture);

		// create transformations
		//glm::mat4 transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
		//transform = glm::translate(transform, glm::vec3(0.5f, -0.6f, 0.0f));
		//transform = glm::rotate(transform, currentTime, glm::vec3(0.0f, 0.0f, 1.0f));
		

		gClockMainShader.use(); // don't forget to activate/use the shader before setting uniforms!

		//unsigned int transformLoc = glGetUniformLocation(gClockMainShader.ID, "transform");
		//glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

		//gClockMainShader.SetMatrix4fv("transform", transform);

		glBindVertexArray(gClockMainVAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


		glBindVertexArray(0);
		/*漏掉这句后，胶囊体变为两颗小球，胶囊体中间的圆柱体不见了*/
		glDeleteBuffers(1, &gClockVBO);
		glUseProgram(0);

	}

	void SetupSkybox()
	{

		// 天空盒 VAO
		glGenVertexArrays(1, &gSkyboxVAO);
		glGenBuffers(1, &gSkyboxVBO);
		glBindVertexArray(gSkyboxVAO);
		glBindBuffer(GL_ARRAY_BUFFER, gSkyboxVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(gSkyboxVertices), &gSkyboxVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glBindVertexArray(0);

		//int width, height, nrChannels;

		// 禁用多边形背面上的光照、阴影和颜色计算及操作
		glCullFace(GL_BACK);
		//GL_CCW 表示窗口坐标上投影多边形的顶点顺序为逆时针方向的表面为正面
		glFrontFace(GL_CCW);
		//开启深度测试
		glEnable(GL_DEPTH_TEST);
		// 创建纹理对象
		glGenTextures(1, &gCubeTexture);
		glBindTexture(GL_TEXTURE_CUBE_MAP, gCubeTexture);


		if (scenario == true) {
			for (GLuint i = 0; i < 6; i++) {
				if (dataDay[i]) {
					glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB,
						widthDay, heightDay, 0, GL_RGB, GL_UNSIGNED_BYTE, dataDay[i]);
				}
				else {
					std::cout << "Failed to load : " << gSkyboxFaces[i] << std::endl;
					stbi_image_free(dataDay[i]);
				}
			}
		}
		else {
			for (GLuint i = 0; i < 6; i++) {
				if (dataNight[i]) {
					glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB,
						widthNight, heightNight, 0, GL_RGB, GL_UNSIGNED_BYTE, dataNight[i]);
				}
				else {
					std::cout << "Failed to load : " << gSkyboxFaces[i+6] << std::endl;
					stbi_image_free(dataNight[i]);
				}
			}
		}


		// 设置纹理过滤模式
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		//设置纹理渲染模式
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
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
	void RenderModel(Model& model, glm::vec3 pos, glm::vec3 dir , Shader& modelShader, Shader& shadowShader)
	{

		//glEnable(GL_DEPTH_TEST);
		model.setPos(pos);
		modelShader.use();
		//设置光源的属性：环境光强度、漫反射强度、镜面反射强度
		PxVec3 viewPos = sCamera->getEye();
		modelShader.SetVector3f("viewPos", viewPos.x, viewPos.y, viewPos.z);
		//动态改变光的方向
		gLightDir = -gLightPos;
		modelShader.SetVector3f("light.direction", gLightDir);
		modelShader.SetVector3f("light.ambient", gLightAmbient);
		modelShader.SetVector3f("light.diffuse", gLightDiffuse);
		modelShader.SetVector3f("light.specular", gLightSpecular);
		//shininess发光值，发光值越高，反射能力越强，散射越少，高光点越小
		modelShader.SetFloat("material.shininess", gOthersShininess);
		glm::mat4 modelMat = glm::mat4(1.0f);
		modelMat = glm::translate(modelMat, model.getPos());  //平移操作，在（1.0， 1.0， 1.0， 1.0）的基础上平移model.getPos()， 默认为vec3（0.0， 0.0， 0.0， 0.0），上面有个setPos来传入参数
		//modelMat = glm::rotate(modelMat, 1.0f, glm::vec3(0, -1, 0));  //旋转操作，第一个参数是原矩阵，第二个参数是选装角度，用弧度制（glm::radians(90.0f)）， 第三个参数表示绕哪个轴旋转
		modelMat *= glm::mat4_cast(glm::quatLookAt(dir, glm::vec3(0, 1, 0)));    // ??????????
		modelMat = glm::scale(modelMat, glm::vec3(.1f, .1f, .1f)); // 缩放操作，x， y，z坐标都缩小到原来的十分之一
		glm::mat4 viewMat = getViewMat();
		glm::mat4 projectionMat = glm::perspective(45.0f, (float)glutGet(GLUT_WINDOW_WIDTH) / (float)glutGet(GLUT_WINDOW_HEIGHT), 0.1f, 1000.0f);
		modelShader.SetMatrix4fv("projection", projectionMat);
		modelShader.SetMatrix4fv("view", viewMat);
		modelShader.SetMatrix4fv("model", modelMat);


		/*std::cout << "=======================================================================" << std::endl;
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				std::cout << modelMat[i][j] << std::endl;
			}
		}

		std::cout << "=======================================================================" << std::endl;*/
		

		model.Draw(modelShader);



		//shadow
		//====================================
		if (scenario == true) {
			shadowShader.use();
			shadowShader.SetMatrix4fv("projection", projectionMat);
			shadowShader.SetMatrix4fv("view", viewMat);
			shadowShader.SetMatrix4fv("model", modelMat);
			shadowShader.SetVector3f("light", gLightPos);
			model.Draw(shadowShader);
		}
		//=====================================

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
			gModelShader.SetMatrix4fv("projection", projectionMat);
			gModelShader.SetMatrix4fv("view", viewMat);
			gModelShader.SetMatrix4fv("model", modelMat);
			gModelShader.SetFloat("material.shininess", gOthersShininess);
			mod->MyModel->Draw(gModelShader);


			//shadow
			//================================
			/*if (scenario == true) {
			gShadowShader.use();
			gShadowShader.SetMatrix4fv("projection", projectionMat);
			gShadowShader.SetMatrix4fv("view", viewMat);
			gShadowShader.SetMatrix4fv("model", modelMat);
			gShadowShader.SetVector3f("light", gLightPos);
			mod->MyModel->Draw(gShadowShader);
			}*/
			//=================================


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
		glm::mat4 modelMat0 = glm::mat4(1.0f);
		modelMat0 = glm::translate(modelMat0, Mathf::P3ToV3(gameObject.transform.p));
		modelMat0 *= glm::mat4_cast(Mathf::Toquat(gameObject.transform.q));
		modelMat0 = glm::scale(modelMat0, gameObject.g_body->getScale());
		glm::mat4 viewMat = getViewMat();
		glm::mat4 projectionMat = glm::perspective(45.0f, (float)glutGet(GLUT_WINDOW_WIDTH) / (float)glutGet(GLUT_WINDOW_HEIGHT), 0.1f, 1000.0f);
		gModelShader.SetMatrix4fv("projection", projectionMat);
		gModelShader.SetMatrix4fv("view", viewMat);
		gModelShader.SetMatrix4fv("model", modelMat0);
		gameObject.g_body->switchSpotLightStatus(vehicleUseSpotLight, gModelShader);
		gameObject.g_body->Draw(gModelShader);

		
		


		//渲染车轮
		Model* wheels[4] = { gameObject.g_wheel_fl,gameObject.g_wheel_fr,gameObject.g_wheel_bl,gameObject.g_wheel_br};
		PxVec3 offset[4] = { PxVec3(0.88467, -0.7733, 1.6328) , PxVec3(-0.88467, -0.7733, 1.6328) ,PxVec3(0.88467, -0.7733, -1.2502),PxVec3(-0.88467, -0.7733, -1.2502)};
		//spotlight
		//车辆中心位置
		PxVec3 vehiclePos = gameObject.transform.p;
		//车辆前进方向
		PxVec3 vehicleForward = gameObject.transform.q.getBasisVector2().getNormalized();
		//车辆水平方向
		//PxVec3 vehicleHorizon = gameObject.transform.q.getBasisVector0().getNormalized();
		//灯位置
		PxVec3 vehicleLight = vehiclePos + vehicleForward * 2.3f;

		//向shader传入参数
		gModelShader.SetVector3f("spotLight.position", vehicleLight.x, vehicleLight.y, vehicleLight.z);
		gModelShader.SetVector3f("spotLight.direction", vehicleForward.x, vehicleForward.y - 0.3f, vehicleForward.z);
		gModelShader.SetFloat("spotLight.cutOff", glm::cos(glm::radians(gSpotLightCutOff)));
		gModelShader.SetVector3f("spotLight.ambient", gSpotLightAmbient);
		gModelShader.SetVector3f("spotLight.diffuse", gSpotLightDiffuse);
		gModelShader.SetVector3f("spotLight.specular", gSpotLightSpecular);
		gModelShader.SetFloat("material.shininess", gVehicleShininess);
		//应该对每个车轮应用不同转换矩阵
		for (size_t i = 0; i < 4; i++)
		{
			glm::mat4 modelMat1 = glm::mat4(1.0f);
			modelMat1 = glm::translate(modelMat1, Mathf::P3ToV3(gameObject.transform.p));
			modelMat1 *= glm::mat4_cast(Mathf::Toquat(gameObject.transform.q));
			modelMat1 = glm::translate(modelMat1, Mathf::P3ToV3(offset[i]));
			modelMat1 *= glm::mat4_cast(Mathf::Toquat(vehicleshapes[i]->getLocalPose().q));
			modelMat1 = glm::translate(modelMat1, Mathf::P3ToV3(-offset[i]));
			modelMat1 = glm::scale(modelMat1, gameObject.g_body->getScale());
			viewMat = getViewMat();
			projectionMat = glm::perspective(45.0f, (float)glutGet(GLUT_WINDOW_WIDTH) / (float)glutGet(GLUT_WINDOW_HEIGHT), 0.1f, 1000.0f);
			gModelShader.SetMatrix4fv("projection", projectionMat);
			gModelShader.SetMatrix4fv("view", viewMat);
			gModelShader.SetMatrix4fv("model", modelMat1);
			wheels[i]->Draw(gModelShader);
			
		}

		//shadow(just for the body)
		//================================
		if (scenario == true) {
			gShadowShader.use();
			gShadowShader.SetMatrix4fv("projection", projectionMat);
			gShadowShader.SetMatrix4fv("view", viewMat);
			gShadowShader.SetMatrix4fv("model", modelMat0);
			gShadowShader.SetVector3f("light", gLightPos);
			gameObject.g_body->Draw(gShadowShader);
		}
		//=================================
		glUseProgram(0);
	}


	//渲染任务图标
	void RenderMissionObject()
	{
		for (size_t i = 0; i < missionManager.MissionList.size(); i++)
		{
			if (!missionManager.MissionList[i]->State)
			{
				Model m;
				PxTransform t;
				if (!missionManager.MissionList[i]->IsActive)
				{
					m = gExclamation;
					t = missionManager.MissionList[i]->StartTrigger->getGlobalPose();
				}
				else
				{
					m = gStar;
					t = missionManager.MissionList[i]->EndTrigger->getGlobalPose();

				}
				gModelShader.use();
				glm::mat4 modelMat0 = glm::mat4(1.0f);
				modelMat0 = glm::translate(modelMat0, Mathf::P3ToV3(t.p));
				modelMat0 *= glm::mat4_cast(Mathf::Toquat(t.q));
				modelMat0 = glm::scale(modelMat0, m.getScale()/=50);
				glm::mat4 viewMat = getViewMat();
				glm::mat4 projectionMat = glm::perspective(45.0f, (float)glutGet(GLUT_WINDOW_WIDTH) / (float)glutGet(GLUT_WINDOW_HEIGHT), 0.1f, 1000.0f);
				gModelShader.SetMatrix4fv("projection", projectionMat);
				gModelShader.SetMatrix4fv("view", viewMat);
				gModelShader.SetMatrix4fv("model", modelMat0);
				m.Draw(gModelShader);
				glUseProgram(0);
			}

		}

	}
	
	bool engineState = false;
	ISoundEngine* backgroundMusicEngine = nullptr;
	ISound* snd = nullptr;

	ISoundEngine* BackgroundMusicEngine = createIrrKlangDevice();
	char pathDay[] = "../../assets/audio/owu12-u5eaj.wav";
	char pathNight[] = "../../assets/audio/Lawrence.wav";
	ISound* sndDay = BackgroundMusicEngine->play2D(pathDay, true, true, false, ESM_AUTO_DETECT);
	ISound* sndNight = BackgroundMusicEngine->play2D(pathNight, true, true, false, ESM_AUTO_DETECT);

	void playBackgroundMusic(float timeSpeed, int calendarHour, int calendarMinute, bool backgroundMusic, float volume0 )
	{
		

		if (backgroundMusic == true)
		{
			if (engineState == false) {
				sndDay->setIsPaused(false);
				sndNight->setIsPaused(false);
				engineState = true;
			}
			if (calendarHour < 2) {
				float rate = (float)(calendarHour * 60 + calendarMinute) / 120;
				sndDay->setVolume(volume0 * rate);
				sndNight->setVolume(0.0);
				cout << "rate: " << rate << endl;
				//cout << "calendarHour: " << calendarHour << endl;
				//cout << "calendarMinute: " << calendarMinute << endl;
			}
			if (calendarHour > 1 && calendarHour < 10) {
				sndDay->setVolume(volume0);
				sndNight->setVolume(0.0);
			}
			if (calendarHour > 9 && calendarHour < 12) {
				float rate = (float)(120 - (calendarHour - 10) * 60 - calendarMinute) / 120;
				sndDay->setVolume(volume0 * rate);
				sndNight->setVolume(0.0);
				cout << "rate: " << rate << endl;
				//cout << "calendarHour: " << calendarHour << endl;
				//cout << "calendarMinute: " << calendarMinute << endl;
			}
			if (calendarHour > 11 && calendarHour < 14) {
				float rate = (float)((calendarHour - 12) * 60 + calendarMinute) / 120;
				sndNight->setVolume(volume0 * rate);
				sndDay->setVolume(0.0);
				cout << "rate: " << rate << endl;
				//cout << "calendarHour: " << calendarHour << endl;
				//cout << "calendarMinute: " << calendarMinute << endl;
			}
			if (calendarHour > 13 && calendarHour < 21) {
				sndNight->setVolume(volume0);
				sndDay->setVolume(0.0);
			}
			if (calendarHour > 21 && calendarHour < 24) {
				float rate = (float)(120 - (calendarHour - 22) * 60 - calendarMinute) / 120;
				sndNight->setVolume(volume0 * rate);
				sndDay->setVolume(0.0);

				cout << "rate: " << rate << endl;
				//cout << "calendarHour: " << calendarHour << endl;
				//cout << "calendarMinute: " << calendarMinute << endl;
			}
		}
		else {
			if (engineState == true) {
				sndDay->setIsPaused(true);
				sndNight->setIsPaused(true);
				engineState = false;
			}
		}
	}


	void playBackgroundMusic()
	{
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
	}
	//随时间动态变换光照强度
	void changeLightDynamic(bool add)
	{
		float dayPeriod = 240.0f / timeSpeed;
		float omega = 2.0f * glm::pi<float>() / dayPeriod;
		if (add)
		{
			gLightAmbient = glm::vec3(gLightAmbientBasis + 0.4f * glm::sin(omega * currentTime));
			gLightDiffuse = glm::vec3(gLightDiffuseBasis + 0.2f * glm::sin(omega * currentTime));
			gLightSpecular = glm::vec3(gLightSpecularBasis + 0.4 * glm::sin(omega * currentTime));
		}
		else {
			gLightAmbient = glm::vec3(gLightAmbientBasis - 0.2f * abs(glm::sin(omega * currentTime)));
			gLightDiffuse = glm::vec3(gLightDiffuseBasis - 0.2f * abs(glm::sin(omega * currentTime)));
			gLightSpecular = glm::vec3(gLightSpecularBasis - 0.2 * abs(glm::sin(omega * currentTime)));
		}
	}

	//显示窗口
	void renderCallback()
	{

		//关于时间(这是一段无法维护的代码)
		QueryPerformanceCounter((LARGE_INTEGER*)&gTime); //get current count
		QueryPerformanceFrequency((LARGE_INTEGER*)&freq); //get processor freq
		deltaTime = (float)(gTime - gLastTime) / (float)freq;
		gLastTime = gTime;
		gameTime = (float)(gTime - firstCount) / (float)pow(10, 7);
		//float gameTimeMatch = gameTime / timeSpeed;
		currentTime += deltaTime;  //当前时间线
		if (timeSpeed != 0.0) {
			if (compareTimeSpeed != timeSpeed) {
				float rate = compareTimeSpeed / timeSpeed;
				currentTime *= rate;
			}
		}
		else
		{
			currentTime -= deltaTime;
		}

		//昼夜时间钟

		int intGmaeTime = (int)gameTime;  //int版的游戏时间
		int intCurrentTime = (int)currentTime; //int版的当前时间线

		float clock = currentTime - (intCurrentTime / (int)(120 / timeSpeed)) * (int)(120 / timeSpeed); //产生时钟上升沿跳变
		if (timeSpeed != 0.0) {
		int turn = intCurrentTime % (int)(240 / timeSpeed);  
		day = (turn >= 0 && turn < (120 / timeSpeed))? true : false;  //现在是白天还是黑夜（黑夜0， 白天1）
		int half = intCurrentTime % (int)(120 / timeSpeed); 
		former = (half >= 0 && half < (60 / timeSpeed)) ? true : false; //现在是不是白天或者黑夜的上半场（是1， 不是0）
		}


		//Calendar
		if (timeSpeed != 0.0) {
			//游戏内一天的周期等于现实240秒/timeSpeed
			calendarDay = intCurrentTime / (int)(240 / timeSpeed);
			calendarDayDisplay = calendarDay + 1;
			//游戏内一小时等于现实10秒/timeSpeed
			calendarHour = (int)((intCurrentTime % (int)(240 / timeSpeed)) * timeSpeed / 10);
			calendarHourDisplay = calendarHour;
			if (calendarHour < 24) {
				calendarHourDisplay += 6;
			}
			if (calendarHour > 17 ) {
				calendarHourDisplay = calendarHour - 18;
			}
			calendarMinute = ((intCurrentTime % (int)(240 / timeSpeed)) * timeSpeed - calendarHour * 10) * 6;
			calendarMinuteDisplay = calendarMinute;
		}

		//游戏时钟的应用：
		//==============================================================
		if (day) {        
			scenario = true;
			if (former) {
				gLightPos.x += 20.0f * timeSpeed * deltaTime;
				gLightPos.y += 20.0f * timeSpeed * deltaTime;
				gLightPos.z += 20.0f * timeSpeed * deltaTime;
			}
			else {
				gLightPos.x += 20.0f * timeSpeed * deltaTime;
				gLightPos.y -= 20.0f * timeSpeed * deltaTime;
				gLightPos.z += 20.0f * timeSpeed * deltaTime;
			}
		}
		else
		{
			scenario = false;
			//gLightPos.x = -1200.0f;
			//gLightPos.y = 600.0f;
			//gLightPos.z = -1200.0f;
			if (former) {
				gLightPos.x -= 20.0f * timeSpeed * deltaTime;
				gLightPos.y += 20.0f * timeSpeed * deltaTime;
				gLightPos.z -= 20.0f * timeSpeed * deltaTime;
			}
			else {
				gLightPos.x -= 20.0f * timeSpeed * deltaTime;
				gLightPos.y -= 20.0f * timeSpeed * deltaTime;
				gLightPos.z -= 20.0f * timeSpeed * deltaTime;
			}
		}
		//输出游戏当前时间
		/*cout << "===============================";
		cout << calendarHourDisplay << ":" << calendarMinuteDisplay << endl;
		cout << "===============================";*/
		//动态变换光强度
		//scenario 是否白天 
		changeLightDynamic(scenario);
		if (clock < deltaTime) {
			scenarioChange = true;
		}

		//====================================================================




		if (timeSpeed != 0.0) {         //保存上一帧的timeSpeed状态
		compareTimeSpeed = timeSpeed;
		}

		//cout << clock<<endl;
		//cout << currentTime << endl;
		//std::cout << deltaTime << std::endl;
		//std::cout << intGmaeTime << std::endl;
		//std::cout << (int)gameTime << std::endl;
		//cout << timeSpeed << endl;
		//cout << "您已进入游戏: " << gameTime << "秒				";
		//cout << "游戏进入第" << calendarDayDisplay << "天    " << calendarHourDisplay << "点" << calendarMinuteDisplay << "分" << endl;
		//cout << currentTime <<endl;

		
		

		//天空盒初始化状态机
		if (scenarioChange == true) {
			SetupSkybox();
			cout << "scenario: " << scenario << endl;
			cout << "=======================================" << endl;
			scenarioChange = false;
		}

		//背景音乐播放状态机
		playBackgroundMusic(timeSpeed, calendarHour, calendarMinute, backgroundMusic, volume0);
		
		//Imgui中需要加入渲染回调的函数
		Snippets::glut_display_func();
		
		//物理模拟
		stepPhysics(true);

		//渲染相机场景
		Snippets::startRender(sCamera->getEye(), sCamera->getDir(),0.1f, 1000.0f);




		RenderSkybox();

		//RenderClock();
		//RenderClockMain();

		RenderCarObject(carObject);
		RenderMissionObject();
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
		RenderModel(gModel, glm::vec3(haha.x, haha.y, haha.z),-forwardDir, gModelShader, gShadowShader);
			//RenderModel(gModel, glm::vec3(-20.0f, 10.0f, -45.0f), gModelShader);
		//RenderModel(gModel2, glm::vec3(10.0, 1.0f, 10.0f),glm::vec3(0,0,1),
		//	gModelShader);



		


		//////////////////////To be deleted////////////////////////////////
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
		//////////////////////To be deleted////////////////////////////////





		Snippets::finishRender();

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
		sCamera->SetConfig(2.5f, 2.25f, 3.0f, PxVec3(0, 0.5f, 0));


		//初始化鼠标位置;
		//GetCursorPos(&p);
		//lastX = p.x;
		//lastY = p.y;


		Snippets::setupDefaultWindow("Nayeon Studio");
		Snippets::setupDefaultRenderState();

		glewInit();

		
		for (int i = 0; i < 6; i++) {
			dataDay[i] = stbi_load(gSkyboxFaces[i], &widthDay, &heightDay, &nrChannelsDay, 0);
		}
		for (int i = 6; i < 12; i++) {
			dataNight[i-6] = stbi_load(gSkyboxFaces[i], &widthNight, &heightNight, &nrChannelsNight, 0);
		}

		//----------Render Model----------
		gBodyModel = Model("../../assets/objects/car/body.obj");
		gWheelModel_fl = Model("../../assets/objects/car/wheel_fl.obj");
		gWheelModel_fr = Model("../../assets/objects/car/wheel_fr.obj");
		gWheelModel_bl = Model("../../assets/objects/car/wheel_bl.obj");
		gWheelModel_br= Model("../../assets/objects/car/wheel_br.obj");


		gStar = Model("../../assets/objects/mission/SM_Icon_Star_01.fbx");
		gArrow = Model("../../assets/objects/mission/SM_Icon_Arrow_Small_01.fbx");
		gExclamation = Model("../../assets/objects/mission/SM_Icon_Letter_Exclamation_01.fbx");

		gModel = Model("../../assets/objects/nanosuit/nanosuit.obj");
		//gModel2 = Model("../../assets/objects/Models/hougitse.fbx");
		


		//最初的shader
		gModelShader = Shader("../../src/ModelLoading/model_loading.vs",
								"../../src/ModelLoading/model_loading.fs");
		gShadowShader = Shader("../../src/ModelLoading/shadow_loading.vs",
								"../../src/ModelLoading/shadow_loading.fs");
		//加载SHADER
		gSkyboxShader = Shader("../../src/Render/SkyBox.vs",
			"../../src/Render/SkyBox.fs");

		gClockShader = Shader("../../src/Render/Clock.vs",
			"../../src/Render/Clock.fs");

		gClockMainShader = Shader("../../src/Render/ClockMain.vs",
			"../../src/Render/ClockMain.fs");
		////使用带光照的shader
		//gModelShader = Shader("../../src/Light/light.vs", "../../src/Light/light.fs");

		//----------Render Model----------
		//SetupSkybox();

		//SetupClockMain();
		//SetupClock();

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
