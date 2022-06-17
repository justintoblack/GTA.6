#ifndef ANIMATOR_H
#define ANIMATOR_H

#include <glm/glm.hpp>
#include "Animation.h"
#include "AssimpGlmUtil.h"

#define reservedVectorSize 100

class Animator {
public:

	Animator(Animation* animation) 
	{
		mCurrentAnimation = animation;
		mCurrentTick = 0.0f;
		mRootGlobalTransform = AssimpGlmUtil::ConvertAiMatrix44ToGlmMat4(mCurrentAnimation->GetGlobalTransformation());
		InitFinalBoneMatrices();
	}
	
	//����ʱ��dt���¶���, �ݹ������������ʱ����transform����
	void UpdateAnimation(float dt)
	{
		mCurrentTick += dt * mCurrentAnimation->GetTicksPerSecond();
		//cout << "mCurrentTick:" << mCurrentTick << endl;
		mCurrentTick = fmod(mCurrentTick, mCurrentAnimation->GetDuration());
		//cout<<"after fmod, mCurrentTick:" << mCurrentTick << endl;
		CalculateBoneTransform(&(mCurrentAnimation->GetRootNode()), glm::mat4(1.0f));
	}

	// calculate bone transform recursively
	void CalculateBoneTransform(const AssimpNode* nowNode , glm::mat4 parentTransform)
	{
		string nowNodeName = nowNode->name;
		//�ýڵ���Ը��ڵ�ı任
		glm::mat4 nowNodeTransform = nowNode->transformation;

		Bone* bone = mCurrentAnimation->FindBone(nowNodeName);
		if (bone)
		{
			bone->UpdateLocalTransform(mCurrentTick);
			// ���µ�ǰʱ���󣬸ýڵ���Ը��ڵ�ı任
			nowNodeTransform = bone->getLocalTransform();
		}
		// ��ǰ�ڵ�ı任������Ҫ�����ڵ��Ӱ�����
		glm::mat4 nowGlobalTransform = parentTransform * nowNodeTransform;

		map<string, BoneInfo> bonemap = mCurrentAnimation->GetBoneInfoMap();

		if (bonemap.find(nowNodeName) != bonemap.end())
		{
			int boneID = bonemap[nowNodeName].id;
			//�����ռ�ת��������ռ�
			glm::mat4 offset = bonemap[nowNodeName].offset;
			//���ڵ�任 * ������ڵ��۳˵���ǰ�ڵ�õ���TRS�任 * �ӹ����ռ䵽����ռ�ı任
			mFinalBoneMatrices[boneID] = mRootGlobalTransform * nowGlobalTransform * offset;

		}

		//�ݹ�����ӽڵ�ı任
		int numChild = nowNode->numChildren;
		for (int childIndex = 0; childIndex < numChild; childIndex++)
			CalculateBoneTransform(&nowNode->children[childIndex], nowGlobalTransform);
	}

	void PlayAnimation(Animation* animation) {
		mCurrentAnimation = animation;
		mCurrentTick = 0;
	}

	vector<glm::mat4> GetFinalBoneMatrices() {
		return mFinalBoneMatrices;
	}

private:

	// ��mFinalBoneMatrices���г�ʼ����������ֵ����Ϊglm::mat4(1.0f)
	void InitFinalBoneMatrices() {
		mFinalBoneMatrices.reserve(reservedVectorSize);
		for (int i = 0; i < reservedVectorSize; i++)
			mFinalBoneMatrices.push_back(glm::mat4(1.0f));
	}

	Animation* mCurrentAnimation;
	float mCurrentTick;
	//���ڵ�任
	glm::mat4 mRootGlobalTransform;
	vector<glm::mat4> mFinalBoneMatrices;
};
#endif // !ANIMATOR_H
