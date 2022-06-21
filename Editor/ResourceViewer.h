#pragma once

#include <memory>
#include <map>
#include <string>
#include "../Resource/Texture2D.h"
#include "../Rendering/RenderTexture.h"

class ResourceViewer
{
public:
	// name --> view
	// view tex own by Texture2D and RenderTexture class
	std::map<std::string, Texture2D*> textureViewMap;
	std::map<std::string, Texture2D*> renderTextureViewMap;

	// view tex own by ResourceViewer class
	std::map<std::string, RenderTexture*> meshViewMap;
	std::map<std::string, RenderTexture*> materialViewMap;

	bool isOpen = false;
	bool isSelect = false;
	std::string selectResourceName;

	ResourceViewer();
	~ResourceViewer();

	// load resource from disk
	// store into g_xxxResourcePool, eg: texture, mesh, material
	void Init();	
	void RenderUI();

	void SelectAndClose(std::string selectName);
	void Open();
};
