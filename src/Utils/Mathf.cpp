#include "Mathf.h"
float Mathf::DegToRad(float deg)
{
	return PxPi/180*deg;
}

float Mathf::RadToDeg(float rad)
{
	return 180/PxPi*rad;
}

//PxVec3 Mathf::QuatToEuler(PxQuat q)
//{
//	//PxVec3 result;
//	//result.x = q.getBasisVector0();
//	return PxVec3();
//}

//PxQuat Mathf::EulerToQuat(PxVec3 vec)
//{
//	PxQuat q;
//	q.x = PxSin(vec.y / 2) * PxSin(vec.z / 2) * PxCos(vec.x / 2) + PxCos(vec.y / 2) * PxCos(vec.z / 2) * PxSin(vec.x / 2);
//	q.y = PxSin(vec.y / 2) * PxCos(vec.z / 2) * PxCos(vec.x / 2) + PxCos(vec.y / 2) * PxSin(vec.z / 2) * PxSin(vec.x / 2);
//	q.z = PxCos(vec.y / 2) * PxSin(vec.z / 2) * PxCos(vec.x / 2) - PxSin(vec.y / 2) * PxCos(vec.z / 2) * PxSin(vec.x / 2);
//	q.w = PxCos(vec.y / 2) * PxCos(vec.z / 2) * PxCos(vec.x / 2) - PxSin(vec.y / 2) * PxSin(vec.z / 2) * PxSin(vec.x / 2);
//	return q;
//}

void Mathf::Debug(PxVec3 vec)
{
	cout << "(" << vec.x << "," << vec.y << "," << vec.z << ")" << endl;
}

void Mathf::Debug(PxQuat q)
{
	cout << "(" << q.x << "," << q.y << "," << q.z <<","<<q.w<< ")" << endl;
}

//两向量的弧度
float Mathf::Angle(PxVec3 from, PxVec3 to)
{

	return 	PxAcos(from.dot(to) / Distance(from) / Distance(to));
}


float Mathf::Distance(PxVec3 vec)
{
	return sqrt(PxPow(vec.x,2)+ PxPow(vec.y,2)+ PxPow(vec.z,2));
}

