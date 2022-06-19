#include "TheCreator.h"

using namespace snippetvehicle;

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
		PxShape* shape = PxRigidActorExt::createExclusiveShape(*rd, capsuleGeom, *gMaterial);
		makeObjectDrivable(shape);
		m_gScene->addActor(*rd);
	}
}

void TheCreator::Init(PxPhysics* physics, PxScene* gScene)
{
	m_gPhysics = physics;
	m_gScene = gScene;

	poleModel = Model("../../assets/objects/Models/SM_Prop_LightPole_Base_02.fbx");
	stationModel = Model("../../assets/objects/Models/SM_Bld_Station_01.fbx");
	stationModel_01 = Model("../../assets/objects/Models/SM_Bld_Station_03.fbx");
	road = Model("../../assets/objects/Models/SM_Env_Road_Bare_01.fbx");
	_carBody=Model("../../assets/objects/Models/carBody.fbx");
	_carWheelLeft=Model("../../assets/objects/Models/wheel_left.fbx");
	_carWheelRight=Model("../../assets/objects/Models/wheel_right.fbx");

	string filePath = "../../assets/objects/Models";
	string format = ".fbx";
	GetAllFormatFiles(filePath, ModelPath, format);

	for (int i = 0; i < ModelPath.size(); i++)
	{
		Model temp = Model(ModelPath[i]);
		Models.push_back(temp);
	}
	for (int i = 0; i < ModelPath.size(); i++)
	{
		int end = ModelPath[i].find_last_of('.');
		cout << end<<endl;
		string name = ModelPath[i].substr(ModelPath[i].find_last_of("/") + 1, ModelPath[i].find_last_of('.') );
		name = name.substr(0, name.find_last_of('.'));
		cout << name << endl;
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
	float halfHeight,float density,float force,float torque)
{
	GameObject gameObject;
	char objectName[] = "Pole";
	strcpy(gameObject.Name,objectName);
	gameObject.AddRigidbody(true);
	gameObject.AddBoxCollider(radius, halfHeight, radius, PxTransform(0, 0, 0));
	gameObject.AddModel(poleModel);
	gameObject.SetTransform(PxTransform(PxVec3(pos)+PxVec3(0,halfHeight,0)));
	gameObject.AddToScene();


	//PxRigidDynamic* link = PxCreateDynamic(*m_gPhysics,
	//	PxTransform(pos + PxVec3(0, halfHeight, 0)),
	//	PxBoxGeometry(radius, halfHeight, radius),
	//	*gMaterial, density);

	PxFixedJoint* j = PxFixedJointCreate(*m_gPhysics, NULL, PxTransform(pos), gameObject.g_rigidBody,
		PxTransform(PxVec3(0,-halfHeight, 0)));

	j->setBreakForce(force, torque);

	////gScene->addActor(*link);
	SceneGameObject.push_back(gameObject);
}

void TheCreator::CreatePoles(PxVec3 pos, PxVec3 dir, float separate, PxU32 num, PxMaterial* gMaterial, float halfXZ, float halfHeight, float density, float force, float torque)
{
	dir = dir.getNormalized();
	CreatePole(pos, gMaterial, halfXZ, halfHeight, density, force, torque);

	for (int i = 1; i < num; i++)
	{
		pos += dir * separate;
		CreatePole(pos, gMaterial, halfXZ, halfHeight, density, force, torque);
	}
}
extern  float gameObjectPosition[3];
void TheCreator::CreateGameObject()
{
	GameObject tempObject;
	char objectName[] = "station_00";
	strcpy(tempObject.Name, objectName);
	tempObject.AddRigidbody(false);
	tempObject.AddModel(stationModel);
	tempObject.AddBoxCollider(4.35f, 4.25f, 4.6f, PxTransform(0, 4.29f, 0));
	tempObject.SetTransform(PxTransform(20, 0, 10));
	tempObject.AddToScene();

	SceneGameObject.push_back(tempObject);

	char objectName1[] = "station_01";
	strcpy(tempObject.Name, objectName1);
	tempObject.AddRigidbody(false);
	tempObject.AddModel(stationModel_01);
	tempObject.AddBoxCollider(5.38f, 2.87f, 2.95f, PxTransform(0, 2.87f, 0));
	tempObject.SetTransform(PxTransform(29, 0, 10));
	tempObject.AddToScene();

	SceneGameObject.push_back(tempObject);

	char objectName2[] = "station_02";
	strcpy(tempObject.Name, objectName2);
	tempObject.AddRigidbody(false);
	tempObject.AddModel(stationModel_01);
	tempObject.AddBoxCollider(5.38f, 2.87f, 2.95f, PxTransform(0, 2.87f, 0));
	tempObject.SetTransform(PxTransform(11, 0, 10));
	tempObject.AddToScene();

	SceneGameObject.push_back(tempObject);

	//////////////////////////Test/////////////////////////////
	GameObject* test = new GameObject();
	RigidBody* rigidbody = new RigidBody(test);
	BoxCollider* boxCollider = new BoxCollider(test);
	test->SetName("123");

	test->AddToScene();
	ModelComponent* modelComponent = new ModelComponent(test);
	modelComponent->SetModel(stationModel_01);
	SceneGameObject.push_back(*test);
	//////////////////////////Test-End/////////////////////////////
}

void TheCreator::CreateNewGameObject()
{
	GameObject* gameObject=new GameObject();
	SceneGameObject.push_back(*gameObject);
}









































































