#include "shader.hpp"

using std::string;
using std::ifstream;
using std::stringstream;
using std::cout;
using std::endl;

Shader::Shader(const char* vsPath, const char* fsPath, const char* gsPath)
{
	// 1、从文件路径中获取顶点/片段着色器
	string vsCode, fsCode, gsCode;
	ifstream vsFile, fsFile, gsFile;
	// 保证ifstream对象可以抛出异常
	vsFile.exceptions(ifstream::failbit | ifstream::badbit);
	fsFile.exceptions(ifstream::failbit | ifstream::badbit);
	gsFile.exceptions(ifstream::failbit | ifstream::badbit);
	try 
	{
		// 打开文件
		vsFile.open(vsPath);
		fsFile.open(fsPath);

		// 读取文件的缓冲内容到数据流中
		stringstream vsss, fsss, gsss;
		vsss << vsFile.rdbuf();
		fsss << fsFile.rdbuf();
		// 关闭文件处理器
		vsFile.close();
		fsFile.close();
		// 转换数据流到string
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


	// 2、编写着色器
	unsigned int vertex, fragment, geometry;
	int success;
	char infoLog[512];
	// 顶点着色器
	vertex = glCreateShader(GL_VERTEX_SHADER);
	const char* vscode = vsCode.c_str();
	glShaderSource(vertex, 1, &vscode, nullptr); // 第二个参数指出要传递多少个字符串
	glCompileShader(vertex);
		// 打印编译错误
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertex, 512, nullptr, infoLog);
		cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << endl;
	}
	// 片段着色器
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	const char* fscode = fsCode.c_str();
	glShaderSource(fragment, 1, &fscode, nullptr);
	glCompileShader(fragment);
		// 打印编译错误
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragment, 512, nullptr, infoLog);
		cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << endl;
	}
	// 几何着色器
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
	// 链接着色器
	id_ = glCreateProgram();
	glAttachShader(id_, vertex);
	glAttachShader(id_, fragment);
	if (gsPath != nullptr)
		glAttachShader(id_, geometry);
	glLinkProgram(id_);
		// 打印链接错误
	glGetProgramiv(id_, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(id_, 512, nullptr, infoLog);
		cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << endl;
	}
	// 删除着色器
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
