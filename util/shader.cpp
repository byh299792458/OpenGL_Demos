#include "shader.hpp"

using std::string;
using std::ifstream;
using std::stringstream;
using std::cout;
using std::endl;

Shader::Shader(const char* vsPath, const char* fsPath, const char* gsPath)
{
	// 1�����ļ�·���л�ȡ����/Ƭ����ɫ��
	string vsCode, fsCode, gsCode;
	ifstream vsFile, fsFile, gsFile;
	// ��֤ifstream��������׳��쳣
	vsFile.exceptions(ifstream::failbit | ifstream::badbit);
	fsFile.exceptions(ifstream::failbit | ifstream::badbit);
	gsFile.exceptions(ifstream::failbit | ifstream::badbit);
	try 
	{
		// ���ļ�
		vsFile.open(vsPath);
		fsFile.open(fsPath);

		// ��ȡ�ļ��Ļ������ݵ���������
		stringstream vsss, fsss, gsss;
		vsss << vsFile.rdbuf();
		fsss << fsFile.rdbuf();
		// �ر��ļ�������
		vsFile.close();
		fsFile.close();
		// ת����������string
		vsCode = vsss.str();
		fsCode = fsss.str();

		if (gsPath != nullptr)
		{
			gsFile.open(gsPath);
			std::stringstream gShaderStream;
			gShaderStream << gsFile.rdbuf();
			gsFile.close();
			gsCode = gShaderStream.str();
		}
	}
	catch (ifstream::failure e)
	{
		cout << "ERROR::" << __FILE__ << "::" << __LINE__ << "::FILE_NOT_SUCCESFULLY_READ" << endl;
	}


	// 2����д��ɫ��
	unsigned int vertex, fragment, geometry;
	int success;
	char infoLog[512];
	// ������ɫ��
	vertex = glCreateShader(GL_VERTEX_SHADER);
	const char* vscode = vsCode.c_str();
	glShaderSource(vertex, 1, &vscode, nullptr); // �ڶ�������ָ��Ҫ���ݶ��ٸ��ַ���
	glCompileShader(vertex);
		// ��ӡ�������
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertex, 512, nullptr, infoLog);
		cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << endl;
	}
	// Ƭ����ɫ��
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	const char* fscode = fsCode.c_str();
	glShaderSource(fragment, 1, &fscode, nullptr);
	glCompileShader(fragment);
		// ��ӡ�������
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragment, 512, nullptr, infoLog);
		cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << endl;
	}
	// ������ɫ��
	if (gsPath != nullptr)
	{
		const char* gShaderCode = gsCode.c_str();
		geometry = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometry, 1, &gShaderCode, NULL);
		glCompileShader(geometry);
		glGetShaderiv(geometry, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(geometry, 512, nullptr, infoLog);
			cout << "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n" << infoLog << endl;
		}
	}
	// ������ɫ��
	id_ = glCreateProgram();
	glAttachShader(id_, vertex);
	glAttachShader(id_, fragment);
	if (gsPath != nullptr)
		glAttachShader(id_, geometry);
	glLinkProgram(id_);
		// ��ӡ���Ӵ���
	glGetProgramiv(id_, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(id_, 512, nullptr, infoLog);
		cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << endl;
	}
	// ɾ����ɫ��
	glDeleteShader(vertex);
	glDeleteShader(fragment);
	if (gsPath != nullptr)
		glDeleteShader(geometry);
}

void Shader::use()
{
	glUseProgram(id_);
}

void Shader::setBool(const std::string& name, bool value) const 
{
	glUniform1i(glGetUniformLocation(id_, name.c_str()), (int)value);
}

void Shader::setInt(const std::string& name, int value) const 
{
	glUniform1i(glGetUniformLocation(id_, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) const 
{
	glUniform1f(glGetUniformLocation(id_, name.c_str()), value);
}

void Shader::setVec3f(const std::string& name, glm::vec3& vec3)const
{
	glUniform3fv(glGetUniformLocation(id_, name.c_str()), 1, glm::value_ptr(vec3));
}

void Shader::setVec3f(const std::string& name, float v0, float v1, float v2) const
{
	glUniform3f(glGetUniformLocation(id_, name.c_str()), v0, v1, v2);
}

void Shader::setVec4f(const std::string& name, glm::vec4& vec4)const
{
	glUniform4fv(glGetUniformLocation(id_, name.c_str()), 1, glm::value_ptr(vec4));
}

void Shader::setVec4f(const std::string& name, float v0, float v1, float v2, float v3) const
{
	glUniform4f(glGetUniformLocation(id_, name.c_str()), v0, v1, v2, v3);
}

void Shader::setMat2f(const std::string& name, glm::mat2& mat2) const
{
	glUniformMatrix2fv(glGetUniformLocation(id_, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat2));
}

void Shader::setMat2x3f(const std::string& name, glm::mat2x3& mat2x3) const
{
	glUniformMatrix2x3fv(glGetUniformLocation(id_, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat2x3));
}

void Shader::setMat4f(const std::string& name, glm::mat4& mat4)const
{
	glUniformMatrix4fv(glGetUniformLocation(id_, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat4));
}

void Shader::setTexUnit(const std::string& sampler, int unit) const
{
	glUniform1i(glGetUniformLocation(id_, sampler.c_str()), unit);
}
