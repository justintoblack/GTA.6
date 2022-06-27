#include "TheCreator.h"

using namespace snippetvehicle;
extern const char** _allModelsName;
extern void FileToString(string& path);
vector<Model> Models;			//模型

PxQuat q0 = PxQuat(PxIdentity);
PxQuat q1 = PxQuat(Mathf::DegToRad(90), PxVec3(0, -1, 0));
PxQuat q2 = PxQuat(Mathf::DegToRad(180), PxVec3(0, -1, 0));
PxQuat q3 = PxQuat(Mathf::DegToRad(270), PxVec3(0, -1, 0));
PxTransform polesPos[] =
{
	PxTransform(PxVec3(1.8,3.1,-110),q1),PxTransform(PxVec3(1.8,3.1,-125),q1),PxTransform(PxVec3( 1.8,3.1,0),q1),
	PxTransform(PxVec3(1.8,3.1,15),q1),PxTransform(PxVec3( 1.8,3.1,30),q1),PxTransform(PxVec3(1.8,3.1,50),q1),
	PxTransform(PxVec3(1.8,3.1,85),q1),PxTransform(PxVec3(1.8,3.1,100),q1),PxTransform(PxVec3(1.8,3.1,115),q1),
	PxTransform(PxVec3(1.8,3.1,130),q1),PxTransform(PxVec3(1.8,3.1,-80),q1),PxTransform(PxVec3(1.8,3.1,-50),q1),
	PxTransform(PxVec3(-11.5,3.1,-110),q3),PxTransform(PxVec3(-11.5,3.1,-125),q3),PxTransform(PxVec3(-11.5,3.1,0),q3),
	PxTransform(PxVec3(-11.5,3.1,15),q3),PxTransform(PxVec3(-11.5,3.1,30),q3),PxTransform(PxVec3(-11.5,3.1,50),q3),
	PxTransform(PxVec3(-11.5,3.1,85),q3),PxTransform(PxVec3(-11.5,3.1,100),q3),PxTransform(PxVec3(-11.5,3.1,115),q3),
	PxTransform(PxVec3(-11.5,3.1,130),q3),PxTransform(PxVec3(-11.5,3.1,-80),q3),PxTransform(PxVec3(-11.5,3.1,-50),q3),

	PxTransform(PxVec3(-68,3.1,-75),q1),PxTransform(PxVec3(-68,3.1,-60),q1),PxTransform(PxVec3(-81.6,3.1,-75),q3),
	PxTransform(PxVec3(-81.6,3.1,-60),q3),PxTransform(PxVec3(-108,3.1,-70),q1),PxTransform(PxVec3(-108,3.1,-46),q1),
	PxTransform(PxVec3(-121,3.1,-70),q3),PxTransform(PxVec3(-121,3.1,-46),q3),PxTransform(PxVec3(-146.5,3.1,-32.6),q3),
	PxTransform(PxVec3(-146.5,3.1,-17.6),q3),PxTransform(PxVec3(-151.4,3.1,-65.2),q3),PxTransform(PxVec3(-151.4,3.1,-90.2),q3),

	PxTransform(PxVec3(106.7,3.1,48.3),q1),PxTransform(PxVec3(106.7,3.1,28.3),q1),PxTransform(PxVec3(106.7,3.1,8.3),q1),
	PxTransform(PxVec3(106.7,3.1,-12),q1),PxTransform(PxVec3(-133.3,3.1,-8),q3),PxTransform(PxVec3(-133.3,3.1,-7),q3),
	PxTransform(PxVec3(-133.3,3.1,27),q3),PxTransform(PxVec3(-133.3,3.1,47),q3),PxTransform(PxVec3(-133.3,3.1,67),q3),
};

//可行使
void makeObjectDrivable(PxShape*& shape)
{

	PxFilterData simFilterData(COLLISION_FLAG_OBSTACLE, COLLISION_FLAG_WHEEL, PxPairFlag::eMODIFY_CONTACTS | PxPairFlag::eDETECT_CCD_CONTACT, 0);
	shape->setSimulationFilterData(simFilterData);
	PxFilterData qryFilterData;
	setupDrivableSurface(qryFilterData);
	shape->setQueryFilterData(qryFilterData);
}

void TheCreator::createSlowArea(PxVec3 startPosition, PxF32 capsuleRadii, PxF32 interval, PxU32 num, PxMaterial* gMaterial)
{
	//Create several static obstacles for the first vehicle to drive on.
	//  (i) collide only with wheel shapes
	// (ii) have continuous collision detection (CCD) enabled
	//(iii) have contact modification enabled
	// (iv) are configured to be drivable surfaces
	for (PxU32 i = 0; i < num; i++)
	{
		PxF32 zPosition = startPosition.z + i * interval;
		PxTransform t(PxVec3(startPosition.x, capsuleRadii, zPosition), PxQuat(PxIdentity));
		PxRigidStatic* rd = m_gPhysics->createRigidStatic(t);
		PxCapsuleGeometry capsuleGeom(capsuleRadii, 3.0f); 
		PxBoxGeometry box(10, 0.1, capsuleRadii);
		PxShape* shape = PxRigidActorExt::createExclusiveShape(*rd, box, *gMaterial);
		makeObjectDrivable(shape);
		m_gScene->addActor(*rd);
	}
}

