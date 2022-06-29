#ifndef MODEL_ANIMATION_H
#define MODEL_ANIMATION_H

//#include <glad/glad.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../Render/stb_image.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../ModelLoading/mesh.h"
#include "../Render/Shader.h"
#include "Bone.h"
#include "../Utils/AssimpGlmUtil.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
using namespace std;

//static unsigned int TextureFromFile(const char* path, const string& directory, bool gamma = false);
//static unsigned int TextureFrom_FBX_EmbeddedTexture(const aiTexture* aiTex, bool gama = false);
//static unsigned int GenerateTex(unsigned char* data, int width, int height, int nrComponents, bool gama = false);

//支持骨骼动画的模型
class ModelAnimation
{
public:

    // stores all loaded textures, make sure textures aren't loaded more than once.
    vector<Texture> textures_loaded;
    vector<Mesh>    meshes;
    vector<aiString> meshNames;
    int nMesh;
    string directory;
    bool gammaCorrection;

    ModelAnimation() {}
    ModelAnimation(string const& path, bool gamma = false) : gammaCorrection(gamma)
    {
        loadModel(path);
    }

    // draws the model, and all its meshes
    void Draw(Shader& shader)
    {
        for (unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].Draw(shader);
    }

    //setter & getter
    void setPos(glm::vec3 newPos)
    {
        this->pos = newPos;
    }

    glm::vec3 getPos()
    {
        return this->pos;
    }
    map<string, BoneInfo> getBoneInfoMap()
    {
        return mBoneInfoMap;
    }
    int& getBoneCount()
    {
        return mBoneCount;
    }
private:
    //通过骨骼的名称获取BoneInfo
    map<string, BoneInfo> mBoneInfoMap;
    int mBoneCount = 0;

    //loads a model from path
    void loadModel(string const& path)
    {
        std::cout << "loadModel(), path: " << path << std::endl;
        // read file via ASSIMP
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
        // check for errors
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
        {
            cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
            return;
        }
        
        directory = path.substr(0, path.find_last_of('/'));
        std::cout << "directory: " << directory << std::endl;
        // process ASSIMP's root node recursively
        processNode(scene->mRootNode, scene);
    }

