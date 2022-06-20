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
#define _CRT_SECURE_NO_WARNINGS
#include "../GameDemo/TheCreator.h"
#include "Render.h"
#include <iostream>


using namespace physx;


extern	TheCreator theCreator;


//==================================================================ImGUI state
bool main_window = false;  //之所以不设置为静态全局变量，是因为在DemoTestRender.cpp中会使用到这个变量
bool show_another_window = false;
bool inspector_window = false;
bool isSimulation = true;
static bool show_demo_window = false;

//ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
physx::PxVec3 clear_color = physx::PxVec3(0.45f, 0.55f, 0.60f);
bool backgroundMusic = false;
bool soundEffect = false;
float volume0;
float volume1;
float gameObjectPosition[3] = { 0.10f, 0.20f, 0.30f };
bool editState;


//==================================================================ImGUI state


//////////////////////////ImGUI-Content//////////////////////////////
static GameObject* curGameObject=nullptr;
static PxVec3 _position=PxVec3(0,0,0);
static PxVec3 _rotation=PxVec3(0,0,0);
static PxQuat _quaternion = PxQuat(0, 0, 0, 1);
static bool _isAddRigidbodyStatic=false;
static char  _objName[16];
const char** _allModelsName;


//////////////////////////////ImGUI////////////////////////////////

static float gCylinderData[]={
	1.0f,0.0f,1.0f,1.0f,0.0f,1.0f,1.0f,0.0f,0.0f,1.0f,0.0f,0.0f,
	0.866025f,0.500000f,1.0f,0.866025f,0.500000f,1.0f,0.866025f,0.500000f,0.0f,0.866025f,0.500000f,0.0f,
	0.500000f,0.866025f,1.0f,0.500000f,0.866025f,1.0f,0.500000f,0.866025f,0.0f,0.500000f,0.866025f,0.0f,
	-0.0f,1.0f,1.0f,-0.0f,1.0f,1.0f,-0.0f,1.0f,0.0f,-0.0f,1.0f,0.0f,
	-0.500000f,0.866025f,1.0f,-0.500000f,0.866025f,1.0f,-0.500000f,0.866025f,0.0f,-0.500000f,0.866025f,0.0f,
	-0.866025f,0.500000f,1.0f,-0.866025f,0.500000f,1.0f,-0.866025f,0.500000f,0.0f,-0.866025f,0.500000f,0.0f,
	-1.0f,-0.0f,1.0f,-1.0f,-0.0f,1.0f,-1.0f,-0.0f,0.0f,-1.0f,-0.0f,0.0f,
	-0.866025f,-0.500000f,1.0f,-0.866025f,-0.500000f,1.0f,-0.866025f,-0.500000f,0.0f,-0.866025f,-0.500000f,0.0f,
	-0.500000f,-0.866025f,1.0f,-0.500000f,-0.866025f,1.0f,-0.500000f,-0.866025f,0.0f,-0.500000f,-0.866025f,0.0f,
	0.0f,-1.0f,1.0f,0.0f,-1.0f,1.0f,0.0f,-1.0f,0.0f,0.0f,-1.0f,0.0f,
	0.500000f,-0.866025f,1.0f,0.500000f,-0.866025f,1.0f,0.500000f,-0.866025f,0.0f,0.500000f,-0.866025f,0.0f,
	0.866026f,-0.500000f,1.0f,0.866026f,-0.500000f,1.0f,0.866026f,-0.500000f,0.0f,0.866026f,-0.500000f,0.0f,
	1.0f,0.0f,1.0f,1.0f,0.0f,1.0f,1.0f,0.0f,0.0f,1.0f,0.0f,0.0f
};

#define MAX_NUM_MESH_VEC3S  1024
static PxVec3 gVertexBuffer[MAX_NUM_MESH_VEC3S];