void TheCreator::Init(PxPhysics* physics, PxScene* gScene)
{
	m_gPhysics = physics;
	m_gScene = gScene;

	//poleModel = Model("../../assets/objects/Models/01_SM_Prop_LightPole_Base_02.fbx");
	Street_Pole = Model("../../assets/objects/Models/47_Street_Pole.fbx");
	Traffic_light = Model("../../assets/objects/Models/46_Traffic_Pole.fbx");


	//加载模型
	string filePath = "../../assets/objects/Models";
	string format = ".fbx";
	GetAllFormatFiles(filePath, ModelPath, format);

	for (int i = 0; i < ModelPath.size(); i++)
	{
		Model temp = Model(ModelPath[i]);
		Models.push_back(temp);
		string name = ModelPath[i].substr(ModelPath[i].find_last_of('/')+1);
		ModelName.push_back(name);
	}
	 _modelsCount = ModelName.size();
	_allModelsName = new const char* [_modelsCount];

	 for (int i = 0; i < _modelsCount; i++)
	 {
		 _allModelsName[i] = ModelName[i].data();
		 cout << ModelName[i]<< endl;
	 }

	 //初始化场景
	 string scenePath = "../../assets/Scene/Scene.Data";
	 FileToString(scenePath);

	 //创建MonoBehaviour物体
	 Zombie *testZombie = new Zombie();
	 SpecialGameObject.push_back(testZombie);
	 for (int i = 0; i < SpecialGameObject.size(); i++)
	 {
		 SpecialGameObject[i]->Awake();
	 }

}

void TheCreator::CreateAnchorBall(PxTransform pos, PxMaterial* gMaterial,float radius)
{
	PxRigidStatic* ballBody = PxCreateStatic(*m_gPhysics, pos, PxSphereGeometry(radius), 
		*gMaterial,PxTransform(PxIdentity));
	m_gScene->addActor(*ballBody);
}

void TheCreator::CreateAnchorBox(PxTransform pos, PxMaterial* gMaterial, float halfX,float halfY,float halfZ)
{
	PxRigidStatic* ballBody = PxCreateStatic(*m_gPhysics, pos, PxBoxGeometry(halfX,halfY,halfZ),
		*gMaterial, PxTransform(PxIdentity));
	m_gScene->addActor(*ballBody);
}

void TheCreator::CreateDynamicSphere(PxTransform pos, PxMaterial* gMaterial, float radius)
{
	PxRigidDynamic* body = PxCreateDynamic(*m_gPhysics, pos,
		PxSphereGeometry(radius), *gMaterial, 1.0f);

	m_gScene->addActor(*body);
}

void TheCreator::CreateBanister(PxVec3 pos,PxVec3 dir,  PxMaterial* gMaterial,float halfHeight, 
	float halfLength, float density, float force, float torque,BreakType type)
{
	//单位化
	dir = dir.getNormalized();

	//旋转轴
	PxVec3 rotate = PxVec3(0, 0, 1).cross(dir).getNormalized();

	//旋转角度
	float angle = Mathf::Angle(PxVec3(0, 0, 1), dir);

	//旋转四元数
	//PxQuat q = PxQuat(Mathf::DegToRad( angle), rotate);
	
	PxRigidDynamic* link1 = PxCreateDynamic(*m_gPhysics,PxTransform(PxVec3(0,halfHeight,0)+pos), PxBoxGeometry(0.15f, halfHeight, 0.15f), *gMaterial, density);
	PxFixedJoint* j1 = PxFixedJointCreate(*m_gPhysics, NULL, PxTransform(pos), link1, PxTransform(0, -halfHeight, 0));

	PxRigidDynamic* link2 = PxCreateDynamic(*m_gPhysics, 
		PxTransform(PxVec3(0,halfHeight*2,0)+pos+dir*halfLength, PxQuat(angle, rotate)), PxBoxGeometry(0.15f, 0.15f, halfLength), *gMaterial, density);
	PxFixedJoint* j2 = PxFixedJointCreate(*m_gPhysics, link1, PxTransform(0, halfHeight, 0), link2,
		PxTransform(PxVec3(0, 0, -halfLength), PxQuat(angle, -rotate)));

	PxRigidDynamic* link3 = PxCreateDynamic(*m_gPhysics, PxTransform(pos+dir*halfLength*2+PxVec3(0,halfHeight,0)),
		PxBoxGeometry(0.15f, halfHeight, 0.15f), *gMaterial, density);
	PxFixedJoint* j3 = PxFixedJointCreate(*m_gPhysics, link2, 
		PxTransform(PxVec3( 0, 0, halfLength), PxQuat(angle, -rotate)), link3, PxTransform(0, halfHeight, 0));

	PxFixedJoint* j4 = PxFixedJointCreate(*m_gPhysics, link3, PxTransform(0, -halfHeight, 0), NULL,
		PxTransform(pos+dir*halfLength*2));

	if (type == AllInPiece)
	{
		j2->setBreakForce(force, torque);
	}
	j1->setBreakForce(force, torque);
	j3->setBreakForce(force, torque);
	j4->setBreakForce(force, torque);

	m_gScene->addActor(*link1);
	m_gScene->addActor(*link2);
	m_gScene->addActor(*link3);
}