    // processes a node recursively
    void processNode(aiNode* node, const aiScene* scene)
    {
        nMesh = node->mNumMeshes;
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene));
            aiString name = mesh->mName;
            meshNames.push_back(name);
        }
        //process child node recursively
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene);
        }
    }

    //初始化顶点所做的工作，将该顶点vertex的每个BoneID都设为DEFAULT_BONE_ID，表示还没有赋值，将影响权重m_Weights都设为0
    void setDefaultBoneWeightToVertex(Vertex& vertex)
    {
        for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
        {
            vertex.m_BoneIDs[i] = DEFAULT_BONE_ID;
            vertex.m_Weights[i] = 0.0f;
        }
    }

    Mesh processMesh(aiMesh* mesh, const aiScene* scene)
    {
        vector<Vertex> vertices;
        vector<unsigned int> indices;
        vector<Texture> textures;

        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
            glm::vec3 vector;
            setDefaultBoneWeightToVertex(vertex);
            // positions
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.Position = vector;
            // normals
            if (mesh->HasNormals())
            {
                vector.x = mesh->mNormals[i].x;
                vector.y = mesh->mNormals[i].y;
                vector.z = mesh->mNormals[i].z;
                vertex.Normal = vector;
            }
            // texture coordinates
            if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
            {
                glm::vec2 vec;
                // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
                // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;
                // tangent
                vector.x = mesh->mTangents[i].x;
                vector.y = mesh->mTangents[i].y;
                vector.z = mesh->mTangents[i].z;
                vertex.Tangent = vector;
                // bitangent
                vector.x = mesh->mBitangents[i].x;
                vector.y = mesh->mBitangents[i].y;
                vector.z = mesh->mBitangents[i].z;
                vertex.Bitangent = vector;
            }
            else
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);

            vertices.push_back(vertex);
        }

        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            // retrieve all indices of the face and store them in the indices vector
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }

        // process materials
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", scene);
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

        vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular", scene);
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        
        std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal", scene);
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        
        std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height", scene);
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

        //extract bone info
        cout << "开始：ExtractBoneInfo.." << endl;
        ExtractBoneInfo(mesh, vertices);
        cout << "结束：ExtractBoneInfo.." << endl;
        return Mesh(vertices, indices, textures);
    }
    
    /**
    * 将该Bone信息指定给Vertex，即：指定了vertex被boneID骨骼影响，影响权重为weight
    */
    void setBoneWeightToVertex(Vertex& vertex, const int boneID, const float weight)
    {
        for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
        {
            if (vertex.m_BoneIDs[i] == DEFAULT_BONE_ID)
            {
                vertex.m_BoneIDs[i] = boneID;
                vertex.m_Weights[i] = weight;
                break;
            }
        }
    }

    //bug点，第二个参数vector<Vertex>& 忘记写引用，导致并没有真正的更改到顶点上
    /**
    * @param mesh 要处理骨骼的mesh
    * @param vertices 该mesh的所有顶点
    */
    void ExtractBoneInfo(aiMesh* mesh, vector<Vertex>& vertices)
    {
        //对Mesh的每块骨骼进行处理
        for (int boneIndex = 0; boneIndex < mesh->mNumBones; boneIndex++)
        {
            //为该骨骼分配ID
            int boneID = DEFAULT_BONE_ID;
            //获取当前循环的骨骼
            aiBone* pNowBone = mesh->mBones[boneIndex];
            //获取骨骼名称
            string boneName = pNowBone->mName.C_Str();
            cout << "boneIndex:" << boneIndex << "  ";
            cout << "骨骼名称：" << boneName << endl;
            //如果当前不存在该名字的骨骼
            //则将bondID和offset矩阵存储到BoneInfo中，将该新的BoneInfo添加到映射表mBoneInfoMap
            if (mBoneInfoMap.find(boneName) == mBoneInfoMap.end()) {
                BoneInfo boneInfo;
                //指定当前的骨骼数量作为该新骨骼的ID
                //bug,漏写boneID = mBoneCount++;导致每次set的时候boneid都是-1
                boneID = mBoneCount++;
                boneInfo.id = boneID;
                //获取offset矩阵，转为glm::mat4格式
                boneInfo.offset = AssimpGlmUtil::ConvertAiMatrix44ToGlmMat4(pNowBone->mOffsetMatrix);
                //建立映射，方便后续查找
                mBoneInfoMap[boneName] = boneInfo;
            }
            else
                boneID = mBoneInfoMap[boneName].id;
            //遍历该骨骼的权重数组,对于数组里的每个aiVertexWeight,里面还有vertexId和mWeight
            //即该骨骼对vertexID顶点有着mWeight的影响权重
            int numWeights = pNowBone->mNumWeights;
            aiVertexWeight* weightsArray = pNowBone->mWeights;
            
            for (int weightIndex = 0; weightIndex < numWeights; weightIndex++)
            {
                int vertexID = weightsArray[weightIndex].mVertexId;
                //之前将weight的float类型写成了int
                float weight = weightsArray[weightIndex].mWeight;
                /*cout << "bonename, vertexid, weights:" << boneName
                    << " " << vertexID << " " << weight << endl;*/
                //将该骨骼对顶点的影响设置到顶点中
                
                /*cout << "before:" << endl;
                for (int vIndex = 0; vIndex < MAX_BONE_INFLUENCE; vIndex++)
                {
                    cout << "boneid, weight:" << vertices[vertexID].m_BoneIDs[vIndex] << " "
                        << vertices[vertexID].m_Weights[vIndex] << endl;
                }*/
                setBoneWeightToVertex(vertices[vertexID], boneID, weight);
                /*cout << "after:" << endl;
                for (int vIndex = 0; vIndex < MAX_BONE_INFLUENCE; vIndex++)
                {
                    cout << "boneid, weight:" << vertices[vertexID].m_BoneIDs[vIndex] << " "
                        << vertices[vertexID].m_Weights[vIndex] << endl;
                }*/
            }
            
        }
    }

    
    vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName, const aiScene* scene)
    {
        vector<Texture> textures;
        for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);

            // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
            bool skip = false;
            for (unsigned int j = 0; j < textures_loaded.size(); j++)
            {
                if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
                {
                    textures.push_back(textures_loaded[j]);
                    skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                    break;
                }
            }
            if (!skip)
            {   // if texture hasn't been loaded already, load it
                Texture texture;
                //判断是否内嵌贴图
                auto tex = scene->GetEmbeddedTexture(str.C_Str());

                if (tex != nullptr)
                {
                    //有内嵌贴图
                    cout << "有贴图" << endl;
                    texture.id = TextureFrom_FBX_EmbeddedTexture(tex);
                }
                else
                {
                    //无内嵌贴图，外部图片路径加载
                    cout << "无贴图" << endl;
                    texture.id = TextureFromFile(str.C_Str(), this->directory);
                }

                texture.type = typeName;
                texture.path = str.C_Str();
                textures.push_back(texture);
                textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
            }
        }
        return textures;
    }

    //used in model matrix
    glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f);
    //glm::quat q=glm::quat(0,0,0,0);
    glm::vec3 scale = glm::vec3(1, 1, 1);
};