void renderGeometry(const PxGeometryHolder& h)
{
	switch(h.getType())
	{
	case PxGeometryType::eBOX:			
		{
			glScalef(h.box().halfExtents.x, h.box().halfExtents.y, h.box().halfExtents.z);
			glutSolidCube(2.0);
		}
		break;
	case PxGeometryType::eSPHERE:		
		{
			glutSolidSphere(GLdouble(h.sphere().radius), 10, 10);
		}
		break;
	case PxGeometryType::eCAPSULE:
		{

			const PxF32 radius = h.capsule().radius;
			const PxF32 halfHeight = h.capsule().halfHeight;

			//Sphere
			glPushMatrix();
			glTranslatef(halfHeight, 0.0f, 0.0f);
			glScalef(radius,radius,radius);
			glutSolidSphere(1, 10, 10);		
			glPopMatrix();

			//Sphere
			glPushMatrix();
			glTranslatef(-halfHeight, 0.0f, 0.0f);
			glScalef(radius,radius,radius);
			glutSolidSphere(1, 10, 10);		
			glPopMatrix();

			//Cylinder
			glPushMatrix();
			glTranslatef(-halfHeight, 0.0f, 0.0f);
			glScalef(2.0f*halfHeight, radius,radius);
			glRotatef(90.0f,0.0f,1.0f,0.0f);
			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_NORMAL_ARRAY);
			glVertexPointer(3, GL_FLOAT, 2*3*sizeof(float), gCylinderData);
			glNormalPointer(GL_FLOAT, 2*3*sizeof(float), gCylinderData+3);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 13*2);
			glDisableClientState(GL_VERTEX_ARRAY);
			glDisableClientState(GL_NORMAL_ARRAY);
			glPopMatrix();
		}
		break;
	case PxGeometryType::eCONVEXMESH:
		{

			//Compute triangles for each polygon.
			const PxVec3 scale = h.convexMesh().scale.scale;
			PxConvexMesh* mesh = h.convexMesh().convexMesh;
			const PxU32 nbPolys = mesh->getNbPolygons();
			const PxU8* polygons = mesh->getIndexBuffer();
			const PxVec3* verts = mesh->getVertices();
			PxU32 nbVerts = mesh->getNbVertices();
			PX_UNUSED(nbVerts);

			PxU32 numTotalTriangles = 0;
			for(PxU32 i = 0; i < nbPolys; i++)
			{
				PxHullPolygon data;
				mesh->getPolygonData(i, data);

				const PxU32 nbTris = PxU32(data.mNbVerts - 2);
				const PxU8 vref0 = polygons[data.mIndexBase + 0];
				PX_ASSERT(vref0 < nbVerts);
				for(PxU32 j=0;j<nbTris;j++)
				{
					const PxU32 vref1 = polygons[data.mIndexBase + 0 + j + 1];
					const PxU32 vref2 = polygons[data.mIndexBase + 0 + j + 2];

					//generate face normal:
					PxVec3 e0 = verts[vref1] - verts[vref0];
					PxVec3 e1 = verts[vref2] - verts[vref0];

					PX_ASSERT(vref1 < nbVerts);
					PX_ASSERT(vref2 < nbVerts);

					PxVec3 fnormal = e0.cross(e1);
					fnormal.normalize();
			
					if(numTotalTriangles*6 < MAX_NUM_MESH_VEC3S)
					{
						gVertexBuffer[numTotalTriangles*6 + 0] = fnormal;
						gVertexBuffer[numTotalTriangles*6 + 1] = verts[vref0];
						gVertexBuffer[numTotalTriangles*6 + 2] = fnormal;
						gVertexBuffer[numTotalTriangles*6 + 3] = verts[vref1];
						gVertexBuffer[numTotalTriangles*6 + 4] = fnormal;
						gVertexBuffer[numTotalTriangles*6 + 5] = verts[vref2];
						numTotalTriangles++;
					}
				}
			}
			glPushMatrix();
			glScalef(scale.x, scale.y, scale.z);
			glEnableClientState(GL_NORMAL_ARRAY);
			glEnableClientState(GL_VERTEX_ARRAY);
			glNormalPointer(GL_FLOAT, 2*3*sizeof(float), gVertexBuffer);
			glVertexPointer(3, GL_FLOAT, 2*3*sizeof(float), gVertexBuffer+1);
			glDrawArrays(GL_TRIANGLES, 0, int(numTotalTriangles * 3));
			glPopMatrix();
		}
		break;
	case PxGeometryType::eTRIANGLEMESH:
		{
			const PxTriangleMeshGeometry& triGeom = h.triangleMesh();
			const PxTriangleMesh& mesh = *triGeom.triangleMesh;
			const PxVec3 scale = triGeom.scale.scale;

			const PxU32 triangleCount = mesh.getNbTriangles();
			const PxU32 has16BitIndices = mesh.getTriangleMeshFlags() & PxTriangleMeshFlag::e16_BIT_INDICES;
			const void* indexBuffer = mesh.getTriangles();

			const PxVec3* vertexBuffer = mesh.getVertices();

			const PxU32* intIndices = reinterpret_cast<const PxU32*>(indexBuffer);
			const PxU16* shortIndices = reinterpret_cast<const PxU16*>(indexBuffer);
			PxU32 numTotalTriangles = 0;
			for(PxU32 i=0; i < triangleCount; ++i)
			{
				PxVec3 triVert[3];

				if(has16BitIndices)
				{
					triVert[0] = vertexBuffer[*shortIndices++];
					triVert[1] = vertexBuffer[*shortIndices++];
					triVert[2] = vertexBuffer[*shortIndices++];
				}
				else
				{
					triVert[0] = vertexBuffer[*intIndices++];
					triVert[1] = vertexBuffer[*intIndices++];
					triVert[2] = vertexBuffer[*intIndices++];
				}

				PxVec3 fnormal = (triVert[1] - triVert[0]).cross(triVert[2] - triVert[0]);
				fnormal.normalize();

				if(numTotalTriangles*6 < MAX_NUM_MESH_VEC3S)
				{
					gVertexBuffer[numTotalTriangles*6 + 0] = fnormal;
					gVertexBuffer[numTotalTriangles*6 + 1] = triVert[0];
					gVertexBuffer[numTotalTriangles*6 + 2] = fnormal;
					gVertexBuffer[numTotalTriangles*6 + 3] = triVert[1];
					gVertexBuffer[numTotalTriangles*6 + 4] = fnormal;
					gVertexBuffer[numTotalTriangles*6 + 5] = triVert[2];
					numTotalTriangles++;
				}
			}
			glPushMatrix();
			glScalef(scale.x, scale.y, scale.z);
			glEnableClientState(GL_NORMAL_ARRAY);
			glEnableClientState(GL_VERTEX_ARRAY);
			glNormalPointer(GL_FLOAT, 2*3*sizeof(float), gVertexBuffer);
			glVertexPointer(3, GL_FLOAT, 2*3*sizeof(float), gVertexBuffer+1);
			glDrawArrays(GL_TRIANGLES, 0, int(numTotalTriangles * 3));
			glDisableClientState(GL_VERTEX_ARRAY);
			glDisableClientState(GL_NORMAL_ARRAY);
			glPopMatrix();
		}
		break;
	case PxGeometryType::eINVALID:
	case PxGeometryType::eHEIGHTFIELD:
	case PxGeometryType::eGEOMETRY_COUNT:	
	case PxGeometryType::ePLANE:/*这里设置了平面，给了几个点让glut画了个正方形，颜色设置为120的绿色（满值是255）*/
		glBegin(GL_QUADS);
		glColor4ub(170, 255, 0, 200); glVertex3f(-1.0f, -300.0f, -300.0f);/*这个坐标有点奇怪 后面应该是变换过，不用太在意，反正就当第一个是y轴就可以了*/
		glColor4ub(170, 255, 0, 200); glVertex3f(-1.0f, -300.0f, 300.0f);
		glColor4ub(170, 255, 0, 200); glVertex3f(-1.0f, 300.0f, 300.0f);
		glColor4ub(170, 255, 0, 200); glVertex3f(-1.0f, 300.0f, -300.0f);
		glEnd();
		break;
	}
}

