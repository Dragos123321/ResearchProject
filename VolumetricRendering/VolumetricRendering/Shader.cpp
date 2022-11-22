#include "Shader.h"

Shader::Shader(std::string name) : m_name{ name }, m_linked{ false }, m_isCompute{ false }, m_ID{ glCreateProgram() }
{
}

Shader::~Shader() {
	glDeleteProgram(m_ID);
}

Shader::Shader(std::string name, const char* computeShaderPath) : m_name{ name }, m_linked{ false }, m_isCompute{ false }, m_ID{ glCreateProgram() }
{
	attachShader(BaseShader(computeShaderPath));
	linkPrograms();
}


Shader* Shader::attachShader(BaseShader s)
{
	if (!m_isCompute) 
	{
		glAttachShader(m_ID, s.getShader());
		if (s.getName() == "COMPUTE")
			m_isCompute = true;
		this->m_shaders.push_back(s.getShader());
	}
	else 
	{
		std::cout << "ERROR: TRYING TO LINK A NON COMPUTE SHADER TO COMPUTE PROGRAM" << std::endl;
	}

	return this;
}

void Shader::linkPrograms()
{
	glLinkProgram(m_ID);

	if (checkCompileErrors(m_ID, "PROGRAM", "")) 
	{
		m_linked = true;
		std::cout << "PROGRAM " << m_name << " CORRECTLY LINKED" << std::endl;
		while (!m_shaders.empty()) 
		{
			glDeleteShader(m_shaders.back());
			m_shaders.pop_back();
		}
	}
	else 
	{
		std::cout << "ERROR WHILE LINKING TO " << m_name << " PROGRAM" << std::endl;
	}
}


void Shader::use()
{
	if (m_linked)
		glUseProgram(m_ID);
	else 
	{
		std::cout << "PROGRAMS NOT LINKED!" << std::endl;
	}
}

void Shader::setBool(const std::string& name, bool value) const
{
	glUniform1i(glGetUniformLocation(m_ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string& name, int value) const
{
	glUniform1i(glGetUniformLocation(m_ID, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) const
{
	glUniform1f(glGetUniformLocation(m_ID, name.c_str()), value);
}
void Shader::setVec2(const std::string& name, glm::vec2 vector) const
{
	unsigned int location = glGetUniformLocation(m_ID, name.c_str());
	glUniform2fv(location, 1, glm::value_ptr(vector));
}
void Shader::setVec3(const std::string& name, glm::vec3 vector) const
{
	unsigned int location = glGetUniformLocation(m_ID, name.c_str());
	glUniform3fv(location, 1, glm::value_ptr(vector));
}

void Shader::setVec4(const std::string& name, glm::vec4 vector) const
{
	unsigned int location = glGetUniformLocation(m_ID, name.c_str());
	glUniform4fv(location, 1, glm::value_ptr(vector));
}

void Shader::setMat4(const std::string& name, glm::mat4 matrix) const
{
	unsigned int mat = glGetUniformLocation(m_ID, name.c_str());
	glUniformMatrix4fv(mat, 1, false, glm::value_ptr(matrix));
}

void Shader::setSampler2D(const std::string& name, unsigned int texture, int id) const
{
	glActiveTexture(GL_TEXTURE0 + id);
	glBindTexture(GL_TEXTURE_2D, texture);
	this->setInt(name, id);
}
void Shader::setSampler3D(const std::string& name, unsigned int texture, int id) const
{
	glActiveTexture(GL_TEXTURE0 + id);
	glBindTexture(GL_TEXTURE_3D, texture);
	this->setInt(name, id);
}