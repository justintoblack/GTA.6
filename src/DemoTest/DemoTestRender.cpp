﻿//
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
#include "../Bone/ModelAnimation.h"
#include "../Bone/Animation.h"
#include "../Bone/Animator.h"
#include "../InputSystem/ActionMap.h"

using namespace irrklang;
using namespace physx;


GLuint              gCubeTexture;
GLuint              gClockTexture;
GLuint				gClockMainTexture;
GLuint				gDashboardTexture;
GLuint				gTaskBarTexture;
GLuint				gSelectTexture;
GLuint				gBoxTexture;

unsigned int		gSkyboxVAO, gSkyboxVBO;
unsigned int		gClockVAO, gClockVBO, gClockEBO;
unsigned int		gClockMainVAO, gClockMainVBO, gClockMainEBO;
unsigned int		gDashboardVAO, gDashboardVBO, gDashboardEBO;
unsigned int		gTaskBarVAO, gTaskBarVBO, gTaskBarEBO;
unsigned int		gSelectVAO, gSelectVBO, gSelectEBO;
unsigned int		gBoxVAO, gBoxVBO, gBoxEBO;

Model				gModel;
Model               gBodyModel, gWheelModel_fl, gWheelModel_fr, gWheelModel_bl, gWheelModel_br;
Model               gStar, gArrow, gExclamation;

Shader				gSkyboxShader;
Shader				gModelShader;
Shader				gShadowShader;
Shader				gClockShader;
Shader				gClockMainShader;
Shader				gDashboardShader;
Shader				gTaskBarShader;
Shader				gSelectShader;
Shader				gBoxShader;


float				gLightAmbientBasis = 0.3f;//6点、18点左右的亮度
float				gLightDiffuseBasis = 0.3f;
float				gLightSpecularBasis = 0.6f;
glm::vec3			gLightPos = glm::vec3(-1200.0f, 600.0f, -1200.0f);
glm::vec3			gLightDir;
glm::vec3			gLightAmbient = glm::vec3(gLightAmbientBasis);
glm::vec3			gLightDiffuse = glm::vec3(gLightDiffuseBasis);
glm::vec3			gLightSpecular = glm::vec3(gLightSpecularBasis);
glm::vec3			gSpotLightAmbient = glm::vec3(0.7f, 0.7f, 0.7f);
glm::vec3			gSpotLightDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
glm::vec3			gSpotLightSpecular = glm::vec3(0.8f, 0.8f, 0.8f);


glm::mat4			clockTrans = glm::mat4(1.0f);
glm::mat4			clockMainTrans = glm::mat4(1.0f);
glm::mat4			dashboardTrans = glm::mat4(1.0f);
glm::mat4			taskBarTrans = glm::mat4(1.0f);
glm::mat4			selectTrans = glm::mat4(1.0f);
//glm::mat4			boxTrans = glm::mat4(1.0f);


float				gSpotLightCutOff = 28.0f;
float				gSpotLightOuterCutOff = 35.0f;
float				gVehicleShininess = 64.0f;
float				gOthersShininess = 512.0f;
float				gAttenuationConstant = 1.0f;
float				gAttenuationLinear = 0.09;
float				gAttenuationQuadratic = 0.032;
extern	bool		vehicleUseSpotLight;
Shader				gModelAnimShader;
ModelAnimation*		gModelAnim;
Animation*			gAnimation;
Animation*			gAnimationIdle;
Animation*			gAnimatonJump;
Animation*			gAnimationRun;
Animator*			gAnimator;
Animator*			gAnimatorIdle;
Animator*			gAnimatorJump;
Animator*			gAnimatorRun;
Animator*			gCurrentAnimator = nullptr;
vector<glm::mat4>	gBoneTransform;
extern PxVec3* CameraFollowTarget;
extern PxVec3 currentTraceTarge;
extern  bool		isDriving;
extern  bool		isJumping;
extern  bool		startJumping;
extern  bool		isRunning;
//时钟顶点位置
float gClockVertices[] = {
	// positions          // colors           // texture coords
	 0.1f,  0.1f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
	 0.1f, -0.1f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
	-0.1f, -0.1f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
	-0.1f,  0.1f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
};

//时钟顶点数组
unsigned int gClockIndices[] = {
	   0, 1, 3, // first triangle
	   1, 2, 3  // second triangle
};

//时钟基座顶点位置
float gClockMainVertices[] = {
	// positions          // colors           // texture coords
	 0.1066f,  0.1522f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
	 0.1066f, -0.1522f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
	-0.1066f, -0.1522f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
	-0.1066f,  0.1522f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
};

//时钟基座顶点数组
unsigned int gClockMainIndices[] = {
	   0, 1, 3, // first triangle
	   1, 2, 3  // second triangle
};

//仪表盘顶点位置
float gDashboardVertices[] = {
	// positions          // colors           // texture coords
	 0.2865f,  0.4235f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
	 0.2865f, -0.4235f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
	-0.2865f, -0.4235f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
	-0.2865f,  0.4235f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
};

//仪表盘顶点数组
unsigned int gDashboardIndices[] = {
	   0, 1, 3, // first triangle
	   1, 2, 3  // second triangle
};

float dashboardTheta = 0.0f;
glm::mat4 rotateMain = glm::mat4(1.0f);

//任务栏顶点位置
float gTaskBarVertices[] = {
	// positions          // colors           // texture coords
	 0.2069f,  0.4067f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
	 0.2069f, -0.4067f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
	-0.2069f, -0.4067f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
	-0.2069f,  0.4067f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
};

//任务栏顶点数组
unsigned int gTaskBarIndices[] = {
	   0, 1, 3, // first triangle
	   1, 2, 3  // second triangle
};

//选择框顶点位置
float gSelectVertices[] = {
	// positions          // colors           // texture coords
	 0.03463f,  0.08633f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
	 0.03463f, -0.08633f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
	-0.03463f, -0.08633f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
	-0.03463f,  0.08633f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
};

//选择框顶点数组
unsigned int gSelectIndices[] = {
	   0, 1, 3, // first triangle
	   1, 2, 3  // second triangle
};

//任务详情顶点位置
float gBoxVertices[] = {
	// positions          // colors           // texture coords
	 1.0f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
	 1.0f, -1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
	-1.0f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
	-1.0f,  1.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
};

//任务详情顶点数组
unsigned int gBoxIndices[] = {
	   0, 1, 3, // first triangle
	   1, 2, 3  // second triangle
};


const char* gTaskBarFaces[2] = {
	"../../assets/Textures/taskBarFull.png",
	"../../assets/Textures/missionDisplay.png"
};