//Imgui渲染具体内容
void my_display_code()
{
	if (main_window)
	{

		static int counter = 0;

		ImGui::Begin("Console",&main_window);                          // Create a window called "Hello, world!" and append into it.

		ImGui::Text("setting:");      
		ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
		ImGui::Checkbox("Game Object Settings Window", &show_another_window);// Display some text (you can use a format strings too)
		ImGui::Checkbox("backgroundMusic", &backgroundMusic);
		ImGui::SliderFloat("backgroundMusicVolume", &volume0, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::Checkbox("soundEffect", &soundEffect);
		ImGui::SliderFloat("soundEffectVolume", &volume1, 0.0f, 1.0f);
		ImGui::ColorEdit3("set color", (float*)&clear_color); // Edit 3 floats representing a color

		if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			editState = true;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		ImGui::Checkbox("IsSimulation", &isSimulation);

		//创建GameObject
		if (ImGui::Button("CreateNewGameObject",
			ImVec2(ImGui::GetContentRegionAvail().x, 20)))
		{
			curGameObject = nullptr;
			theCreator.CreateNewGameObject();
		}

		ImGui::End();
	}
	if (show_another_window)
	{
		ImGui::Begin("Hierarchy", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		//ImGui::Text("Here are some game object settings below:");

		//GameObject列表
		
		for (int i = 0; i < theCreator.SceneGameObject.size(); i++)
		{
			ImGui::PushID(i);
			if (ImGui::Button(theCreator.SceneGameObject[i].Name,
				ImVec2(ImGui::GetContentRegionAvail().x,20)))
			{
				//GameObject
				curGameObject = &theCreator.SceneGameObject[i];

				//Name
				char* tempName = curGameObject->Name;
				strcpy(_objName, tempName);

			}
			ImGui::PopID();
		}
		//const char* items[] = { "AAAA", "BBBB", "CCCC", "DDDD", "EEEE", "FFFF", "GGGG", "HHHH", "IIIIIII", "JJJJ", "KKKKKKK" };
		//const char** point = items;
		//static int item_current = 0;

		//ImGui::Combo("combo", &item_current, point, IM_ARRAYSIZE(items));

		
		//ImGui::InputFloat3("position(x, y, z)", gameObjectPosition);

		//if (ImGui::Button("Close the window"))
		//	show_another_window = false;
		ImGui::End();
	}
	if (inspector_window)
	{
		ImGui::Begin("Inspector", &inspector_window);


		if (curGameObject != nullptr)
		{
			//更新数据

			//Position
			_position= curGameObject->transform.p;

			//Rotation
			_rotation = Mathf::QuatToEuler(curGameObject->transform.q);
			//_quaternion = curGameObject->transform.q;

			//UI-Begin
			ImGui::Text("Name");
			ImGui::InputTextWithHint(" ", "input GameObject name", _objName,
				IM_ARRAYSIZE(_objName));

			ImGui::Text("Position");
			ImGui::PushItemWidth((ImGui::GetContentRegionAvail().x)/3-33);
			ImGui::PushID(1);
			ImGui::DragFloat("x", &_position.x, 0.01);
			ImGui::SameLine();
			ImGui::DragFloat("y", &_position.y, 0.01);
			ImGui::SameLine();
			ImGui::DragFloat("z", &_position.z, 0.01);
			ImGui::PopID();

			ImGui::Text("Rotation");
			ImGui::PushItemWidth((ImGui::GetContentRegionAvail().x) / 3 - 33);
			ImGui::PushID(2);
			ImGui::DragFloat("x", &_rotation.x, 0.1);
			ImGui::SameLine();
			ImGui::DragFloat("y", &_rotation.y, 0.1);
			ImGui::SameLine();
			ImGui::DragFloat("z", &_rotation.z, 0.1);
			ImGui::PopID();
			
			ImGui::Spacing();
			ImGui::Text("Components");
			ImGui::Spacing();


			//组件UI
			for (int i = 0; i < curGameObject->components.size(); i++)
			{
				curGameObject->components[i]->ShowParameter();
			}


			//添加刚体
			if (ImGui::Button("AddRigidbody",
				ImVec2(ImGui::GetContentRegionAvail().x-100, 20)))
			{
				RigidBody* newRigidBody = new RigidBody(curGameObject,_isAddRigidbodyStatic);
			}
			ImGui::SameLine();
			ImGui::Checkbox("isStatic", &_isAddRigidbodyStatic);

			//添加Collider
			if (ImGui::Button("AddBoxCollider", ImVec2(ImGui::GetContentRegionAvail().x - 100, 20)))
			{
				BoxCollider* newBoxCollider = new BoxCollider(curGameObject);
			}

			//添加Model
			if (ImGui::Button("AddModel", ImVec2(ImGui::GetContentRegionAvail().x - 100, 20)))
			{
				ModelComponent* newModelComponent = new ModelComponent(curGameObject);
			}

			//UI-End
			if (curGameObject != nullptr)
			{
				curGameObject->SetTransform(PxTransform(_position,Mathf::EulerToQuat(_rotation)));
				char* str = _objName;
				strcpy(curGameObject->Name, str);
			}


			ImGui::PopItemWidth();
		}

		ImGui::End();
	}
	if (show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);
}
namespace Snippets
{

void glut_display_func()
	{
		// Start the Dear ImGui frame
		ImGui_ImplOpenGL2_NewFrame();
		ImGui_ImplGLUT_NewFrame();

		my_display_code();

		// Rendering
		ImGui::Render();
		ImGuiIO& io = ImGui::GetIO();
		//glViewport(0, 0, (GLsizei)io.DisplaySize.x, (GLsizei)io.DisplaySize.y);
		//glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
		//glClear(GL_COLOR_BUFFER_BIT);
		//glUseProgram(0); // You may want this if using this code in an OpenGL 3+ context where shaders may be bound, but prefer using the GL3+ code.
		ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

		glutSwapBuffers();
		glutPostRedisplay();
		
	}

void changeSize(int w, int h) {

	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if (h == 0)
		h = 1;
	float ratio = 1.0 * w / h;

	// Use the Projection Matrix
	glMatrixMode(GL_PROJECTION);

	// Reset Matrix
	glLoadIdentity();

	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);

	// Set the correct perspective.
	gluPerspective(45, ratio, 0.1, 1000);

	// Get Back to the Modelview
	glMatrixMode(GL_MODELVIEW);
}


void setupDefaultWindow(const char *name)
{
	char* namestr = new char[strlen(name)+1];
	strcpy(namestr, name);
	int argc = 1;
	char* argv[1] = { namestr };

	
	//初始化glut
	glutInit(&argc, argv);
	//显示模式
	glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE|GLUT_DEPTH);

	//glutGameModeString("1920x1080");
	//glutEnterGameMode();
	//glutInitWindowPosition(960, 0);
	glutInitWindowSize(1920, 1080);


	int mainHandle = glutCreateWindow(name);
	glutSetWindow(mainHandle);
	//glutFullScreen();

	//glutCreateWindow(name);
	//glutFullScreen();



	//渲染窗口尺寸和位置
	glutReshapeFunc(changeSize);
	delete[] namestr;
}



void setupDefaultRenderState()
{
	// Setup default render states 初始化设置
	glClearColor(1.0f, 1.0f, 0.86f, 1.0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);

	// Setup lighting 光照设置
	glEnable(GL_LIGHTING);
	PxReal ambientColor[]	= { 0.0f, 0.1f, 0.2f, 0.0f };
	PxReal diffuseColor[]	= { 0.1f, 0.1f, 0.1f, 0.0f };
	PxReal specularColor[]	= { 0.0f, 0.0f, 0.0f, 0.0f };		
	PxReal position[]		= { 100.0f, 100.0f, 400.0f, 1.0f };		
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientColor);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseColor);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularColor);
	glLightfv(GL_LIGHT0, GL_POSITION, position);
	glEnable(GL_LIGHT0);
}


