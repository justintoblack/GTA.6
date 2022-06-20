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
	// �ýڵ���Ը��ڵ�ı任
	glm::mat4 transformation;
	int numChildren;
	vector<AssimpNode> children;
};

class Animation {
public :
	Animation(){}
	//���ع�������
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
		cout << "Animation:: �ҵ���";
		cout << scene->mNumAnimations;
		cout << "������" << endl;
		if (scene->mNumAnimations == 0)
		{
			cout << "Animation:: No Animations." << endl;
			return;
		}
		cout << "��������" << scene->mAnimations[0]->mName.data << endl;
		//��ʱֻ���ص�һ������
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

	//������ΪboneName�Ĺ���
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
	//�������ڵ�ı任
	aiMatrix4x4& GetGlobalTransformation() {
		return mGlobalTransformation;
	}

private:

	void ReadBones(const aiAnimation* animation, ModelAnimation& model)
	{	
		//�Ƚ�ģ�͵�boneInfoMap������ǰ����
		mBoneInfoMap = model.getBoneInfoMap();
		int& boneCount = model.getBoneCount();
		//��ö�����ͨ��channel����
		int numChannel = animation->mNumChannels;
		//ÿ��ͨ��Ӱ��һ���ڵ�/����, ѭ������ÿһ��ͨ��
		for (int cnIndex = 0; cnIndex < numChannel; cnIndex++)
		{
			
			aiNodeAnim* channel = animation->mChannels[cnIndex];
			string name = channel->mNodeName.data;
			//�����û�д洢��ǰ����
			if (mBoneInfoMap.find(name) == mBoneInfoMap.end())
			{
				//��¼������
				mBoneInfoMap[name].id = boneCount++;
			}
			//�����ͨ����Ӱ��Ĺ���ͨ��Bone�Ĺ��캯������������ӵ�mBones������
			//������ôӰ�쵽KeyPosition�ȣ��ŵ�Bone���캯��ʵ��
			mBones.push_back(Bone(name, mBoneInfoMap[name].id, channel));
		}

	}

	//�ݹ��ȡ����aiNode����srcNode������ֵ����root���ٶ������ӽڵ�Ҳִ����ͬ����
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
	//�ö�����ͨ��Ӱ��Ĺ���
	vector<Bone> mBones;
	map<string, BoneInfo> mBoneInfoMap;
	//Duration of the animation in ticks
	float mDuration;
	//ticks per second. 0 if not specified.
	float mTicksPerSecond;
	aiMatrix4x4 mGlobalTransformation;
};


#endif // !ANIMATION_H
