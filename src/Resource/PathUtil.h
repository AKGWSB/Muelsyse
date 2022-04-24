#pragma once

#include <iostream>
#include <fstream>
#include "stdlib.h"
#include <direct.h>

class PathUtil
{
public:
#define ASSET_ROOT "asset/"
#define Shader_ROOT "shaders/"

	static LPCWSTR StringToLPCWSTR(std::string orig)
	{
		size_t origsize = orig.length() + 1;
		const size_t newsize = 100;
		size_t convertedChars = 0;
		wchar_t* wcstring = (wchar_t*)malloc(sizeof(wchar_t) * (orig.length() - 1));
		mbstowcs_s(&convertedChars, wcstring, origsize, orig.c_str(), _TRUNCATE);

		return wcstring;
	}

	static std::string GetAssetFullPath(std::string path)
	{
		char cwd[512];
		_getcwd(cwd, 512);

		std::string currentDir = std::string(cwd);
		return currentDir + "/" + ASSET_ROOT + path;
	}


	static std::string GetShaderFullPath(std::string path)
	{
		char cwd[512];
		_getcwd(cwd, 512);

		std::string currentDir = std::string(cwd);
		return currentDir + "/" + Shader_ROOT + path;
	}
};
