#include "TheCreator.h"

void TheCreator::Init(PxPhysics* physics, PxScene* gScene)
{
	m_gPhysics = physics;
	m_gScene = gScene;
}

void TheCreator::CreateBanister(PxTransform pos, PxMaterial* gMaterial,float halfHeight, float halfLength, float density, float force, float torque)
{
	PxRigidDynamic* link1 = PxCreateDynamic(*m_gPhysics, pos * PxTransform(0, halfHeight, 0), PxBoxGeometry(0.5f, halfHeight, 0.5f), *gMaterial, density);
	PxFixedJoint* j1 = PxFixedJointCreate(*m_gPhysics, NULL, pos, link1, PxTransform(0, -halfHeight, 0));
	j1->setBreakForce(force, torque);

	pos = pos * PxTransform(0, halfHeight * 2, halfLength);
	PxRigidDynamic* link2 = PxCreateDynamic(*m_gPhysics, pos, PxBoxGeometry(0.5f, 0.5f, halfLength), *gMaterial, density);
	PxFixedJoint* j2 = PxFixedJointCreate(*m_gPhysics, link1, PxTransform(0, halfHeight, 0), link2, PxTransform(0, 0, -halfLength));
	j2->setBreakForce(force, torque);

	pos = pos * PxTransform(0, -halfHeight, halfLength);
	PxRigidDynamic* link3 = PxCreateDynamic(*m_gPhysics, pos, PxBoxGeometry(0.5f, halfHeight, 0.5f), *gMaterial, density);
	PxFixedJoint* j3 = PxFixedJointCreate(*m_gPhysics, link2, PxTransform(0, 0, halfLength), link3, PxTransform(0, halfHeight, 0));

	PxFixedJoint* j4 = PxFixedJointCreate(*m_gPhysics, link3, PxTransform(0, -halfHeight, 0), NULL, pos * PxTransform(0, -halfHeight, 0));
	j4->setBreakForce(force, torque);

	m_gScene->addActor(*link1);
	m_gScene->addActor(*link2);
	m_gScene->addActor(*link3);
}

void TheCreator::CreateBanisters(physx::PxTransform pos, PxMaterial* gMaterial, float separate, physx::PxU32 num, float halfHeight, float halfLength, float density, float force, float torque)
{
	CreateBanister(pos,gMaterial ,halfHeight, halfLength, density, force, torque);
	for (PxU32 i = 1; i < num; i++)
	{
		pos = PxTransform(0, 0, separate + halfLength * 2) * pos;
		CreateBanister(pos, gMaterial, halfHeight, halfLength, density, force, torque);
	}
}