//unsigned int TextureFromFile(const char* path, const string& directory, bool gamma)
//{
//    std::cout << "direc: " << directory << std::endl;
//    std::cout << "path: " << path << std::endl;
//
//    string filename = string(path);
//    filename = directory + '/' + filename;
//    std::cout << "filename: " << filename << std::endl;
//    unsigned int textureID;
//    glGenTextures(1, &textureID);
//
//    int width, height, nrComponents;
//    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
//    if (data)
//    {
//        GLenum format;
//        if (nrComponents == 1)
//            format = GL_RED;
//        else if (nrComponents == 3)
//            format = GL_RGB;
//        else if (nrComponents == 4)
//            format = GL_RGBA;
//
//        glBindTexture(GL_TEXTURE_2D, textureID);
//        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
//        glGenerateMipmap(GL_TEXTURE_2D);
//
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//        stbi_image_free(data);
//    }
//    else
//    {
//        std::cout << "Texture failed to load at path: " << path << std::endl;
//        stbi_image_free(data);
//    }
//
//    return textureID;
//}

//unsigned int TextureFrom_FBX_EmbeddedTexture(const aiTexture* aiTex, bool gama)
//{
//    int width, height, channels;
//
//    unsigned char* data = nullptr;
//
//    if (aiTex->mHeight == 0)
//    {
//        data = stbi_load_from_memory(reinterpret_cast<unsigned char*>(aiTex->pcData),
//            aiTex->mWidth, &width, &height, &channels, 0);
//    }
//    else
//    {
//        data = stbi_load_from_memory(reinterpret_cast<unsigned char*>(aiTex->pcData),
//            aiTex->mWidth * aiTex->mHeight, &width, &height, &channels, 0);
//    }
//
//    return GenerateTex(data, width, height, channels);
//}
//
//unsigned int GenerateTex(unsigned char* data, int width, int height, int nrComponents, bool gama)
//{
//    unsigned int textureID;
//    glGenTextures(1, &textureID);
//
//    if (data)
//    {
//        GLenum format;
//        if (nrComponents == 1)
//            format = GL_RED;
//        else if (nrComponents == 3)
//            format = GL_RGB;
//        else if (nrComponents == 4)
//            format = GL_RGBA;
//
//        glBindTexture(GL_TEXTURE_2D, textureID);
//
//        //伽马矫正需要设置内部格式（第三个参数）为GL_SRGB或者GL_SRGB_ALPHA，这里未设置，可参考伽马矫正那一章节
//        //https://learnopengl-cn.github.io/05%20Advanced%20Lighting/02%20Gamma%20Correction/
//        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
//        glGenerateMipmap(GL_TEXTURE_2D);
//
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//        stbi_image_free(data);
//    }
//    else
//    {
//        std::cout << "Texture failed to load at path: " << std::endl;
//        stbi_image_free(data);
//    }
//
//    return textureID;
//}

#endif
