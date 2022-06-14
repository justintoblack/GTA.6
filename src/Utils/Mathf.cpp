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

//弧度
float Mathf::Angle(PxVec3 from, PxVec3 to)
{
	//acos���ز������ŵ�ֵ
	return 	PxAcos(from.dot(to) / Distance(from) / Distance(to));
}


float Mathf::Distance(PxVec3 vec)
{
	return sqrt(PxPow(vec.x,2)+ PxPow(vec.y,2)+ PxPow(vec.z,2));
}

//线性插值
float Mathf::Lerp(float a, float b, float t)
{
	return (1-t)*a+t*b;
}

PxVec3 Mathf::Lerp(PxVec3 a, PxVec3 b, float t)
{
	return (1 - t) * a + t * b;
}

PxQuat Mathf::Lerp(PxQuat a, PxQuat b, float t)
{
	return (a* (1 - t) +b*t).getNormalized();
}



//球面线性插值
glm::vec3 Mathf::Slerp(glm::vec3 a, glm::vec3 b, float t)
{
	a = glm::normalize(a);
	b = glm::normalize(b);
	float angle = PxAcos(glm::dot(a, b));
	angle = glm::clamp(angle, 0.001f,3.141f);
	cout << angle << endl;
	glm::vec3 result = (sin((1 - t) * angle) / sin(angle)) * a +
		(sin(t * angle) / sin(angle)) * b;
	return glm::normalize(result);
}

PxVec3 Mathf::Slerp(PxVec3 a, PxVec3 b, float t)
{
	//normalizeFast is ok
	a.normalize();
	b.normalize();
	float angle = Angle(a, b);
	angle = glm::clamp(angle, 0.001f, 3.141f);
	PxVec3 result = (PxSin((1 - t) * angle) / PxSin(angle)) * a +
		(PxSin(t * angle) / PxSin(angle)) * b;

	return result;
}

PxQuat Mathf::Slerp(PxQuat a, PxQuat b, float t)
{
	a.normalize();
	b.normalize();
	float angle = PxAcos(a.dot(b));
	angle = glm::clamp(angle, 0.001f, 3.141f);
	PxQuat result =a* (PxSin((1 - t) * angle) / PxSin(angle)) +
		b*(PxSin(t * angle) / PxSin(angle));

	return result;
}

