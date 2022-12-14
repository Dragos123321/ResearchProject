#pragma once

#include <string>
#include <glad/glad.h>

struct ShaderType {
	ShaderType() : type(-1), name("") {}
	ShaderType(unsigned int type, std::string name) : type(type), name(name) {}

	unsigned int type;
	std::string name;
};

bool checkCompileErrors(unsigned int shader, std::string type, std::string shaderName);
std::string getShaderName(const char* shaderPath);
ShaderType getShaderType(const char* path);

class BaseShader
{
public:
	BaseShader(const char* shaderPath);
	virtual ~BaseShader() = default;

	std::string getName();
	ShaderType getType();
	unsigned int getShader();
private:
	std::string loadShaderFromFile(const char* shaderPath);

	std::string m_path;
	unsigned int m_shader;
	ShaderType m_shaderType;
};