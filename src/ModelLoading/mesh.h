#ifndef MESH_H
#define MESH_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

//#include <learnopengl/shader.h>
#include "../Render/Shader.h"
#include <string>
#include <vector>
using namespace std;

//表示一个顶点最多能被几个骨骼影响
#define MAX_BONE_INFLUENCE 4
//表示没有初始化过的BoneID的值
#define DEFAULT_BONE_ID -1

struct Vertex {

	glm::vec3 Position;

	glm::vec3 Normal;

	glm::vec2 TexCoords;

	glm::vec3 Tangent;

	glm::vec3 Bitangent;

	// 影响该顶点的骨骼ID，最多个数为MAX_BONE_INFLUENCE
	int m_BoneIDs[MAX_BONE_INFLUENCE];

	// 骨骼对该顶点的影响权重
	float m_Weights[MAX_BONE_INFLUENCE];
};

struct Texture
{
	unsigned int id;
	string type;
	string path;
};

class Mesh {
public:

	//该Mesh的所有顶点
	vector<Vertex>			vertices;
	vector<unsigned int>	indices;
	vector<Texture>			textures;
	unsigned int VAO;

	Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures)
	{
		this->vertices = vertices;
		this->indices = indices;
		this->textures = textures;
		setupMesh();
	}

    // render the mesh
    void Draw(Shader &shader) 
    {
        shader.SetInteger("material.diffuseCount", 0);
        shader.SetInteger("material.specularCount", 0);

        // bind appropriate textures
        unsigned int diffuseNr  = 1;
        unsigned int specularNr = 1;
        unsigned int normalNr   = 1;
        unsigned int heightNr   = 1;
        for(unsigned int i = 0; i < textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
            // retrieve texture number (the N in diffuse_textureN)
            string number;
            string name = textures[i].type;
            if (name == "texture_diffuse") {
                shader.SetInteger("material.diffuseCount", diffuseNr);
                number = std::to_string(diffuseNr++);
            }
            else if (name == "texture_specular") {
                shader.SetInteger("material.specularCount", specularNr);
                number = std::to_string(specularNr++); // transfer unsigned int to stream
            }
            else if (name == "texture_normal")
                number = std::to_string(normalNr++); // transfer unsigned int to stream
            else if (name == "texture_height")
                number = std::to_string(heightNr++); // transfer unsigned int to stream
            else if (name == "texture_emissive") {
                number = "";
                cout << "texture_emissive";
            }
            /*cout << "-------------------------" << endl;
            cout << "texture_diffuse" + to_string(diffuseNr) << endl;
            cout << "texture_specular" + to_string(specularNr) << endl;
            cout << "texture_normal" + to_string(normalNr) << endl;
            cout << "texture_height" + to_string(heightNr) << endl;
            cout << "-------------------------" << endl;*/
            // now set the sampler to the correct texture unit
            glUniform1i(glGetUniformLocation(shader.ID, ("material." + name + number).c_str()), i);
            // and finally bind the texture
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
        }
        
        // draw mesh
        glBindVertexArray(VAO);
        GLfloat ks[3] = { 0.5,0.5,0.5 };
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, ks);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        
        // always good practice to set everything back to defaults once configured.
        glActiveTexture(GL_TEXTURE0);
    }


private:
	unsigned int VBO, EBO;
	void setupMesh()
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

		//vertex position
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		// vertex normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
		// vertex texture coords
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
		// vertex tangent
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
		// vertex bitangent
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
		// bone ids
		glEnableVertexAttribArray(5);
		glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));
		// weights
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));

		glBindVertexArray(0);
		//不能漏了
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

};
#endif // !MESH_H
