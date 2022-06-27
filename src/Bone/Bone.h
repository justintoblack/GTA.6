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
// �洢�ؼ�֡Position
struct KeyPosition {
	float mTime;
	glm::vec3 mPosition;
};
// �洢�ؼ�֡��ת
struct KeyRotation {
	float mTime;
	glm::quat mRotation;
};
// �洢�ؼ�֡����
struct KeyScale {
	float mTime;
	glm::vec3 mScale;
};


class Bone {
public:
	//�Ӷ���ͨ������ȡ�ؼ�֡��Ϣ
	Bone(const string& name, int ID, const aiNodeAnim* channel)
	{
		this->mName = name;
		this->mID = ID;
		this->mLocalTransform = glm::mat4(1.0f);

		//��ȡ�ؼ�֡λ��Position
		mNumPosition = channel->mNumPositionKeys;
		for (int posIndex = 0; posIndex < mNumPosition; posIndex++)
		{
			KeyPosition keyPosition;
			aiVectorKey aiKeyPos = channel->mPositionKeys[posIndex];
			keyPosition.mTime = aiKeyPos.mTime;
			keyPosition.mPosition = AssimpGlmUtil::ConvertAiVec3DToGlmVec3(aiKeyPos.mValue);
			mPositionKeys.push_back(keyPosition);
		}

		//��ȡ�ؼ�֡��ת
		mNumRotation = channel->mNumRotationKeys;
		for (int rotIndex = 0; rotIndex < mNumRotation; rotIndex++)
		{
			KeyRotation keyRotation;
			aiQuatKey aiKeyRotation = channel->mRotationKeys[rotIndex];
			keyRotation.mTime = aiKeyRotation.mTime;
			keyRotation.mRotation = AssimpGlmUtil::ConvertAiQuaternionToGlmQuat(aiKeyRotation.mValue);
			mRotationKeys.push_back(keyRotation);
		}

		//��ȡ�ؼ�֡����
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

	//�Ȼ�ȡ��ֵ��Ľ������˺����LocalTransform����
	void UpdateLocalTransform(float animationTime)
	{
		glm::mat4 translation = InterpolatePosition(animationTime);
		glm::mat4 rotation = InterpolateRotation(animationTime);
		glm::mat4 scale = InterpolateScale(animationTime);
		mLocalTransform = translation * rotation * scale;
	}
	//����interpolation factor
	float GetFactorBetweenTwoTime(float time1, float time2, float animationTime)
	{
		float period1 = animationTime - time1;
		float totalTime = time2 - time1;
		return (float)(period1 / totalTime);
	}
	//�����Ĭ��return 0
	//�ҳ�����animationTime����������index��ʱ����ڣ����ض����е�ǰ��
	int getPositionIndex(float animationTime)
	{
		for (int posIndex = 0; posIndex < mNumPosition - 1; posIndex++)
			if (animationTime < mPositionKeys[posIndex + 1].mTime)
				return posIndex;
		return 0;
	}

	//�ҳ�����animationTime����������index��ʱ����ڣ����ض����е�ǰ��
	int getRotationIndex(float animationTime)
	{
		for (int rotIndex = 0; rotIndex < mNumRotation - 1; rotIndex++)
			if (animationTime < mRotationKeys[rotIndex + 1].mTime)
				return rotIndex;
		return 0;
	}

	//�ҳ�����animationTime����������index��ʱ����ڣ����ض����е�ǰ��
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
	//�洢�ؼ�֡λ��Position������
	vector<KeyPosition> mPositionKeys;
	//�洢�ؼ�֡��תRotation������
	vector<KeyRotation> mRotationKeys;
	//�洢�ؼ�֡����Scale������
	vector<KeyScale>	mScaleKeys;
	//KeyPosition������
	int mNumPosition;
	//KeyRotation������
	int mNumRotation;
	//KeyScale������
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
		//��ȡ��ǰʱ������һ������һ���ؼ�֡
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
		//���в�ֵ
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
		//��ȡ��ǰʱ������һ������һ���ؼ�֡
		KeyRotation lastKey = mRotationKeys[animIndex];
		KeyRotation nextKey = mRotationKeys[animIndex + 1];
		float factor = GetFactorBetweenTwoTime(lastKey.mTime, nextKey.mTime, animationTime);
		//���в�ֵ
		glm::quat interpolatedRotation = glm::slerp(lastKey.mRotation, nextKey.mRotation, factor);
		interpolatedRotation = glm::normalize(interpolatedRotation);
		return glm::toMat4(interpolatedRotation);
	}
	
	
	glm::mat4 InterpolateScale(float animationTime)
	{
		if (mNumScale == 1)
			return glm::scale(glm::mat4(1.0f), mScaleKeys[0].mScale);
		
		int animIndex = getScaleIndex(animationTime);
		//��ȡ��ǰʱ������һ������һ���ؼ�֡
		KeyScale lastKey = mScaleKeys[animIndex];
		KeyScale nextKey = mScaleKeys[animIndex + 1];
		float factor = GetFactorBetweenTwoTime(lastKey.mTime, nextKey.mTime, animationTime);
		//��ֵ 
		glm::vec3 interpolatedScale = glm::mix(lastKey.mScale, nextKey.mScale, factor);
		return glm::scale(glm::mat4(1.0f), interpolatedScale);
	}

};
#endif // !BONE_H
