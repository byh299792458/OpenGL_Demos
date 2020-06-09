#ifndef __FILESYSTEM_HPP__
#define __FILESYSTEM_HPP__

#include <string>
#include <cstdlib>
#include <functional>
#include "root_directory.h"

class FileSystem 
{
private:

	using Builder = std::function<std::string(const std::string&)>;

	static std::string const& getRoot(void) 
	{
		static char const* envRoot = getenv("OGL_DEMO_ROOT_PATH");
		static char const* givenRoot = (envRoot != nullptr ? envRoot : ogl_demo_root);
		static std::string root = (givenRoot != nullptr ? givenRoot : "");
		return root;
	}

	static Builder getPathBuilder(void)
	{
		if (getRoot() != "")
			return FileSystem::getPathRelativeRoot;
		else
			return FileSystem::getPathRelativeBinary;
	}

	// 相对于工程根路径
	static std::string getPathRelativeRoot(const std::string& path)
	{
		return getRoot() + std::string("/") + path;
	}

	// 相对于工程二进制文件路径
	static std::string getPathRelativeBinary(const std::string& path)
	{
		return "../../../" + path;
	}

public:

	static std::string getPath(const std::string& path) 
	{
		static Builder path_builder = getPathBuilder();
		return path_builder(path);
	}

};

#endif // !__FILESYSTEM_HPP__

