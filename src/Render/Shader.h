#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
	// ��ɫ������ID
	unsigned int ID;
	Shader() {};
	//���캯�� ��ȡ����ɫ����Ƭ����ɫ��
	Shader(const char* vertexShaderPath, const char* fragmentShaderPath)
	{
		// 1. ���ļ�·�����ļ�
		std::string vertexShaderCode;
		std::string fragmentShaderCode;
		std::ifstream vShaderFile;
		std::ifstream fShaderFile;
		// ȷ��ifstream���ܹ��׳��쳣
		vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try
		{
			// ���ļ�
			vShaderFile.open(vertexShaderPath);
			fShaderFile.open(fragmentShaderPath);
			std::stringstream vShaderStream, fShaderStream;
			// ���ļ����ж��뵽buffer��
			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();
			// �ر��ļ���
			vShaderFile.close();
			fShaderFile.close();
			// ����ת��string����
			vertexShaderCode = vShaderStream.str();
			fragmentShaderCode = fShaderStream.str();
			//���ڲ��ԣ���ӡ������̨
			std::cout << vertexShaderCode << std::endl;
			std::cout << fragmentShaderCode << std::endl;
		}
		catch (std::ifstream::failure e)
		{
			std::cout << "����Shader��ȡ�ļ�ʧ��" << std::endl;
		}

		//ת���ɲ���Ҫ��char *���ͣ�
		const char* vShaderCode = vertexShaderCode.c_str();
		const char* fShaderCode = fragmentShaderCode.c_str();

		// 2. ������ɫ��
		unsigned int vertex, fragment;

		// ������ɫ��
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, NULL);
		glCompileShader(vertex);
		checkCompileErrors(vertex, "VERTEX");

		// Ƭ����ɫ��
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, NULL);
		glCompileShader(fragment);
		checkCompileErrors(fragment, "FRAGMENT");

		// ��ɫ������
		ID = glCreateProgram();
		glAttachShader(ID, vertex);
		glAttachShader(ID, fragment);
		glLinkProgram(ID);
		checkCompileErrors(ID, "PROGRAM");

		// ɾ����ɫ��
		glDeleteShader(vertex);
		glDeleteShader(fragment);
	}
	
	void use()
	{
		glUseProgram(ID);
	}
	
	void setBool(const std::string& name, bool value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
	}

	void setInt(const std::string& name, int value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	}

	void setFloat(const std::string& name, float value) const
	{
		glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
	}

private:
	// ���ڼ����ɫ������ / ���Ӵ����ʵ�ó�������
	void checkCompileErrors(unsigned int shader, std::string type)
	{
		int success;
		char infoLog[1024]; //�洢��Ϣ��־
		if (type != "PROGRAM")
		{
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "����Shader�������: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			}
		}
		else
		{
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "����Shader���Ӵ���: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			}
		}
	}
};
#endif
