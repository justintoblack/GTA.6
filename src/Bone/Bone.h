#pragma once
#ifndef BONE_H
#define BONE_H

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <vector>
#include "../Utils/AssimpGlmUtil.h"

using namespace std;

struct BoneInfo
{
	int id;
	glm::mat4 offset;
};
// 存储关键帧Position
struct KeyPosition {
	float mTime;
	glm::vec3 mPosition;
};
// 存储关键帧旋转
struct KeyRotation {
	float mTime;
	glm::quat mRotation;
};
// 存储关键帧缩放
struct KeyScale {
	float mTime;
	glm::vec3 mScale;
};


class Bone {
public:
	//从动画通道中提取关键帧信息
	Bone(const string& name, int ID, const aiNodeAnim* channel)
	{
		this->mName = name;
		this->mID = ID;
		this->mLocalTransform = glm::mat4(1.0f);

		//提取关键帧位置Position
		mNumPosition = channel->mNumPositionKeys;
		for (int posIndex = 0; posIndex < mNumPosition; posIndex++)
		{
			KeyPosition keyPosition;
			aiVectorKey aiKeyPos = channel->mPositionKeys[posIndex];
			keyPosition.mTime = aiKeyPos.mTime;
			keyPosition.mPosition = AssimpGlmUtil::ConvertAiVec3DToGlmVec3(aiKeyPos.mValue);
			mPositionKeys.push_back(keyPosition);
		}

		//提取关键帧旋转
		mNumRotation = channel->mNumRotationKeys;
		for (int rotIndex = 0; rotIndex < mNumRotation; rotIndex++)
		{
			KeyRotation keyRotation;
			aiQuatKey aiKeyRotation = channel->mRotationKeys[rotIndex];
			keyRotation.mTime = aiKeyRotation.mTime;
			keyRotation.mRotation = AssimpGlmUtil::ConvertAiQuaternionToGlmQuat(aiKeyRotation.mValue);
			mRotationKeys.push_back(keyRotation);
		}

		//提取关键帧缩放
		mNumScale = channel->mNumScalingKeys;
		for (int scaleIndex = 0; scaleIndex < mNumScale; scaleIndex++)
		{
			KeyScale keyScale;
			aiVectorKey aiKeyScale = channel->mScalingKeys[scaleIndex];
			keyScale.mTime = aiKeyScale.mTime;
			keyScale.mScale = AssimpGlmUtil::ConvertAiVec3DToGlmVec3(aiKeyScale.mValue);
			mScaleKeys.push_back(keyScale);
		}
		/*cout << "mNumPosition " << mNumPosition << endl;
		cout << "mNumRotation " << mNumRotation << endl;
		cout << "mNumScale " << mNumScale << endl;*/
	}

	//先获取插值后的结果，相乘后更新LocalTransform矩阵
	void UpdateLocalTransform(float animationTime)
	{
		glm::mat4 translation = InterpolatePosition(animationTime);
		glm::mat4 rotation = InterpolateRotation(animationTime);
		glm::mat4 scale = InterpolateScale(animationTime);
		mLocalTransform = translation * rotation * scale;
	}
	//计算interpolation factor
	float GetFactorBetweenTwoTime(float time1, float time2, float animationTime)
	{
		float period1 = animationTime - time1;
		float totalTime = time2 - time1;
		return (float)(period1 / totalTime);
	}
	//添加了默认return 0
	//找出参数animationTime处于哪两个index的时间段内，返回二者中的前者
	int getPositionIndex(float animationTime)
	{
		for (int posIndex = 0; posIndex < mNumPosition - 1; posIndex++)
			if (animationTime < mPositionKeys[posIndex + 1].mTime)
				return posIndex;
		return 0;
	}

	//找出参数animationTime处于哪两个index的时间段内，返回二者中的前者
	int getRotationIndex(float animationTime)
	{
		for (int rotIndex = 0; rotIndex < mNumRotation - 1; rotIndex++)
			if (animationTime < mRotationKeys[rotIndex + 1].mTime)
				return rotIndex;
		return 0;
	}

