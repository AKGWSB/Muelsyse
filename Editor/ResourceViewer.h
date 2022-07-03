#pragma once

#include <memory>
#include <map>
#include <string>
#include <functional>
#include "../Resource/Texture2D.h"
#include "../Rendering/RenderTexture.h"

enum ResourceViewerOpenMode
{
	ETexture2D,
	EShader,
	EMaterial,
	EMesh
};

class ResourceViewer
{

	// name --> view
	// view tex own by Texture2D and RenderTexture class
	static std::map<std::string, Texture2D*> textureViewMap;
	static std::map<std::string, Texture2D*> renderTextureViewMap;
	
	// view tex own by ResourceViewer class
	static std::map<std::string, RenderTexture*> meshViewMap;
	static std::map<std::string, RenderTexture*> materialViewMap;

	// has not view
	static std::vector<std::string> shaderList;

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

	static ResourceViewerOpenMode currentMode;

	ResourceViewer();
	~ResourceViewer();

	static void ShowResourceView(std::string name, Texture2D* view, bool bFlip = true);

	static void TextureMode();
	static void ShaderMode();
	static void MaterialMode();
	static void MeshMode();

public:
	// load resource from disk
	// store into g_xxxResourcePool, eg: texture, mesh, material
	static void Init();	
	static void RenderUI();
	static void ReleaseView();

	static bool GetSelectResourceName(std::string& o_resourceName, std::string buttonName);
	static void Open(std::string buttonName, ResourceViewerOpenMode mode);
};