void startRender(const PxVec3& cameraEye, const PxVec3& cameraDir, PxReal clipNear, PxReal clipFar)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	

	// Setup camera
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, GLdouble(glutGet(GLUT_WINDOW_WIDTH)) / GLdouble(glutGet(GLUT_WINDOW_HEIGHT)), GLdouble(clipNear), GLdouble(clipFar));

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//看向相机的方向
	gluLookAt(GLdouble(cameraEye.x), GLdouble(cameraEye.y), GLdouble(cameraEye.z), GLdouble(cameraEye.x + cameraDir.x), GLdouble(cameraEye.y + cameraDir.y), GLdouble(cameraEye.z + cameraDir.z), 0.0, 1.0, 0.0);

	glColor4f(0.4f, 0.4f, 0.4f, 1.0f);
}

void renderGameOver(const char text[], int len) 
	{
		renderText(190,250,text,len);
	}


//渲染Actors
void renderActors(PxRigidActor** actors, const PxU32 numActors, bool shadows, const PxVec3 & color)/*渲染actor*/
{
	PxShape* shapes[MAX_NUM_ACTOR_SHAPES];
	for(PxU32 i=0;i<numActors;i++)
	{
		const PxU32 nbShapes = actors[i]->getNbShapes();
		PX_ASSERT(nbShapes <= MAX_NUM_ACTOR_SHAPES);
		actors[i]->getShapes(shapes, nbShapes);
		bool sleeping = actors[i]->is<PxRigidDynamic>() ? actors[i]->is<PxRigidDynamic>()->isSleeping() : false; 
		/*这里以后会用到来判断每一杆的行动机会，sleeping则可以行动，不是sleeping状态说明还在运动*/

		for(PxU32 j=0;j<nbShapes;j++)
		{
			const PxMat44 shapePose(PxShapeExt::getGlobalPose(*shapes[j], *actors[i]));
			PxGeometryHolder h = shapes[j]->getGeometry();

			//if (shapes[j]->getFlags() & PxShapeFlag::eTRIGGER_SHAPE)
			glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
			
			// render object
			glPushMatrix();						
			glMultMatrixf(reinterpret_cast<const float*>(&shapePose));
			if(sleeping)/*是否处于sleeping状态*/
			{
				PxVec3 darkColor = color * 0.5f;/*sleeping的情况下颜色*0.8*/
				glColor4f(darkColor.x, darkColor.y, darkColor.z, 1.0f);
			}
			else
				glColor4f(color.x, color.y, color.z, 1.0f);
			renderGeometry(h);
			glPopMatrix();

			glPolygonMode( GL_FRONT_AND_BACK, GL_FILL);

			if(shadows)/*阴影，，，效果表现上有瑕疵但不知道怎么优化*/
			{
				const PxVec3 shadowDir(0.0f, -0.7071067f, -0.7071067f);
				const PxReal shadowMat[]={ 1,0,0,0, -shadowDir.x/shadowDir.y,0,-shadowDir.z/shadowDir.y,0, 0,0,1,0, 0,0,0,1 };
				glPushMatrix();						
				glMultMatrixf(shadowMat);
				glMultMatrixf(reinterpret_cast<const float*>(&shapePose));
				glDisable(GL_LIGHTING);
				glColor4f(0.1f, 0.2f, 0.3f, 1.0f);
				renderGeometry(h);
				glEnable(GL_LIGHTING);
				glPopMatrix();
			}
		}
	}
}

void finishRender()
{
	//glutSwapBuffers();
	
}

void renderText(int x, int y, const char text[], int len)
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, GLUT_WINDOW_WIDTH, 0, GLUT_WINDOW_HEIGHT);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glLoadIdentity();
	glRasterPos2i(x, y);
	for (int i = 0; i < len; i++) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, text[i]);

	}
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

}

} //namespace Snippets

