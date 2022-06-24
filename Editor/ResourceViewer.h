#pragma once

#include <memory>
#include <map>
#include <string>
#include <functional>
#include "../Resource/Texture2D.h"
#include "../Rendering/RenderTexture.h"

class ResourceViewer
{

	// name --> view
	// view tex own by Texture2D and RenderTexture class
	static std::map<std::string, Texture2D*> textureViewMap;
	static std::map<std::string, Texture2D*> renderTextureViewMap;

	// view tex own by ResourceViewer class
	static std::map<std::string, RenderTexture*> meshViewMap;
	static std::map<std::string, RenderTexture*> materialViewMap;

	static bool isOpen;
	static bool isSelect;

	// pass a name for fake callback
	// we have many buttons that can call our "ResourceViewer" to Open
	// but when "ResourceViewer" finish a selection, 
	// the filepath (or filename) should pass to the specific button (caller)
	// ImGui is a no block API, each button will query the result in the loop, 
	// but "ResourceViewer" don't know who is the "caller"
	// cause ImGUI use name to identify component, so "ResourceViewer" also use name to identify buttons
	static std::string bindButtonName;

	static std::string selectResourceName;

	ResourceViewer();
	~ResourceViewer();

public:
	// load resource from disk
	// store into g_xxxResourcePool, eg: texture, mesh, material
	static void Init();	
	static void RenderUI();

	static bool GetSelectResourceName(std::string& o_resourceName, std::string buttonName);
	static void Open(std::string buttonName);
};