const char* gBoxFaces[8] = {
	"../../assets/Textures/missionDisplay1.png",
	"../../assets/Textures/missionDisplay2.png",
	"../../assets/Textures/missionDisplay3.png",
	"../../assets/Textures/missionDisplay4.png",
	"../../assets/Textures/quitGame.png",
	"../../assets/Textures/enterGame.png",
	"../../assets/Textures/success.png",
	"../../assets/Textures/failed.png"
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


//UI渲染相关参数
int widthDay, heightDay, nrChannelsDay;
int widthNight, heightNight, nrChannelsNight;
int gClockWidth, gClockHeight, gClockNrChannels;
int  gClockMainWidth, gClockMainHeight, gClockMainNrChannels;
int  gDashboardWidth, gDashboardHeight, gDashboardNrChannels;
int  gTaskBarWidth, gTaskBarHeight, gTaskBarNrChannels;
int  gSelectWidth, gSelectHeight, gSelectNrChannels;
int  gBoxWidth, gBoxHeight, gBoxNrChannels;

unsigned char* dataDay[6];
unsigned char* dataNight[6];
unsigned char* gClockData;
unsigned char* gClockMainData;
unsigned char* gDashboardData;
unsigned char* gTaskBarData[2];
unsigned char* gSelectData;
unsigned char* gBoxData[8];


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

//绕任意轴的旋转
glm::mat4 RotateArbitraryLine(glm::vec3 v1, glm::vec3 v2, float theta)
{
	glm::mat4 rmatrix;
	float a = v1.x;
	float b = v1.y;
	float c = v1.z;

	glm::vec3 p1 = v2 - v1;
	glm::vec3 p = glm::normalize(p1);

	float u = p.x;
	float v = p.y;
	float w = p.z;

	float uu = u * u;
	float uv = u * v;
	float uw = u * w;
	float vv = v * v;
	float vw = v * w;
	float ww = w * w;
	float au = a * u;
	float av = a * v;
	float aw = a * w;
	float bu = b * u;
	float bv = b * v;
	float bw = b * w;
	float cu = c * u;
	float cv = c * v;
	float cw = c * w;

	float costheta = glm::cos(theta);
	float sintheta = glm::sin(theta);

	rmatrix[0][0] = uu + (vv + ww) * costheta;
	rmatrix[0][1] = uv * (1 - costheta) + w * sintheta;
	rmatrix[0][2] = uw * (1 - costheta) - v * sintheta;
	rmatrix[0][3] = 0;

	rmatrix[1][0] = uv * (1 - costheta) - w * sintheta;
	rmatrix[1][1] = vv + (uu + ww) * costheta;
	rmatrix[1][2] = vw * (1 - costheta) + u * sintheta;
	rmatrix[1][3] = 0;

	rmatrix[2][0] = uw * (1 - costheta) + v * sintheta;
	rmatrix[2][1] = vw * (1 - costheta) - u * sintheta;
	rmatrix[2][2] = ww + (uu + vv) * costheta;
	rmatrix[2][3] = 0;

	rmatrix[3][0] = (a * (vv + ww) - u * (bv + cw)) * (1 - costheta) + (bw - cv) * sintheta;
	rmatrix[3][1] = (b * (uu + ww) - v * (au + cw)) * (1 - costheta) + (cu - aw) * sintheta;
	rmatrix[3][2] = (c * (uu + vv) - w * (au + bv)) * (1 - costheta) + (av - bu) * sintheta;
	rmatrix[3][3] = 1;

	return rmatrix;
}




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
glm::vec3 forwardDir(0, 0, 1);
extern PxController* m_player;
extern InputSyetem inputSystem;
extern PxVehicleDrive4W* gVehicle4W;
Snippets::Camera* sCamera;


/////////////////////////Imgui//////////////////////////////////

extern bool backgroundMusic;
extern float volume0;
extern bool scenario;
extern bool scenarioChange;
extern float timeSpeed;
bool isWireframe = false;


// /////////////////////////Imgui//////////////////////////////////


////////////////////////////////////UI////////////////////////////////////////////

extern bool leftKey;
extern bool rightKey;
extern bool upKey;
extern bool downKey;
extern bool midKey;
extern bool addKey;
extern bool reduceKey;
extern bool escKey;
extern bool enterKey;

extern void SwitchMode();

extern bool soundEffect;

bool chooseMusic = false;
bool hitMusic = false;
bool enterMusic = false;
bool beginMusic = false;
bool InBeginMusic = false;
bool missionMusic = false;
bool winMusic = false;
bool failedMusic = false;

bool missionFinish = false;
bool missionSuccess = false;


int currentSelect = 5;
bool isInConfirm = false; //在任务详情界面
bool isSelected = false;  //已接受任务时被置为true，没接受任务或者任务结束的那一帧被置为false
bool isInQuit = false;	  //在退出游戏界面
bool isInBegin = true;    //在游戏欢迎界面
bool isEndBegin = false;  //只会在退出欢迎界面的那一帧被置为true，该信号量会用于执行一次SwitchMode，但外界将无法读取到这个信号量被置为true的那一帧
bool gotoSwitchMode = true;  //该信号量只会出现一次跳转，为true只发生在RenderCallback的第一次循环，即第一帧，用于执行一次SwitchMode，之后就一直是false
bool haveSetupBox = false;	 //该信号量用于保证游戏欢迎界面只会被SetupBox一次，并且将负责与退出游戏的SetupBox进行协调
bool compareIsSelected = true; //用于追踪上一帧的IsSelect的值，仅当在任务栏状态切换的那一帧，isSelect != compareIsSelect，且这个状态只出现在两个函数体之间
bool isInEnd = false;     //表示是否在结算界面，一旦接收到missionFinish=true，就会将该值置为true，然后持续三秒时间，三秒时间过后，该值被置为false，missionFinish被置为false，
						  //同时isSelected被置为false，代表此时不在接受任务的状态
float timeCount = 0.0f;
bool haveSetupBoxInEnd = false;


float currentAngle = 0.0f;


////////////////////////////////////UI////////////////////////////////////////////
 



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
		if (key == 27)
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
		//float borderColor[] = { 1.0f, 1.0f, 0.0f, 0.0f };
		//glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// load image, create texture and generate mipmaps
		
		
		//int gClockWidth, gClockHeight, gClockNrChannels;
		//stbi_set_flip_vertically_on_load(true);

		//unsigned char* gClockData = stbi_load("../../assets/Textures/clock.png", &gClockWidth, &gClockHeight, &gClockNrChannels, STBI_rgb_alpha);
		if (gClockData)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, gClockWidth, gClockHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, gClockData);
			
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
		}
		stbi_image_free(gClockData);
	}

	void RenderClock()
	{

		//glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		// bind Texture
		glBindTexture(GL_TEXTURE_2D, gClockTexture);

		glm::vec3 tail = glm::vec3(0.8f, 0.83f, -1.0f);
		glm::vec3 head = glm::vec3(0.8f, 0.83f, 1.0f);
		float theta = glm::radians(1.5f * deltaTime * timeSpeed);

		glm::mat4 rotate = RotateArbitraryLine(tail, head, theta);
		//clockTrans = glm::scale(clockTrans, glm::vec3(float(192 / 108), 1.0, 1.0));
		clockTrans = rotate * clockTrans;
		//clockTrans = glm::scale(clockTrans, glm::vec3(0.5625, 1.0, 1.0));


		gClockShader.use(); // don't forget to activate/use the shader before setting uniforms!

		unsigned int transformLoc = glGetUniformLocation(gClockShader.ID, "transform");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(clockTrans));
		
		glBindVertexArray(gClockVAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


		//glBindVertexArray(0);
		/*漏掉这句后，胶囊体变为两颗小球，胶囊体中间的圆柱体不见了*/
		//glDeleteVertexArrays(1, &gClockVAO);
		//glDeleteBuffers(1, &gClockVBO);
		//glDeleteBuffers(1, &gClockEBO);
		//glUseProgram(0);

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


		glGenTextures(1, &gClockMainTexture);
		glBindTexture(GL_TEXTURE_2D, gClockMainTexture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		

		if (gClockMainData)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, gClockMainWidth, gClockMainHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, gClockMainData);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
		}
		stbi_image_free(gClockMainData);
	}

	void RenderClockMain()
	{

		
		glBindTexture(GL_TEXTURE_2D, gClockMainTexture);

		glm::vec3 tailMain = glm::vec3(0.7f, -0.5839f, -1.0f);
		glm::vec3 headMain = glm::vec3(0.7f, -0.5839f, 1.0f);

		float currentThetaUp = 6.0f * deltaTime * (float)(270.0f / (dashboardTheta + 30.0f)) / 6.0f;    //每帧的角度值变化
		float thetaMainUp = glm::radians(-currentThetaUp); //每帧的弧度值变化
		float currentThetaDown = 6.0f * deltaTime * (float)((dashboardTheta + 30.0f) / 90.0f);    //每帧的角度值变化
		float thetaMainDown = glm::radians(-currentThetaDown); //每帧的弧度值变化

		if (isDriving == true && dashboardTheta < 270) {

			rotateMain = RotateArbitraryLine(tailMain, headMain, thetaMainUp);  //每帧的旋转
			dashboardTheta += currentThetaUp;  //旋转后递增增加的角度值，用于判断
		}
		else if (isDriving == false && dashboardTheta > 0)
		{
			rotateMain = RotateArbitraryLine(tailMain, headMain, -thetaMainDown);
			dashboardTheta -= currentThetaDown ;
		}
		else
		{
			rotateMain = glm::mat4(1.0f);
		}
		

		//clockTrans = glm::scale(clockTrans, glm::vec3(float(192 / 108), 1.0, 1.0));
		clockMainTrans = rotateMain * clockMainTrans;

		gClockMainShader.use(); 

		unsigned int transform = glGetUniformLocation(gClockMainShader.ID, "transformMain");
		glUniformMatrix4fv(transform, 1, GL_FALSE, glm::value_ptr(clockMainTrans));

		glBindVertexArray(gClockMainVAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	}

	void SetupDashboard()
	{
		//时钟
		glGenVertexArrays(1, &gDashboardVAO);
		glGenBuffers(1, &gDashboardVBO);
		glGenBuffers(1, &gDashboardEBO);

		glBindVertexArray(gDashboardVAO);

		glBindBuffer(GL_ARRAY_BUFFER, gDashboardVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(gDashboardVertices), gDashboardVertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gDashboardEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(gDashboardIndices), gDashboardIndices, GL_STATIC_DRAW);

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
		glGenTextures(1, &gDashboardTexture);
		glBindTexture(GL_TEXTURE_2D, gDashboardTexture);
		// set the texture wrapping parameters
		//float borderColor[] = { 1.0f, 1.0f, 0.0f, 0.0f };
		//glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// load image, create texture and generate mipmaps


		//int gClockWidth, gClockHeight, gClockNrChannels;
		

		//unsigned char* gClockData = stbi_load("../../assets/Textures/clock.png", &gClockWidth, &gClockHeight, &gClockNrChannels, STBI_rgb_alpha);
		if (gDashboardData)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, gDashboardWidth, gDashboardHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, gDashboardData);

			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
		}
		stbi_image_free(gDashboardData);
	}

	void RenderDashboard()
	{

		//glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		// bind Texture
		glBindTexture(GL_TEXTURE_2D, gDashboardTexture);

		gDashboardShader.use(); // don't forget to activate/use the shader before setting uniforms!

		unsigned int transformDashboard = glGetUniformLocation(gDashboardShader.ID, "transformDashboard");
		glUniformMatrix4fv(transformDashboard, 1, GL_FALSE, glm::value_ptr(dashboardTrans));

		glBindVertexArray(gDashboardVAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}

	void SetupTaskBar()
	{
		//时钟
		glGenVertexArrays(1, &gTaskBarVAO);
		glGenBuffers(1, &gTaskBarVBO);
		glGenBuffers(1, &gTaskBarEBO);

		glBindVertexArray(gTaskBarVAO);

		glBindBuffer(GL_ARRAY_BUFFER, gTaskBarVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(gTaskBarVertices), gTaskBarVertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gTaskBarEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(gTaskBarIndices), gTaskBarIndices, GL_STATIC_DRAW);

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
		glGenTextures(1, &gTaskBarTexture);
		glBindTexture(GL_TEXTURE_2D, gTaskBarTexture);
		// set the texture wrapping parameters

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


		if (!isSelected) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, gTaskBarWidth, gTaskBarHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, gTaskBarData[0]);
			glGenerateMipmap(GL_TEXTURE_2D);
			//stbi_image_free(gTaskBarData[0]);
		}
		else {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, gTaskBarWidth, gTaskBarHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, gTaskBarData[1]);
			glGenerateMipmap(GL_TEXTURE_2D);
			//stbi_image_free(gTaskBarData[1]);
		}


	}

	void RenderTaskBar()
	{

		//glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		// bind Texture
		glBindTexture(GL_TEXTURE_2D, gTaskBarTexture);

		gTaskBarShader.use(); // don't forget to activate/use the shader before setting uniforms!

		unsigned int transformTaskBar = glGetUniformLocation(gTaskBarShader.ID, "transformTaskBar");
		glUniformMatrix4fv(transformTaskBar, 1, GL_FALSE, glm::value_ptr(taskBarTrans));

		glBindVertexArray(gTaskBarVAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}

	void SetupSelect()
	{
		//时钟
		glGenVertexArrays(1, &gSelectVAO);
		glGenBuffers(1, &gSelectVBO);
		glGenBuffers(1, &gSelectEBO);

		glBindVertexArray(gSelectVAO);

		glBindBuffer(GL_ARRAY_BUFFER, gSelectVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(gSelectVertices), gSelectVertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gSelectEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(gSelectIndices), gSelectIndices, GL_STATIC_DRAW);

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
		glGenTextures(1, &gSelectTexture);
		glBindTexture(GL_TEXTURE_2D, gSelectTexture);
		// set the texture wrapping parameters

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		if (gSelectData)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, gSelectWidth, gSelectHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, gSelectData);

			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
		}
		stbi_image_free(gSelectData);
	}

	void RenderSelect()
	{

		//glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		// bind Texture
		glBindTexture(GL_TEXTURE_2D, gSelectTexture);

		gSelectShader.use(); // don't forget to activate/use the shader before setting uniforms!

		if (leftKey == true)
		{
			if (currentSelect != 1 && currentSelect != 4 && currentSelect != 7) {
				selectTrans = glm::translate(selectTrans, glm::vec3(-0.0775f, 0.0f, 0.0f)); //           x:0.0775         y:0.1911
				currentSelect -= 1;
				chooseMusic = true;
				cout << "currentSelect" << currentSelect << endl;
			}
			else {
				hitMusic = true;
			}
		}
		else if (rightKey == true)
		{
			if (currentSelect != 3 && currentSelect != 6 && currentSelect != 9) {
				selectTrans = glm::translate(selectTrans, glm::vec3(0.0775f, 0.0f, 0.0f));
				currentSelect += 1;
				chooseMusic = true;
				cout << "currentSelect" << currentSelect << endl;
			}
			else {
				hitMusic = true;
			}
		}
		else if (upKey == true)
		{	
			if (currentSelect != 7 && currentSelect != 8 && currentSelect != 9) {
				selectTrans = glm::translate(selectTrans, glm::vec3(0.0f, 0.1911f, 0.0f));
				currentSelect += 3;
				chooseMusic = true;
				cout << "currentSelect" << currentSelect << endl;
			}
			else {
				hitMusic = true;
			}
		}
		else if (downKey == true)
		{
			if (currentSelect != 1 && currentSelect != 2 && currentSelect != 3) {
				selectTrans = glm::translate(selectTrans, glm::vec3(0.0f, -0.1911f, 0.0f));
				currentSelect -= 3;
				chooseMusic = true;
				cout << "currentSelect" << currentSelect << endl;
			}
			else {
				hitMusic = true;
			}
		}
		else
		{
			chooseMusic = false;
			hitMusic = false;
		}
		

		unsigned int transformSelect = glGetUniformLocation(gSelectShader.ID, "transformSelect");
		glUniformMatrix4fv(transformSelect, 1, GL_FALSE, glm::value_ptr(selectTrans));

		glBindVertexArray(gSelectVAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}

	void SetupBox()
	{
		//时钟
		glGenVertexArrays(1, &gBoxVAO);
		glGenBuffers(1, &gBoxVBO);
		glGenBuffers(1, &gBoxEBO);

		glBindVertexArray(gBoxVAO);

		glBindBuffer(GL_ARRAY_BUFFER, gBoxVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(gBoxVertices), gBoxVertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gBoxEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(gBoxIndices), gBoxIndices, GL_STATIC_DRAW);

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
		glGenTextures(1, &gBoxTexture);
		glBindTexture(GL_TEXTURE_2D, gBoxTexture);
		// set the texture wrapping parameters

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		
			
		

		if (isInQuit == true) {
			if (gBoxData[4]) {
				cout << "gBoxData is ready!!!" << endl;
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, gBoxWidth, gBoxHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, gBoxData[4]);
				glGenerateMipmap(GL_TEXTURE_2D);

			}
			else
			{
				std::cout << "Failed to load texture" << std::endl;
			}
			//stbi_image_free(gBoxData[0]);         绝对不要去释放掉这些data内存，否则下一次使用时会因找不到数据而报错
		}
		else if (isInBegin == true) {
			if (gBoxData[5]) {
				cout << "gBoxData is ready!!!" << endl;
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, gBoxWidth, gBoxHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, gBoxData[5]);
				glGenerateMipmap(GL_TEXTURE_2D);

			}
			else
			{
				std::cout << "Failed to load texture" << std::endl;
			}
			//stbi_image_free(gBoxData[0]);         绝对不要去释放掉这些data内存，否则下一次使用时会因找不到数据而报错
		}
		else if (isInEnd == true) {
			if (missionSuccess == true) {
				if (gBoxData[6]) {
					cout << "gBoxData is ready!!!" << endl;
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, gBoxWidth, gBoxHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, gBoxData[6]);
					glGenerateMipmap(GL_TEXTURE_2D);

				}
				else
				{
					std::cout << "Failed to load texture" << std::endl;
				}
			}
			else
			{
				if (gBoxData[7]) {
					cout << "gBoxData is ready!!!" << endl;
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, gBoxWidth, gBoxHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, gBoxData[7]);
					glGenerateMipmap(GL_TEXTURE_2D);

				}
				else
				{
					std::cout << "Failed to load texture" << std::endl;
				}
			}
		}
		else
		{

			if (currentSelect == 1 || currentSelect == 5 || currentSelect == 7) {
				if (gBoxData[0]) {
					cout << "gBoxData is ready!!!" << endl;
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, gBoxWidth, gBoxHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, gBoxData[0]);
					glGenerateMipmap(GL_TEXTURE_2D);

				}
				else
				{
					std::cout << "Failed to load texture" << std::endl;
				}
				//stbi_image_free(gBoxData[0]);         绝对不要去释放掉这些data内存，否则下一次使用时会因找不到数据而报错
			}

			if (currentSelect == 4 || currentSelect == 9) {
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, gBoxWidth, gBoxHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, gBoxData[1]);
				glGenerateMipmap(GL_TEXTURE_2D);
				//stbi_image_free(gBoxData[1]);
			}

			if (currentSelect == 3 || currentSelect == 8) {
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, gBoxWidth, gBoxHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, gBoxData[2]);
				glGenerateMipmap(GL_TEXTURE_2D);
				//stbi_image_free(gBoxData[2]);
			}

			if (currentSelect == 2 || currentSelect == 6) {
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, gBoxWidth, gBoxHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, gBoxData[3]);
				glGenerateMipmap(GL_TEXTURE_2D);
				//stbi_image_free(gBoxData[3]);
			}
		}
	}

	void RenderBox()
	{

		//glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		// bind Texture
		glBindTexture(GL_TEXTURE_2D, gBoxTexture);

		gBoxShader.use(); // don't forget to activate/use the shader before setting uniforms!

		//unsigned int transformBox = glGetUniformLocation(gBoxShader.ID, "transformBox");
		//glUniformMatrix4fv(transformBox, 1, GL_FALSE, glm::value_ptr(boxTrans));

		glBindVertexArray(gBoxVAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
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
					//stbi_image_free(dataDay[i]);
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
					//stbi_image_free(dataNight[i]);
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


		glDeleteBuffers(1, &gSkyboxVBO); //在每次渲染完之后立即释放掉VBO，而仅仅在Setup之前释放掉VBO就会导致ImGUI的渲染出现问题，猜测可能是共用了同个VBO地址
		glDepthMask(GL_TRUE);
		glUseProgram(0);
	}

	//传入模型对象model，以及模型的位置pos
	void RenderModel(Model& model, glm::vec3 pos, glm::vec3 dir, Shader& modelShader, Shader& shadowShader)
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
		modelMat = glm::translate(modelMat, model.getPos()); //平移操作，在（1.0， 1.0， 1.0， 1.0）的基础上平移model.getPos()， 默认为vec3（0.0， 0.0， 0.0， 0.0），上面有个setPos来传入参数
		//modelMat = glm::rotate(modelMat, 1.0f, glm::vec3(0, -1, 0));    //旋转操作，第一个参数是原矩阵，第二个参数是选装角度，用弧度制（glm::radians(90.0f)）， 第三个参数表示绕哪个轴旋转
		modelMat *= glm::mat4_cast(glm::quatLookAt(dir, glm::vec3(0, 1, 0)));    // ??????????
		modelMat = glm::scale(modelMat, glm::vec3(.1f, .1f, .1f)); // 缩放操作，x， y，z坐标都缩小到原来的十分之一
		glm::mat4 viewMat = getViewMat();
		glm::mat4 projectionMat = glm::perspective(45.0f, (float)glutGet(GLUT_WINDOW_WIDTH) / (float)glutGet(GLUT_WINDOW_HEIGHT), 0.1f, 1000.0f);
		modelShader.SetMatrix4fv("projection", projectionMat);
		modelShader.SetMatrix4fv("view", viewMat);
		modelShader.SetMatrix4fv("model", modelMat);


		model.Draw(modelShader);


		//shadow
		//====================================
		shadowShader.use();
		shadowShader.SetMatrix4fv("projection", projectionMat);
		shadowShader.SetMatrix4fv("view", viewMat);
		shadowShader.SetMatrix4fv("model", modelMat);
		shadowShader.SetVector3f("light", gLightPos);
		model.Draw(shadowShader);
		//=====================================

		glUseProgram(0);
	}

	void RenderModelAnim(ModelAnimation* modelAnim, glm::vec3 pos, glm::vec3 dir, Shader& modelAnimShader, Shader& shadowShader)
	{

		//glEnable(GL_DEPTH_TEST);
		modelAnim->setPos(pos);
		modelAnimShader.use();
		//设置光源的属性：环境光强度、漫反射强度、镜面反射强度
		PxVec3 viewPos = sCamera->getEye();
		modelAnimShader.SetVector3f("viewPos", viewPos.x, viewPos.y, viewPos.z);
		//动态改变光的方向
		gLightDir = -gLightPos;
		modelAnimShader.SetVector3f("light.direction", gLightDir);
		modelAnimShader.SetVector3f("light.ambient", gLightAmbient);
		modelAnimShader.SetVector3f("light.diffuse", gLightDiffuse);
		modelAnimShader.SetVector3f("light.specular", gLightSpecular);
		
		glm::mat4 modelMat = glm::mat4(1.0f);
		modelMat = glm::translate(modelMat, modelAnim->getPos()); //平移操作，在（1.0， 1.0， 1.0， 1.0）的基础上平移model.getPos()， 默认为vec3（0.0， 0.0， 0.0， 0.0），上面有个setPos来传入参数
		//modelMat = glm::rotate(modelMat, 1.0f, glm::vec3(0, -1, 0));    //旋转操作，第一个参数是原矩阵，第二个参数是旋转角度，用弧度制（glm::radians(90.0f)）， 第三个参数表示绕哪个轴旋转
		modelMat *= glm::mat4_cast(glm::quatLookAt(dir, glm::vec3(0, 1, 0)));    // ??????????
		modelMat = glm::scale(modelMat, glm::vec3(.01f, .01f, .01f));
		glm::mat4 viewMat = getViewMat();
		glm::mat4 projectionMat = glm::perspective(45.0f, (float)glutGet(GLUT_WINDOW_WIDTH) / (float)glutGet(GLUT_WINDOW_HEIGHT), 0.1f, 1000.0f);
		modelAnimShader.SetMatrix4fv("projection", projectionMat);
		modelAnimShader.SetMatrix4fv("view", viewMat);
		modelAnimShader.SetMatrix4fv("model", modelMat);
		modelAnim->Draw(modelAnimShader);

		//shadow
		//====================================
		shadowShader.use();
		shadowShader.SetMatrix4fv("projection", projectionMat);
		shadowShader.SetMatrix4fv("view", viewMat);
		shadowShader.SetMatrix4fv("model", modelMat);
		shadowShader.SetVector3f("light", gLightPos);
		modelAnim->Draw(shadowShader);
		//=====================================

		glUseProgram(0);
	}

	//渲染GameObject
	void RenderGameObject(GameObject& gameObject)
	{
		//是否有父物体
		if (gameObject.parent != nullptr)
		{
			gameObject.transform = gameObject.parent->transform.transform(gameObject.localTransform);
			gameObject.transform.q = gameObject.parent->transform.q;
			gameObject.transform.q *= gameObject.localTransform.q;
			//gameObject.transform = gameObject.parent->transform.transform
			//(gameObject.localTransform);

			//gameObject.transform.q *= gameObject.localTransform.q;
		}
		//需要跟踪物理模拟
		else if (gameObject.g_rigidBody && gameObject.g_rigidBody->getType() ==
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
			//设置光源的属性：环境光强度、漫反射强度、镜面反射强度
			PxVec3 viewPos = sCamera->getEye();
			gModelShader.SetVector3f("viewPos", viewPos.x, viewPos.y, viewPos.z);
			//动态改变光的方向、强度
			gLightDir = -gLightPos;
			gModelShader.SetVector3f("light.direction", gLightDir);
			gModelShader.SetVector3f("light.ambient", gLightAmbient);
			gModelShader.SetVector3f("light.diffuse", gLightDiffuse);
			gModelShader.SetVector3f("light.specular", gLightSpecular);
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
		//车灯会影响到这些shader
		gameObject.g_body->switchSpotLightStatus(vehicleUseSpotLight, gModelShader);	
		gameObject.g_body->Draw(gModelShader);

		//渲染车轮
		Model* wheels[4] = { gameObject.g_wheel_fl,gameObject.g_wheel_fr,gameObject.g_wheel_bl,gameObject.g_wheel_br};
		PxVec3 offset[4] = { PxVec3(0.88467, -0.7733, 1.6328) , PxVec3(-0.88467, -0.7733, 1.6328) ,PxVec3(0.88467, -0.7733, -1.2502),PxVec3(-0.88467, -0.7733, -1.2502)};
		//如果使用了车灯，才进行相关计算 
		PxVec3 vehicleForward, vehicleLight;
		if (vehicleUseSpotLight)
		{
			//spotlight
			//车辆中心位置
			PxVec3 vehiclePos = gameObject.transform.p;
			//车辆前进方向
			vehicleForward = gameObject.transform.q.getBasisVector2().getNormalized();
			//车辆水平方向
			//PxVec3 vehicleHorizon = gameObject.transform.q.getBasisVector0().getNormalized();
			//灯位置
			vehicleLight = vehiclePos + vehicleForward * 2.3f;

			//向shader传入参数
			gModelShader.SetVector3f("spotLight.position", vehicleLight.x, vehicleLight.y, vehicleLight.z);
			gModelShader.SetVector3f("spotLight.direction", vehicleForward.x, vehicleForward.y - 0.3f, vehicleForward.z);
			gModelShader.SetFloat("spotLight.cutOff", glm::cos(glm::radians(gSpotLightCutOff)));
			gModelShader.SetFloat("spotLight.outerCutOff", glm::cos(glm::radians(gSpotLightOuterCutOff)));
			gModelShader.SetVector3f("spotLight.ambient", gSpotLightAmbient);
			gModelShader.SetVector3f("spotLight.diffuse", gSpotLightDiffuse);
			gModelShader.SetVector3f("spotLight.specular", gSpotLightSpecular);
		}
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
		
		gShadowShader.use();
		gShadowShader.SetMatrix4fv("projection", projectionMat);
		gShadowShader.SetMatrix4fv("view", viewMat);
		gShadowShader.SetMatrix4fv("model", modelMat0);
		gShadowShader.SetVector3f("light", gLightPos);
		gameObject.g_body->Draw(gShadowShader);
		
		//=================================
		//无论本次是否使用车灯，都要更新使用车灯的状态
		gModelAnimShader.use();
		gameObject.g_body->switchSpotLightStatus(vehicleUseSpotLight, gModelAnimShader);
		//只有本次使用了车灯，才计算具体的值
		if (vehicleUseSpotLight)
		{
			gModelAnimShader.SetVector3f("spotLight.position", vehicleLight.x, vehicleLight.y, vehicleLight.z);
			gModelAnimShader.SetVector3f("spotLight.direction", vehicleForward.x, vehicleForward.y - 0.3f, vehicleForward.z);
			gModelAnimShader.SetFloat("spotLight.cutOff", glm::cos(glm::radians(gSpotLightCutOff)));
			gModelAnimShader.SetFloat("spotLight.outerCutOff", glm::cos(glm::radians(gSpotLightOuterCutOff)));
			gModelAnimShader.SetVector3f("spotLight.ambient", gSpotLightAmbient);
			gModelAnimShader.SetVector3f("spotLight.diffuse", gSpotLightDiffuse);
			gModelAnimShader.SetVector3f("spotLight.specular", gSpotLightSpecular);
		}
		glUseProgram(0);
	}

	//渲染任务图标
	void RenderMissionObject()
	{
		bool NeedRenderArrow = false;
		for (size_t i = 0; i < missionManager.MissionList.size(); i++)
		{
			
			if (!missionManager.MissionList[i]->State&& missionManager.MissionList[i]->IsTracing==true)
			{
				NeedRenderArrow = true;
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
				modelMat0 = glm::scale(modelMat0, m.getScale());
				glm::mat4 viewMat = getViewMat();
				glm::mat4 projectionMat = glm::perspective(45.0f, (float)glutGet(GLUT_WINDOW_WIDTH) / (float)glutGet(GLUT_WINDOW_HEIGHT), 0.1f, 1000.0f);
				gModelShader.SetMatrix4fv("projection", projectionMat);
				gModelShader.SetMatrix4fv("view", viewMat);
				gModelShader.SetMatrix4fv("model", modelMat0);
				m.Draw(gModelShader);
				glUseProgram(0);
			}



		}

		if (NeedRenderArrow)
		{
			int size;
			if (isDriving)
			{
				size = 10;
			}
			else
			{
				size = 3;
			}
			//渲染指示箭头
			Model m = gArrow;
			PxVec3 p = *CameraFollowTarget;
			p.y += 1;
			PxVec3 dir =( currentTraceTarge - *CameraFollowTarget);
			dir = dir.getNormalized();
			dir.y = 0;
			gModelShader.use();
			glm::mat4 modelMat0 = glm::mat4(1.0f);
			modelMat0 = glm::translate(modelMat0, Mathf::P3ToV3(p)); 
			modelMat0 = glm::rotate(modelMat0, PxAtan2(dir.x, dir.z), glm::vec3(0,1, 0));
			modelMat0 = glm::rotate(modelMat0, glm::radians(90.0f), glm::vec3(1,0, 0));
			modelMat0 = glm::scale(modelMat0,glm::vec3(size,size,size));
			glm::mat4 viewMat = getViewMat();
			glm::mat4 projectionMat = glm::perspective(45.0f, (float)glutGet(GLUT_WINDOW_WIDTH) / (float)glutGet(GLUT_WINDOW_HEIGHT), 0.1f, 1000.0f);
			gModelShader.SetMatrix4fv("projection", projectionMat);
			gModelShader.SetMatrix4fv("view", viewMat);
			gModelShader.SetMatrix4fv("model", modelMat0);
			m.Draw(gModelShader);
			glUseProgram(0);
		}
	}


	bool engineState = false;
	ISoundEngine* backgroundMusicEngine = nullptr;
	ISound* snd = nullptr;

	ISoundEngine* BackgroundMusicEngine = createIrrKlangDevice();
	char pathDay[] = "../../assets/audio/owu12-u5eaj.wav";
	char pathNight[] = "../../assets/audio/Night.wav";
	ISound* sndDay = BackgroundMusicEngine->play2D(pathDay, true, true, false, ESM_AUTO_DETECT);
	ISound* sndNight = BackgroundMusicEngine->play2D(pathNight, true, true, false, ESM_AUTO_DETECT);
	char pathChoose[] = "../../assets/audio/chooseMusic.wav";
	char pathHit[] = "../../assets/audio/hitMusic.wav";
	char pathEnter[] = "../../assets/audio/enterMusic.wav";
	char pathBegin[] = "../../assets/audio/beginMusic.wav";
	char pathInBegin[] = "../../assets/audio/begining.wav";
	char pathMission[] = "../../assets/audio/mission.wav";
	char pathWin[] = "../../assets/audio/win.wav";
	char pathFailed[] = "../../assets/audio/failed.wav";
	ISoundSource* sourceChoose = BackgroundMusicEngine->addSoundSourceFromFile(pathChoose, ESM_AUTO_DETECT, true);
	ISoundSource* sourceHit = BackgroundMusicEngine->addSoundSourceFromFile(pathHit, ESM_AUTO_DETECT, true);
	ISoundSource* sourceEnter = BackgroundMusicEngine->addSoundSourceFromFile(pathEnter, ESM_AUTO_DETECT, true);
	ISoundSource* sourceBegin = BackgroundMusicEngine->addSoundSourceFromFile(pathBegin, ESM_AUTO_DETECT, true);
	ISoundSource* sourceInBegin = BackgroundMusicEngine->addSoundSourceFromFile(pathInBegin, ESM_AUTO_DETECT, true);
	ISoundSource* sourceMission = BackgroundMusicEngine->addSoundSourceFromFile(pathMission, ESM_AUTO_DETECT, true);
	ISoundSource* sourceWin = BackgroundMusicEngine->addSoundSourceFromFile(pathWin, ESM_AUTO_DETECT, true);
	ISoundSource* sourceFailed = BackgroundMusicEngine->addSoundSourceFromFile(pathFailed, ESM_AUTO_DETECT, true);
	ISound* sndMission = BackgroundMusicEngine->play2D(sourceMission, true, true, false, false);
	ISound* sndInBegin = BackgroundMusicEngine->play2D(sourceInBegin, true, true, false, false);

	void playHitMusic(bool hitMusic, bool soundEffect)
	{
		if (soundEffect == true && hitMusic == true) {
			BackgroundMusicEngine->play2D(sourceHit, false, false, false, false);
		}
	}
	void playChooseMusic(bool chooseMusic, bool soundEffect)
	{
		if (soundEffect == true && chooseMusic == true) {
			BackgroundMusicEngine->play2D(sourceChoose, false, false, false, false);
		}
	}
	void playEnterMusic(bool enterMusic, bool soundEffect)
	{
		if (soundEffect == true && enterMusic == true) {
			BackgroundMusicEngine->play2D(sourceEnter, false, false, false, false);
		}
	}
	void playBeginMusic(bool beginMusic, bool soundEffect)
	{
		if (soundEffect == true && beginMusic == true) {
			BackgroundMusicEngine->play2D(sourceBegin, false, false, false, false);
		}
	}
	void playInBeginMusic(bool gotoSwitchMode, bool &InBeginMusic, bool isInBegin) 
	{
		if (gotoSwitchMode == true && InBeginMusic == false) {   //gotoSwitchMode唯一一次等于false且此时音乐没在播放
			cout << "<>InBeginMusic" << InBeginMusic << endl;
			cout << "ready to play InBegin music!!!" << endl;
			sndInBegin->setIsPaused(false);
			cout << "ready to set InBeginMusic!!!================================================" << endl;
			InBeginMusic = true;  //音乐在播放
			cout << "InBeginMusic" << InBeginMusic << endl;
		}
		/*if (sndInBegin) {
			cout << "sndInBegin is exit!!!1" << endl;
		}*/
		//cout << "InBeginMusic" << InBeginMusic << endl;
		//cout << "isInBegin" << isInBegin << endl;
		if (InBeginMusic == true && isInBegin == false) {
			cout << "ready to stop InBegin music!!!" << endl;
			sndInBegin->setIsPaused(true);
			//sndInBegin->drop();
			InBeginMusic = false;
		}
	}
	void playMissionMusic(bool isSelected, bool compareIsSelected, bool soundEffect, bool &missionMusic)
	{
		if (soundEffect == true) {
			cout << "<>isSelected" << isSelected << endl;
			cout << "<>compareIsSelected" << compareIsSelected << endl;
			cout << "<>missionMusic" << missionMusic << endl;

			if (isSelected != compareIsSelected && isSelected == true) {
				cout << "ready to play Mission music!!!" << endl;
				sndMission->setIsPaused(false);
				missionMusic = true;
			}
		}
		//cout << "isSelected" << isSelected << endl;
		//cout << "compareIsSelected" << compareIsSelected << endl;
		//cout << "missionMusic" << missionMusic << endl;
		if ((soundEffect == false && missionMusic == true)|| (isSelected != compareIsSelected && isSelected == false && missionMusic == true)) {
			cout << "ready to play Mission music!!!" << endl;
			sndMission->setIsPaused(true);
			//sndMission->drop();
			missionMusic = false;
		}
	}
	void playWinMusic()
	{
		if ((winMusic == false) && (timeCount > 1.0f) && (missionSuccess == true)) {
			cout << "begin to play WinMusic!!!===============" << endl;
			BackgroundMusicEngine->play2D(sourceWin, false, false, false, false);
			winMusic = true;
		}
	}
	void playFailedMusic()
	{
		if ((failedMusic) == false && (timeCount > 1.0f) && (missionSuccess == false)) {
			cout << "begin to play FailedMusic!!!===============" << endl;
			BackgroundMusicEngine->play2D(sourceFailed, false, false, false, false);
			failedMusic = true;
		}
	}

	void playBackgroundMusic(float timeSpeed, int calendarHour, int calendarMinute, bool backgroundMusic, float volume0)
	{

		if (backgroundMusic == true && isInBegin == false && isSelected == false)
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
				cout << "rate: " << "1.0" << endl;
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
				cout << "rate: " << "1.0" << endl;
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
			gLightAmbient = glm::vec3(gLightAmbientBasis - 0.25f * abs(glm::sin(omega * currentTime)));
			gLightDiffuse = glm::vec3(gLightDiffuseBasis - 0.2f * abs(glm::sin(omega * currentTime)));
			gLightSpecular = glm::vec3(gLightSpecularBasis - 0.4 * abs(glm::sin(omega * currentTime)));
		}
	}

	//显示窗口
	void renderCallback()
	{

		//关于时间(这是一段无法维护的代码)

		//关于时间的各个参数说明：
		//1、 timeSpeed：这个参数代表当前帧的时间流速；float timeSpeed 有这些枚举值：1/6， 1/3， 1/2， 1， 2， 3， 6，0    请注意：timeSpeed可为0！！！
		//2、 compareTimeSpeed： 这个参数在timeSpeed不等于0的情况下都会等于timeSpeed， 当timeSpeed = 0时，这个参数会等于离这一帧最近的不为零的timeSpeed
		//3、 deltaTime，这个值代表这一帧持续的时间
		//4、 clock，这个值只会在昼夜更替的那一帧小于deltaTime
		//5、 currentTime，这个值代表在当前游戏流速的情况下，已经离游戏的开始过去了多少时间，可以理解为在此时的时间流速下，游戏人物的视角中，现在离游戏开始已经过了多久了
		//6、 gameTime,这个值代表在现实中，离游戏的开始过去了多少时间
		//7、 day，这个值是个布尔值，代表当前帧是白天还是黑夜（0代表黑夜，1代表白天）
		//8、 former， 这个值是个布尔值，代表当前帧是（白天或者黑夜）的前半段时间     （1代表是， 0代表不是）
		
		//9、 calendarDay， 这是个int值，游戏开始时从0开始，代表过了几天
		//10、calendarHour，这是个int值，代表当前帧是游戏内哪个小时（0到24），（白天从calendarHour = 0 开始）
		//11、calendarMinute， 这是个int值，代表当前帧是游戏内哪一分钟（0到60）
		
		//9、 calendarDayDisplay， 这是个int值，游戏开始时从1开始，代表现在是第几天
		//10、calendarHourDisplay，这是个int值，代表当前帧是游戏内哪个小时（0到24），（白天从calendarHour = 6 开始）
		//11、calendarMinuteDisplay， 这是个int值，代表当前帧是游戏内哪一分钟（0到60），与calendarMinute并无差别，只是为了对称

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

		int intGmaeTime = (int)gameTime; //int版的游戏时间
		int intCurrentTime = (int)currentTime; //int版的当前时间线

		float clock = currentTime - (intCurrentTime / (int)(120 / timeSpeed)) * (int)(120 / timeSpeed); //产生时钟上升沿跳变
		if (timeSpeed != 0.0) {
			int turn = intCurrentTime % (int)(240 / timeSpeed);
			day = (turn >= 0 && turn < (120 / timeSpeed)) ? true : false;  //现在是白天还是黑夜（黑夜0， 白天1）
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
			if (calendarHour > 17) {
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
			glDeleteVertexArrays(1, &gSkyboxVAO);
			glDeleteBuffers(1, &gSkyboxVBO);
			SetupSkybox();
			//cout << "scenario: " << scenario << endl;
			//cout << "=======================================" << endl;
			scenarioChange = false;
		}

		//背景音乐播放状态机
		playBackgroundMusic(timeSpeed, calendarHour, calendarMinute, backgroundMusic, volume0);
		playChooseMusic(chooseMusic, soundEffect);
		playHitMusic(hitMusic, soundEffect);
		playEnterMusic(enterMusic, soundEffect);
		playBeginMusic(beginMusic, soundEffect);
		playInBeginMusic(gotoSwitchMode, InBeginMusic, isInBegin);
		playWinMusic();
		playFailedMusic();
		
		
		//Imgui中需要加入渲染回调的函数
		Snippets::glut_display_func();


		//改变当前任务追踪状态



		//物理模拟
		stepPhysics(true);

		//渲染相机场景
		Snippets::startRender(sCamera->getEye(), sCamera->getDir(), 0.1f, 1000.0f);

		RenderSkybox();

		if (missionFinish == true) {
			isInEnd = true;
			cout << "isInEnd===========================" << isInEnd << endl;
			timeCount += deltaTime;
			cout << "timeCount" << timeCount << endl;
			isSelected = false;
		}
		if (timeCount >= 5.0f) {
			cout << "timeCount >= 3.0" << endl;
			isInEnd = false;
			haveSetupBoxInEnd = false;
			failedMusic = false;
			winMusic = false;
			cout << "isInEnd===========================" << isInEnd << endl;
			missionFinish = false;
			timeCount = 0.0f;
		}

		if (escKey == true && isInConfirm == false) {
			isInQuit = true;
			//haveSetupBox = false;
			glDeleteVertexArrays(1, &gBoxVAO);
			glDeleteBuffers(1, &gBoxVBO);
			glDeleteBuffers(1, &gBoxEBO);
			cout << "Begin to SetupBox in quit!!! " << endl;
			SetupBox();
		}

		if (isInBegin == true && haveSetupBox == false) {
			haveSetupBox = true;
			glDeleteVertexArrays(1, &gBoxVAO);
			glDeleteBuffers(1, &gBoxVBO);
			glDeleteBuffers(1, &gBoxEBO);
			cout << "Begin to SetupBox in begining!!! " << endl;
			SetupBox();
		}

		if (isInQuit == true && addKey == true) {
			exit(0);
		}

		if (isInQuit == true && reduceKey == true) {
			isInQuit = false;
			haveSetupBox = false;
			cout << "isInQuit" << isInQuit << endl;
		}

		

		if (isInConfirm == true || isInQuit == true || isInBegin == true) {               //现在正处在确认界面中
			//cout << "ready to RenderBox" << endl;
			RenderBox();
			//cout << "RenderBox is working!!!" << endl;
		}

		if (!isInBegin) {

			cout << "missionFinish" << missionFinish << endl;
			cout << "missionSuccess" << missionSuccess << endl;

			if (isInEnd == true && haveSetupBoxInEnd == false) {
				glDeleteVertexArrays(1, &gBoxVAO);
				glDeleteBuffers(1, &gBoxVBO);
				glDeleteBuffers(1, &gBoxEBO);
				cout << "Begin to SetupBox in End!!! " << endl;
				SetupBox();
				haveSetupBoxInEnd = true;
			}
			if (isInEnd == true) {
				cout << "ready to RenderBox of InEnd" << endl;
				RenderBox();
				cout << "RenderBox is working in End!!!" << endl;
			}

			if (midKey == true && isInConfirm == false && isSelected == false) {   //在没有进入确认界面的状态下按下了“5”
				glDeleteVertexArrays(1, &gBoxVAO);
				glDeleteBuffers(1, &gBoxVBO);
				glDeleteBuffers(1, &gBoxEBO);
				cout << "ready to SetupBox" << endl;
				SetupBox();
				cout << "SetupBox is finished" << endl;
				isInConfirm = true;
				cout << "isInConfirm" << isInConfirm << endl;
				enterMusic = true;
			}
			else {
				enterMusic = false;
			}

			//if (midKey == true && isInConfirm == true) {   //处于确认界面的状态下按下了“5”
			//	
			//	isInConfirm = false;
			//	cout << "isInConfirm" << isInConfirm << endl;
			//}

			if (reduceKey == true && isInConfirm == true) { //处于确认界面的状态下按下了“-”
			
				isInConfirm = false;
				cout << "isInConfirm" << isInConfirm << endl;
			}

			if (addKey == true && isInConfirm == true) {    //处于确认界面的状态下按下了“+”
			
				isInConfirm = false;
			
				isSelected = true;               //进入正式任务界面

				missionManager.MissionList[currentSelect]->IsTracing = true;
				currentTraceTarge = missionManager.MissionList[currentSelect]->StartTrigger->getGlobalPose().p;

				beginMusic = true;
				cout << "isInConfirm" << isInConfirm << endl;
			}
			else {
				beginMusic = false;
			}
			playMissionMusic(isSelected, compareIsSelected, soundEffect, missionMusic);

			if (compareIsSelected != isSelected && isInEnd == false) {          //还没进入到正式任务界面
				glDeleteVertexArrays(1, &gTaskBarVAO);
				glDeleteBuffers(1, &gTaskBarVBO);
				glDeleteBuffers(1, &gTaskBarEBO);
				SetupTaskBar();
				compareIsSelected = isSelected;
			}

			if (isSelected == false && isInEnd == false)
			{
				RenderSelect();
			}

			RenderTaskBar();











			if (inputSystem.isVehicle) {
				RenderClockMain();
				RenderDashboard();
			}
		}
		RenderClock();

		

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
			RenderGameObject(*theCreator.SpecialGameObject[i]);
		}

		//RenderGameObject(carObject);
		//RenderGameObject(wheelFLObj);
		//RenderGameObject(wheelFRObj);
		//RenderGameObject(wheelBLObj);
		//RenderGameObject(wheelBRObj);

		float rotateSpeed = 5;
		//表示正在移动
		PxExtendedVec3 haha = m_player->getFootPosition();
		
		if (!moveDir.isZero())
		{
			glm::vec3 targetDir = glm::vec3(moveDir.x, 0, moveDir.z);
			forwardDir = glm::normalize( Mathf::Slerp(forwardDir, targetDir, deltaTime * rotateSpeed));
			
		}
		if (isJumping) {
			if (startJumping) {
				gAnimatorJump->setCurrentTick(15.0f);
				startJumping = false;
			}
			gAnimatorJump->UpdateAnimation(deltaTime);
			gBoneTransform = gAnimatorJump->GetFinalBoneMatrices();
		}
		else if (!moveDir.isZero()) {
			if (isRunning)
			{
				gAnimatorRun->UpdateAnimation(deltaTime);
				gBoneTransform = gAnimatorRun->GetFinalBoneMatrices();
			}
			else //walk
			{
				gAnimator->UpdateAnimation(deltaTime);
				gBoneTransform = gAnimator->GetFinalBoneMatrices();
			}
		}
		else {
			gAnimatorIdle->UpdateAnimation(deltaTime);
			gBoneTransform = gAnimatorIdle->GetFinalBoneMatrices();
		}
		//更新骨骼变换矩阵
		
		gModelAnimShader.use();
		for (int i = 0; i < gBoneTransform.size(); i++)
		{
			string name = "finalBonesMatrices[" + std::to_string(i) + "]";
			gModelAnimShader.SetMatrix4fv(name.c_str(), gBoneTransform[i]);
		}
		RenderModelAnim(gModelAnim, glm::vec3(haha.x, haha.y, haha.z),-forwardDir, gModelAnimShader, gShadowShader);
			//RenderModel(gModel, glm::vec3(-20.0f, 10.0f, -45.0f), gModelShader);
		//RenderModel(gModel2, glm::vec3(10.0, 1.0f, 10.0f),glm::vec3(0,0,1),
		//	gModelShader);


		//////////////////////To be deleted////////////////////////////////
		PxScene* scene;
		PxGetPhysics().getScenes(&scene, 1);

		//获取场景中的Actor并用OpenGL渲染
		PxU32 nbActors = scene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC);
		if (nbActors)
		{
			std::vector<PxRigidActor*> actors(nbActors);
			scene->getActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC, reinterpret_cast<PxActor**>(&actors[0]), nbActors);
			Snippets::renderActors(&actors[0], static_cast<PxU32>(actors.size()), false, isWireframe);
		}
		//////////////////////To be deleted////////////////////////////////

		if (enterKey) {
			cout << "enterKey is pressed!!!" << endl;
		}

		if (isInBegin == true && enterKey == false) {
			if (gotoSwitchMode == true) {
				SwitchMode();
			}
			physx::PxVec3 cameraUIPos = physx::PxVec3(150.0f * sin(glm::radians(currentAngle)), 150.0f, 160.0f * cos(glm::radians(currentAngle)));
			sCamera->SetEye(cameraUIPos);
			physx::PxVec3 cameraUIDir = physx::PxVec3(-150.0f * sin(glm::radians(currentAngle)), -150.0f, -150.0f * cos(glm::radians(currentAngle)));
			sCamera->SetDir(cameraUIDir);
			currentAngle += 10.0f * deltaTime;
		}
		if (isInBegin == true && enterKey == true) {
			isInBegin = false;
			isEndBegin = true;
			cout << "ready go to SwitchMode" << endl;
			SwitchMode();
			isEndBegin = false;
		}


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

	sCamera = new Snippets::Camera(PxVec3(50.0f, 50.0f, 50.0f), PxVec3(-0.6f, -0.2f, -0.7f));
	sCamera->SetConfig(2.5f, 2.25f, 3.0f, PxVec3(0, 0.5f, 0));



		//初始化鼠标位置;
		//GetCursorPos(&p);
		//lastX = p.x;
		//lastY = p.y;


	Snippets::setupDefaultWindow("Nayeon Studio");
	Snippets::setupDefaultRenderState();

	glewInit();

		

		//---------------------stbi_load--------------------------------
		for (int i = 0; i < 6; i++) {
			dataDay[i] = stbi_load(gSkyboxFaces[i], &widthDay, &heightDay, &nrChannelsDay, 0);
		}
		for (int i = 6; i < 12; i++) {
			dataNight[i-6] = stbi_load(gSkyboxFaces[i], &widthNight, &heightNight, &nrChannelsNight, 0);
		}
		gClockData = stbi_load("../../assets/Textures/777.png", &gClockWidth, &gClockHeight, &gClockNrChannels, STBI_rgb_alpha); //STBI_rgb_alpha
		stbi_set_flip_vertically_on_load(true);
		gClockMainData = stbi_load("../../assets/Textures/pointer.png", &gClockMainWidth, &gClockMainHeight, &gClockMainNrChannels, STBI_rgb_alpha);
		gDashboardData = stbi_load("../../assets/Textures/dashboardMain.png", &gDashboardWidth, &gDashboardHeight, &gDashboardNrChannels, STBI_rgb_alpha);

		for (int i = 0; i < 2; i++) {
			gTaskBarData[i] = stbi_load(gTaskBarFaces[i], &gTaskBarWidth, &gTaskBarHeight, &gTaskBarNrChannels, STBI_rgb_alpha);
		}
		
		gSelectData = stbi_load("../../assets/Textures/select.png", &gSelectWidth, &gSelectHeight, &gSelectNrChannels, STBI_rgb_alpha);

		for (int i = 0; i < 8; i++) {
			gBoxData[i] = stbi_load(gBoxFaces[i], &gBoxWidth, &gBoxHeight, &gBoxNrChannels, STBI_rgb_alpha);
		}

		
		stbi_set_flip_vertically_on_load(false);
		


	for (int i = 0; i < 6; i++) {
		dataDay[i] = stbi_load(gSkyboxFaces[i], &widthDay, &heightDay, &nrChannelsDay, 0);
	}
	for (int i = 6; i < 12; i++) {
		dataNight[i - 6] = stbi_load(gSkyboxFaces[i], &widthNight, &heightNight, &nrChannelsNight, 0);
	}

	//----------Render Model----------
	gBodyModel = Model("../../assets/objects/car/body.obj");
	gWheelModel_fl = Model("../../assets/objects/car/wheel_fl.obj");
	gWheelModel_fr = Model("../../assets/objects/car/wheel_fr.obj");
	gWheelModel_bl = Model("../../assets/objects/car/wheel_bl.obj");
	gWheelModel_br = Model("../../assets/objects/car/wheel_br.obj");


	gStar = Model("../../assets/objects/Models/49_Icon_Star.fbx");
	gArrow = Model("../../assets/objects/Models/48_Icon_Arrow.fbx");
	gExclamation = Model("../../assets/objects/Models/50_Icon_Exclamation.fbx");
	gModel = Model("../../assets/objects/nanosuit/nanosuit.obj");
	//gModel2 = Model("../../assets/objects/Models/hougitse.fbx");



	//最初的shader
	gModelShader = Shader("../../src/ModelLoading/model_loading.vs",
		"../../src/ModelLoading/model_loading.fs");
	gShadowShader = Shader("../../src/ModelLoading/shadow_loading.vs",
		"../../src/ModelLoading/shadow_loading.fs");
	//加载SHADER
	gSkyboxShader = Shader("../../src/Render/SkyBox.vs", "../../src/Render/SkyBox.fs");
		gClockMainShader = Shader("../../src/Render/ClockMain.vs", "../../src/Render/ClockMain.fs");

		gDashboardShader = Shader("../../src/Render/Dashboard.vs", "../../src/Render/Dashboard.fs");

		gTaskBarShader = Shader("../../src/Render/TaskBar.vs", "../../src/Render/TaskBar.fs");

		gSelectShader = Shader("../../src/Render/Select.vs", "../../src/Render/Select.fs");

		gBoxShader = Shader("../../src/Render/Box.vs", "../../src/Render/Box.fs");


		////使用带光照的shader
		//gModelShader = Shader("../../src/Light/light.vs", "../../src/Light/light.fs");

		gClockShader = Shader("../../src/Render/Clock.vs", "../../src/Render/Clock.fs");

		////使用带光照的shader
		//gModelShader = Shader("../../src/Light/light.vs", "../../src/Light/light.fs");
		SetupClockMain();
		SetupClock();
		SetupDashboard();
		SetupSelect();
		

	//----------Render Model----------
	//SetupSkybox();

		clockTrans = glm::translate(clockTrans, glm::vec3(0.8f, 0.83f, 0.0f));
		clockMainTrans = glm::translate(clockMainTrans, glm::vec3(0.5934f, -0.7316f, 0.0f));
		dashboardTrans = glm::translate(dashboardTrans, glm::vec3(0.7f, -0.5f, 0.0f));
		taskBarTrans = glm::translate(taskBarTrans, glm::vec3(-0.7920f, 0.59f, 0.0f));
		selectTrans = glm::translate(selectTrans, glm::vec3(-0.7985f, 0.5300f, 0.0f));
		//boxTrans = glm::translate(selectTrans, glm::vec3(0.0f, 0.0f, 0.0f));

		glutDisplayFunc(renderCallback);

		//----------Model Anim-----------------------------
		gModelAnimShader = Shader("../../src/Bone/ModelAnim.vs",
			"../../src/ModelLoading/model_loading.fs");
		string modelAnimPath("../../assets/objects/Models/Standard_Walk_skin.fbx");
		string modelAnimIdlePath("../../assets/objects/Models/Neutral_Idle_skin.fbx");
		string modelAnimJumpPath("../../assets/objects/Models/jump_skin.fbx");
		string modelAnimRunPath("../../assets/objects/Models/Running_skin.fbx");
		gModelAnim = new ModelAnimation(modelAnimPath);
		gAnimation = new Animation(modelAnimPath, gModelAnim);
		gAnimator = new Animator(gAnimation);

		gAnimationIdle = new Animation(modelAnimIdlePath, gModelAnim);
		gAnimatorIdle = new Animator(gAnimationIdle);
		gAnimatorIdle->UpdateAnimation(deltaTime);

		gAnimatonJump = new Animation(modelAnimJumpPath, gModelAnim);
		gAnimatorJump = new Animator(gAnimatonJump);

		gAnimationRun = new Animation(modelAnimRunPath, gModelAnim);
		gAnimatorRun = new Animator(gAnimationRun);

		gModelAnimShader.use();
		//shininess发光值，发光值越高，反射能力越强，散射越少，高光点越小
		gModelAnimShader.SetFloat("material.shininess", gOthersShininess);
		gBoneTransform = gAnimatorIdle->GetFinalBoneMatrices();
		for (int i = 0; i < gBoneTransform.size(); i++)
		{
			string name = "finalBonesMatrices[" + std::to_string(i) + "]";
			gModelAnimShader.SetMatrix4fv(name.c_str(), gBoneTransform[i]);
		}
		glUseProgram(0);
		//----------Model Anim-----------------------------
	

		//这个idle函数意为空闲函数，将在事件队列的最后（即完成鼠标键盘事件响应，准备下一个渲染帧，渲染当前帧）进行，具有最低的优先级
		glutIdleFunc(idleCallback);
		//注册好回调函数后
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
