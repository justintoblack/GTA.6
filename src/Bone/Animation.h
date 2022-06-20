#ifndef ANIMATION_H
#define ANIMATION_H

#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include "Bone.h"
#include "ModelAnimation.h"
#include "AssimpGlmUtil.h"

using namespace std;
struct AssimpNode {
	string name;
	// 该节点相对父节点的变换
	glm::mat4 transformation;
	int numChildren;
	vector<AssimpNode> children;
};

class Animation {
public :
	Animation(){}
	//加载骨骼动画
	Animation(const string& animationPath, ModelAnimation* model)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(animationPath, aiProcess_Triangulate);
		// check for errors
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
		{
			cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
			return;
		}
		cout << "Animation:: 找到了";
		cout << scene->mNumAnimations;
		cout << "个动画" << endl;
		if (scene->mNumAnimations == 0)
		{
			cout << "Animation:: No Animations." << endl;
			return;
		}
		cout << "动画名：" << scene->mAnimations[0]->mName.data << endl;
		//暂时只加载第一个动画
		aiAnimation* animation = scene->mAnimations[0];
		mDuration = animation->mDuration;
		cout << "Duration:" << mDuration << endl;
		mTicksPerSecond = animation->mTicksPerSecond;
		cout << "TicksPerSecond:" << mTicksPerSecond << endl;
		mGlobalTransformation = scene->mRootNode->mTransformation;
		mGlobalTransformation = mGlobalTransformation.Inverse();
		ReadHierarchyNode(mRootNode, scene->mRootNode);
		ReadBones(animation, *model);
	}

	//查找名为boneName的骨骼
	Bone* FindBone(const string boneName) {
		vector<Bone>::iterator it = mBones.begin();
		for (; it != mBones.end(); it++)
		{
			if (it->getName() == boneName)
				return &(*it);
		}
		return nullptr;
	}

	AssimpNode& GetRootNode() {
		return mRootNode;
	}
	map<string, BoneInfo>& GetBoneInfoMap() {
		return mBoneInfoMap;
	}
	float GetDuration() {
		return mDuration;
	}
	float GetTicksPerSecond() {
		return mTicksPerSecond;
	}
	//场景根节点的变换
	aiMatrix4x4& GetGlobalTransformation() {
		return mGlobalTransformation;
	}

private:

	void ReadBones(const aiAnimation* animation, ModelAnimation& model)
	{	
		//先将模型的boneInfoMap赋给当前动画
		mBoneInfoMap = model.getBoneInfoMap();
		int& boneCount = model.getBoneCount();
		//获得动画的通道channel数量
		int numChannel = animation->mNumChannels;
		//每个通道影响一个节点/骨骼, 循环遍历每一个通道
		for (int cnIndex = 0; cnIndex < numChannel; cnIndex++)
		{
			
			aiNodeAnim* channel = animation->mChannels[cnIndex];
			string name = channel->mNodeName.data;
			//如果还没有存储当前骨骼
			if (mBoneInfoMap.find(name) == mBoneInfoMap.end())
			{
				//记录这块骨骼
				mBoneInfoMap[name].id = boneCount++;
			}
			//将这个通道所影响的骨骼通过Bone的构造函数创建，并添加到mBones数组中
			//具体怎么影响到KeyPosition等，放到Bone构造函数实现
			mBones.push_back(Bone(name, mBoneInfoMap[name].id, channel));
		}

	}

	//递归读取所有aiNode，将srcNode的属性值赋给root，再对所有子节点也执行相同操作
	void ReadHierarchyNode(AssimpNode& root, const aiNode* srcNode)
	{
		root.name = srcNode->mName.data;
		root.numChildren = srcNode->mNumChildren;
		root.transformation = AssimpGlmUtil::ConvertAiMatrix44ToGlmMat4(srcNode->mTransformation);

		for (int childIndex = 0; childIndex < root.numChildren; childIndex++) {
			AssimpNode childNode;
			ReadHierarchyNode(childNode, srcNode->mChildren[childIndex]);
			root.children.push_back(childNode);
		}
	}
	AssimpNode mRootNode;
	//该动画的通道影响的骨骼
	vector<Bone> mBones;
	map<string, BoneInfo> mBoneInfoMap;
	//Duration of the animation in ticks
	float mDuration;
	//ticks per second. 0 if not specified.
	float mTicksPerSecond;
	aiMatrix4x4 mGlobalTransformation;
};


#endif // !ANIMATION_H
