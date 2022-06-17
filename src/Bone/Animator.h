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
	
	//根据时间dt更新动画, 递归计算骨骼在这个时间点的transform矩阵
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
		//该节点相对父节点的变换
		glm::mat4 nowNodeTransform = nowNode->transformation;

		Bone* bone = mCurrentAnimation->FindBone(nowNodeName);
		if (bone)
		{
			bone->UpdateLocalTransform(mCurrentTick);
			// 更新当前时间点后，该节点相对父节点的变换
			nowNodeTransform = bone->getLocalTransform();
		}
		// 当前节点的变换矩阵，需要将父节点的影响乘上
		glm::mat4 nowGlobalTransform = parentTransform * nowNodeTransform;

		map<string, BoneInfo> bonemap = mCurrentAnimation->GetBoneInfoMap();

		if (bonemap.find(nowNodeName) != bonemap.end())
		{
			int boneID = bonemap[nowNodeName].id;
			//骨骼空间转换到网格空间
			glm::mat4 offset = bonemap[nowNodeName].offset;
			//根节点变换 * 逐个父节点累乘到当前节点得到的TRS变换 * 从骨骼空间到网格空间的变换
			mFinalBoneMatrices[boneID] = mRootGlobalTransform * nowGlobalTransform * offset;

		}

		//递归计算子节点的变换
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

	// 将mFinalBoneMatrices进行初始化，将矩阵值都设为glm::mat4(1.0f)
	void InitFinalBoneMatrices() {
		mFinalBoneMatrices.reserve(reservedVectorSize);
		for (int i = 0; i < reservedVectorSize; i++)
			mFinalBoneMatrices.push_back(glm::mat4(1.0f));
	}

	Animation* mCurrentAnimation;
	float mCurrentTick;
	//根节点变换
	glm::mat4 mRootGlobalTransform;
	vector<glm::mat4> mFinalBoneMatrices;
};
#endif // !ANIMATOR_H
