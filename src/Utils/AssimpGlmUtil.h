#pragma once
#include <glm/glm.hpp>
#include <assimp/matrix4x4.h>
#include <assimp/quaternion.h>
#include <glm/gtx/quaternion.hpp>
class AssimpGlmUtil {
public:
	static glm::mat4 ConvertAiMatrix44ToGlmMat4(aiMatrix4x4 mat)
	{
		glm::mat4 res;
		res[0][0] = mat.a1; res[1][0] = mat.a2; res[2][0] = mat.a3; res[3][0] = mat.a4;
		res[0][1] = mat.b1; res[1][1] = mat.b2; res[2][1] = mat.b3; res[3][1] = mat.b4;
		res[0][2] = mat.c1; res[1][2] = mat.c2; res[2][2] = mat.c3; res[3][2] = mat.c4;
		res[0][3] = mat.d1; res[1][3] = mat.d2; res[2][3] = mat.d3; res[3][3] = mat.d4;
		return res;
	}
	static glm::vec3 ConvertAiVec3DToGlmVec3(aiVector3D v)
	{
		return glm::vec3(v.x, v.y, v.z);
	}
	static glm::quat ConvertAiQuaternionToGlmQuat(aiQuaternion q) {
		return glm::quat(q.w, q.x, q.y, q.z);
	}
};