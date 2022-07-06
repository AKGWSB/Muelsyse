#pragma once

#include <memory>
#include <map>
#include <string>
#include <functional>

#include "../Resource/Texture2D.h"
#include "../Rendering/RenderTexture.h"
#include "../Rendering/RenderPass.h"
#include "../Rendering/Camera.h"
#include "Actor.h"

enum ResourceViewerOpenMode
{
	ETexture2D,
	EShader,
	EMaterial,
	EMesh,
	EInputText
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
	static bool isOpen;
	static bool isSelect;

	// resource to dynamic update resource's view
	// cause we use the g_commandList, so our update must between "GraphicContex::PreRender()" and "GraphicContex::PostRender()"
	static std::unique_ptr<Actor> m_tempActor;
	static std::unique_ptr<RenderPass> m_tempPass;
	static std::unique_ptr<Camera> m_tempCamera;

	ResourceViewer();
	~ResourceViewer();

	static void ShowResourceView(std::string name, Texture2D* view, bool bFlip = true);

	static void TextureMode();
	static void ShaderMode();
	static void MaterialMode();
	static void MeshMode();

	// allow user to input a text, avoid
	static void InputTextMode();

	// material's view may change while runtime, compute it ever frame
	static void UpdateMaterialView(std::string filepath, bool singleFrame = true);

public:
	// load resource from disk
	// store into g_xxxResourcePool, eg: texture, mesh, material
	static void Init();	
	static void RenderUI();
	static void ReleaseView();

	static bool GetSelectResourceName(std::string& o_resourceName, std::string buttonName);
	static void Open(std::string buttonName, ResourceViewerOpenMode mode);

	// get snap by name and resource type
	static Texture2D* GetResourceViewByName(std::string name, ResourceViewerOpenMode mode);

	// render dynamic (runtime) resource view (eg. change material's texture, the view will update)
	// must call before render UI
	static void RenderResourceView();
};