	//找出参数animationTime处于哪两个index的时间段内，返回二者中的前者
	int getScaleIndex(float animationTime)
	{
		for (int scaleIndex = 0; scaleIndex < mNumScale - 1; scaleIndex++)
			if (animationTime < mScaleKeys[scaleIndex + 1].mTime)
				return scaleIndex;
		return 0;
	}

	//TRS
	glm::mat4 getLocalTransform()
	{
		return mLocalTransform;
	}
	string getName() const
	{
		return mName;
	}
	int getID()
	{
		return mID;
	}
private:
	//存储关键帧位置Position的数组
	vector<KeyPosition> mPositionKeys;
	//存储关键帧旋转Rotation的数组
	vector<KeyRotation> mRotationKeys;
	//存储关键帧缩放Scale的数组
	vector<KeyScale>	mScaleKeys;
	//KeyPosition的数量
	int mNumPosition;
	//KeyRotation的数量
	int mNumRotation;
	//KeyScale的数量
	int mNumScale;
	//translate rotate scale
	glm::mat4 mLocalTransform;
	string mName;
	int mID;

	glm::mat4 InterpolatePosition(float animationTime)
	{
		if (mNumPosition == 1)
			return glm::translate(glm::mat4(1.0f), mPositionKeys[0].mPosition);

		int animIndex = getPositionIndex(animationTime);
		//获取当前时间点的上一个和下一个关键帧
		KeyPosition lastKey = mPositionKeys[animIndex];
		KeyPosition nextKey = mPositionKeys[animIndex + 1];
		if (animIndex > mNumPosition - 1) {
			cout << "1=============" << animIndex << endl;
		}
		if (animIndex + 1 > mNumPosition - 1)
		{
			cout << "2================="<< animIndex << endl;
		}
		float factor = GetFactorBetweenTwoTime(lastKey.mTime, nextKey.mTime, animationTime);
		//进行插值
		glm::vec3 interpolatedPosition = glm::mix(lastKey.mPosition, lastKey.mPosition, factor);
		
		return glm::translate(glm::mat4(1.0f), interpolatedPosition);
	}

	glm::mat4 InterpolateRotation(float animationTime)
	{
		if (mNumRotation == 1) {
			glm::quat rotation = glm::normalize(mRotationKeys[0].mRotation);
			return glm::toMat4(rotation);
		}
		int animIndex = getRotationIndex(animationTime);
		//获取当前时间点的上一个和下一个关键帧
		KeyRotation lastKey = mRotationKeys[animIndex];
		KeyRotation nextKey = mRotationKeys[animIndex + 1];
		float factor = GetFactorBetweenTwoTime(lastKey.mTime, nextKey.mTime, animationTime);
		//进行插值
		glm::quat interpolatedRotation = glm::slerp(lastKey.mRotation, nextKey.mRotation, factor);
		interpolatedRotation = glm::normalize(interpolatedRotation);
		return glm::toMat4(interpolatedRotation);
	}
	
	
	glm::mat4 InterpolateScale(float animationTime)
	{
		if (mNumScale == 1)
			return glm::scale(glm::mat4(1.0f), mScaleKeys[0].mScale);
		
		int animIndex = getScaleIndex(animationTime);
		//获取当前时间点的上一个和下一个关键帧
		KeyScale lastKey = mScaleKeys[animIndex];
		KeyScale nextKey = mScaleKeys[animIndex + 1];
		float factor = GetFactorBetweenTwoTime(lastKey.mTime, nextKey.mTime, animationTime);
		//插值 
		glm::vec3 interpolatedScale = glm::mix(lastKey.mScale, nextKey.mScale, factor);
		return glm::scale(glm::mat4(1.0f), interpolatedScale);
	}

};
#endif // !BONE_H
