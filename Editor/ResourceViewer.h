#pragma once

#include <memory>
#include <map>
#include <string>
#include <functional>
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

	// pass a name for fake callback
	// we have many buttons that can call our "ResourceViewer" to Open
	// but when "ResourceViewer" finish a selection, 
	// the filepath (or filename) should pass to the specific button (caller)
	// ImGui is a no block API, each button will query the result in the loop, 
	// but "ResourceViewer" don't know who is the "caller"
	// cause ImGUI use name to identify component, so "ResourceViewer" also use name to identify buttons
	std::string bindButtonName;		

	std::string selectResourceName;

	ResourceViewer();
	~ResourceViewer();

	// load resource from disk
	// store into g_xxxResourcePool, eg: texture, mesh, material
	void Init();	
	void RenderUI();

	bool GetSelectResourceName(std::string& o_resourceName, std::string buttonName);
	void Open(std::string buttonName);
};