//加入随机
void TheCreator::CreateBanisters(physx::PxVec3 pos, physx:: PxVec3 dir, PxMaterial* gMaterial,
	float separate, physx::PxU32 num, float halfHeight, float halfLength, float density, float force, 
	float torque)
{
	dir = dir.getNormalized();
	CreateBanister(pos,dir,gMaterial,halfHeight, halfLength, density, force, torque,SomeInPiece);

	srand((unsigned)time(NULL));
	for (PxU32 i = 1; i < num; i++)
	{
		BreakType type=SomeInPiece;

		//1/2概率
		int r= rand() %2 ;
		if (r == 0)
		{
			type = AllInPiece;
		}
		pos += dir*halfLength*2+dir*separate;
		CreateBanister(pos,dir, gMaterial, halfHeight, halfLength, density, force, torque,type);
	}
}

void TheCreator::CreatePole(PxVec3 pos, PxMaterial* gMaterial,float radius,
	float halfHeight,float density,float force,float torque,Model& Createmodel)
{
	GameObject* gameObject=new GameObject();
	gameObject->SetName("Pole");
	RigidBody* rig = new RigidBody(gameObject,false);
	BoxCollider* box = new BoxCollider(gameObject);
	box->Size = PxVec3(radius, halfHeight, radius);
	box->SetShape();
	ModelComponent* model = new ModelComponent(gameObject);
	model->SetModel(Createmodel);
	gameObject->SetTransform(PxTransform(PxVec3(pos)+PxVec3(0,halfHeight,0)));


	PxRigidDynamic* link = PxCreateDynamic(*m_gPhysics,
		PxTransform(pos + PxVec3(0, halfHeight, 0)),
		PxBoxGeometry(radius, halfHeight, radius),
		*gMaterial, density);

	PxFixedJoint* j = PxFixedJointCreate(*m_gPhysics, NULL, PxTransform(pos), gameObject->g_rigidBody,
		PxTransform(PxVec3(0,-halfHeight, 0)));

	j->setBreakForce(force, torque);

	////gScene->addActor(*link);
	SpecialGameObject.push_back(gameObject);
	gScene->addActor(*link);
	SceneGameObject.push_back(*gameObject);
}

void TheCreator::CreateStreetPole(PxTransform pos)
{
	GameObject* gameObject = new GameObject();
	gameObject->SetName("Pole");

	PxShape* shape = gPhysics->createShape(PxBoxGeometry(0.1, 3.1, 0.1), *gMaterial);
	PxRigidDynamic* link = PxCreateDynamic(*m_gPhysics, pos, *shape, 10);
	shape= gPhysics->createShape(PxBoxGeometry(0.1, 0.1, 1.1), *gMaterial);
	shape->setLocalPose(PxTransform(0, 3.3, 1.26));
	link->attachShape(*shape);
	PxFixedJoint* j = PxFixedJointCreate(*m_gPhysics, NULL, PxTransform(PxVec3(pos.p)-PxVec3(0,3.1,0),pos.q), link,
		PxTransform(PxVec3(0, -3.1, 0)));
	j->setBreakForce(100, 100);
	RigidBody* rig = new RigidBody(gameObject, link);
	ModelComponent* model = new ModelComponent(gameObject);
	model->SetModel(Street_Pole);
	gameObject->SetTransform(pos);
	SceneGameObject.push_back(*gameObject);
}

void TheCreator::CreatePoles(PxVec3 pos, PxVec3 dir, float separate, PxU32 num, PxMaterial* gMaterial, float halfXZ, float halfHeight, float density, float force, float torque, Model& Createmodel)
{
	dir = dir.getNormalized();
	CreatePole(pos, gMaterial, halfXZ, halfHeight, density, force, torque,Createmodel);

	for (int i = 1; i < num; i++)
	{
		pos += dir * separate;
		CreatePole(pos, gMaterial, halfXZ, halfHeight, density, force, torque,Createmodel);
	}
}
extern  float gameObjectPosition[3];
void TheCreator::CreateGameObject()
{
	for (int i = 0; i < 45; i++)
	{
		CreateStreetPole(polesPos[i]);
	}
}

void TheCreator::CreatePaticle(PxVec3 pos)
{
}

void TheCreator::CreateNewGameObject()
{
	GameObject* gameObject=new GameObject();
	SceneGameObject.push_back(*gameObject);
}

void TheCreator::DuplicateGameObject(GameObject* gameObject)
{
	GameObject newGameObject;
	GameObject other=*gameObject;
	newGameObject = other;
	SceneGameObject.push_back(newGameObject);
}









































































