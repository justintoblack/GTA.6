#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader
{
public:
	// 着色器程序ID
	unsigned int ID;
	Shader() {};
	//构造函数 读取点着色器和片段着色器
	Shader(const char* vertexShaderPath, const char* fragmentShaderPath)
	{
		// 1. 从文件路径打开文件
		std::string vertexShaderCode;
		std::string fragmentShaderCode;
		std::ifstream vShaderFile;
		std::ifstream fShaderFile;
		// 确保ifstream流能够抛出异常
		vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try
		{
			// 打开文件
			vShaderFile.open(vertexShaderPath);
			fShaderFile.open(fragmentShaderPath);
			std::stringstream vShaderStream, fShaderStream;
			// 从文件流中读入到buffer中
			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();
			// 关闭文件流
			vShaderFile.close();
			fShaderFile.close();
			// 把流转到string里面
			vertexShaderCode = vShaderStream.str();
			fragmentShaderCode = fShaderStream.str();
			//用于测试，打印到控制台
			std::cout << vertexShaderCode << std::endl;
			std::cout << fragmentShaderCode << std::endl;
		}
		catch (std::ifstream::failure e)
		{
			std::cout << "错误：Shader读取文件失败" << std::endl;
		}

		//转换成参数要求char *类型，
		const char* vShaderCode = vertexShaderCode.c_str();
		const char* fShaderCode = fragmentShaderCode.c_str();

		// 2. 编译着色器
		unsigned int vertex, fragment;

		// 顶点着色器
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, NULL);
		glCompileShader(vertex);
		checkCompileErrors(vertex, "VERTEX");

		// 片段着色器
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, NULL);
		glCompileShader(fragment);
		checkCompileErrors(fragment, "FRAGMENT");

		// 着色器程序
		ID = glCreateProgram();
		glAttachShader(ID, vertex);
		glAttachShader(ID, fragment);
		glLinkProgram(ID);
		checkCompileErrors(ID, "PROGRAM");

		// 删除着色器
		glDeleteShader(vertex);
		glDeleteShader(fragment);
	}
	
	void use()
	{
		glUseProgram(ID);
	}

	void SetFloat(const char* name, float value)
	{
		glUniform1f(glGetUniformLocation(this->ID, name), value);
	}
	void SetInteger(const char* name, int value)
	{
		glUniform1i(glGetUniformLocation(this->ID, name), value);
	}
	void SetVector2f(const char* name, float x, float y)
	{
		glUniform2f(glGetUniformLocation(this->ID, name), x, y);
	}
	void SetVector2f(const char* name, const glm::vec2& value)
	{
		glUniform2f(glGetUniformLocation(this->ID, name), value.x, value.y);
	}
	void SetVector3f(const char* name, float x, float y, float z)
	{
		glUniform3f(glGetUniformLocation(this->ID, name), x, y, z);
	}
	void SetVector3f(const char* name, const glm::vec3& value)
	{
		glUniform3f(glGetUniformLocation(this->ID, name), value.x, value.y, value.z);
	}
	void SetVector4f(const char* name, float x, float y, float z, float w)
	{
		glUniform4f(glGetUniformLocation(this->ID, name), x, y, z, w);
	}
	void SetVector4f(const char* name, const glm::vec4& value)
	{
		glUniform4f(glGetUniformLocation(this->ID, name), value.x, value.y, value.z, value.w);
	}
	void SetMatrix4fv(const char* name, const glm::mat4& matrix)
	{
		glUniformMatrix4fv(glGetUniformLocation(this->ID, name), 1, false, glm::value_ptr(matrix));
	}

private:
	// 用于检查着色器编译 / 链接错误的实用程序函数。
	void checkCompileErrors(unsigned int shader, std::string type)
	{
		int success;
		char infoLog[1024]; //存储信息日志
		if (type != "PROGRAM")
		{
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "错误：Shader编译错误: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			}
		}
		else
		{
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "错误：Shader链接错误: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			}
		}
	}
};
#endif
