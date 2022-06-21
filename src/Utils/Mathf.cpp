#include "Mathf.h"

float Mathf::DegToRad(float deg)
{
	return PxPi/180*deg;
}

float Mathf::RadToDeg(float rad)
{
	return 180/PxPi*rad;
}

glm::vec3 Mathf::P3ToV3(PxVec3 a)
{
	return glm::vec3(a.x,a.y,a.z);
}

PxVec3 Mathf::V3ToP3(glm::vec3 a)
{
	return PxVec3(a.x,a.y,a.z);
}

glm::quat Mathf::Toquat(PxQuat q)
{
	return glm::quat(q.w,q.x,q.y,q.z);
}

PxVec3 Mathf::QuatToEuler(PxQuat q)
{
	PxVec3 euler;
	euler.x = Mathf::RadToDeg(atan2(2 * (q.w * q.x + q.y * q.z), 1 - 2 * (q.y * q.y + q.x + q.x)));

	float sinp = 2 * (q.w * q.y - q.z * q.x);
	if (abs(sinp) >= 1)
	{
		euler.y = Mathf::RadToDeg(copysign(PxPi/2,sinp));
	}
	else
	{
		euler.y = Mathf::RadToDeg(std::asin(sinp));
	}

	euler.z = Mathf::RadToDeg(atan2(2 * (q.w * q.z + q.x * q.y), 1 - 2 * (q.y * q.y + q.z * q.z)));
	return euler;
}

PxQuat Mathf::EulerToQuat(PxVec3 vec)
{
	//q *= PxQuat(Mathf::DegToRad(vec.y), PxVec3(0, 1, 0));
	//q*= PxQuat(Mathf::DegToRad(vec.z), PxVec3(0, 0, 1));
	PxQuat q;

	vec = PxVec3(Mathf::DegToRad(vec.x), Mathf::DegToRad(vec.y), 
		Mathf::DegToRad(vec.z));
	double cy = cos(vec.z * 0.5);
	double sy = sin(vec.z * 0.5);
	double cp = cos(vec.y * 0.5);
	double sp = sin(vec.y * 0.5);
	double cr = cos(vec.x * 0.5);
	double sr = sin(vec.x * 0.5);

	////笛卡尔坐标系
	//q.w = cr * cp * cy + sr * sp * sy;
	//q.x = sr * cp * cy - cr * sp * sy;
	//q.y = cr * sp * cy + sr * cp * sy;
	//q.z = cr * cp * sy - sr * sp * cy;

	////Direct3D

	q.w = cy * cp * cr + sy * sp * sr;
	q.x = cy * cp * sr - sy * sp * cr;
	q.y = sy * cp * sr + cy * sp * cr;
	q.z = sy * cp * cr - cy * sp * sr;

	return q;
}


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

	return 	PxAcos(from.dot(to) / from.magnitude() / to.magnitude());
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

PxVec3 Mathf::ModelToWorld(PxVec3 WorldPosition, PxVec3 LocalPosition, PxQuat WorldRotation)
{

	return PxVec3();
}

