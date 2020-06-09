#ifndef __SHADER_HPP__
#define __SHADER_HPP__

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <GL/gl3w.h>
#include <glm/glm.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader
{
public:
	// 程序 id
	unsigned int id_;
	// 构造器读取并构建着色器
	Shader(const char* vsPath, const char* fsPath, const char* gsPath = nullptr);
	// 使用/激活函数
	void use();
	// uniform工具函数
	void setBool(const std::string& name, bool value)const;
	void setInt(const std::string& name, int value)const;
	void setFloat(const std::string& name, float value)const;
	void setVec3f(const std::string& name, glm::vec3& vec3)const;
	void setVec3f(const std::string& nane, float v0, float v1, float v2) const;
	void setVec4f(const std::string& name, glm::vec4& vec4)const;
	void setVec4f(const std::string& name, float v0, float v1, float v2, float v3) const;
	void setMat2f(const std::string& name, glm::mat2& mat2)const;
	void setMat2x3f(const std::string& name, glm::mat2x3& mat2x3) const;
	void setMat4f(const std::string& name, glm::mat4& mat4)const;

	void setTexUnit(const std::string& sampler, int unit) const;
};

#endif // !__SHADER_HPP__