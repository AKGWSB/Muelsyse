#include "ResourceViewer.h"
#include <filesystem>

#include "../Library/imgui/imgui.h"
#include "../Library/imgui/imgui_impl_win32.h"
#include "../Library/imgui/imgui_impl_dx12.h"

#include "Actor.h"
#include "../Rendering/RenderPass.h"
#include "../Core/GraphicContex.h"

using std::filesystem::recursive_directory_iterator;


std::map<std::string, Texture2D*> ResourceViewer::textureViewMap;
std::map<std::string, Texture2D*> ResourceViewer::renderTextureViewMap;
std::map<std::string, RenderTexture*> ResourceViewer::meshViewMap;
std::map<std::string, RenderTexture*> ResourceViewer::materialViewMap;
bool ResourceViewer::isOpen = false;
bool ResourceViewer::isSelect = false;
std::string ResourceViewer::bindButtonName;
std::string ResourceViewer::selectResourceName;
ResourceViewerOpenMode ResourceViewer::currentMode;
std::vector<std::string> ResourceViewer::shaderList;

ResourceViewer::ResourceViewer()
{

}

ResourceViewer::~ResourceViewer()
{
	
}

void ResourceViewer::Init()
{
	// scan dir
	for (const auto& file : recursive_directory_iterator("Asset/"))
	{
		std::string filepath = file.path().string();

		// replace '\\' by '/'
		std::string::size_type pos = 0;
		while ((pos = filepath.find('\\', pos)) != std::string::npos)
		{
			filepath.replace(pos, 1,  "/");
			pos = pos + 1;
		}

		std::string extname = filepath.substr(filepath.find_last_of(".") + 1);

		// is dir
		if (filepath == extname) continue;

		// image
		if (extname == "jpg" || extname == "png")
		{
			textureViewMap[filepath] = Texture2D::Find(filepath);
		}

		// mesh
		if (extname == "obj")
		{
			Actor* tempActor = new Actor();
			tempActor->mesh = Mesh::Find(filepath);
			tempActor->material = new Material();
			tempActor->material->shader = Shader::Find("Shaders/lit.hlsl");

			RenderTexture* tempRT = new RenderTexture(128, 128, DXGI_FORMAT_R8G8B8A8_UNORM);
			
			RenderPass* tempPass = new RenderPass();
			tempPass->renderTargets = { tempRT };

			Camera* tempCamera = new Camera(128, 128);
			tempCamera->SetPosition(XMFLOAT3(1, 1, -1));
			tempCamera->SetTarget(XMFLOAT3(0, 0, 0));

			// render to texture
			GraphicContex::PreRender();
			GraphicContex::RenderLoop(tempCamera, tempPass, { tempActor });
			GraphicContex::PostRender();

			// hold the view, until program exit
			meshViewMap[filepath] = tempRT;

			delete tempActor->material;
			delete tempActor;
			delete tempPass;
			delete tempCamera;
		}
	}

	for (const auto& file : recursive_directory_iterator("Shaders/"))
	{
		std::string filepath = file.path().string();

		// replace '\\' by '/'
		std::string::size_type pos = 0;
		while ((pos = filepath.find('\\', pos)) != std::string::npos)
		{
			filepath.replace(pos, 1, "/");
			pos = pos + 1;
		}

		std::string extname = filepath.substr(filepath.find_last_of(".") + 1);

		// shader
		if (extname == "hlsl")
		{
			shaderList.push_back(filepath);
		}
	}
}

void ResourceViewer::ShowResourceView(std::string name, Texture2D* view, bool bFlip)
{
	ImGui::Text(name.c_str());

	ImTextureID id = (ImTextureID)(D3D12_GPU_DESCRIPTOR_HANDLE(view->srvGpuHandle).ptr);

	ImVec2 size = ImVec2(128, 128);							// Size of the image we want to make visible
	ImVec2 uv0 = ImVec2(0.0f, 1.0f);                        // UV coordinates for lower-left
	ImVec2 uv1 = ImVec2(1, 0);								// UV coordinates for (32,32) in our texture
	ImVec4 bg_col = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);         // Black background
	ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);       // No tint

	if (!bFlip)
	{
		uv0 = ImVec2(0, 0);
		uv1 = ImVec2(1, 1);
	}

	if (ImGui::ImageButton(id, size, uv0, uv1, 3, bg_col, tint_col))
	{
		isOpen = false;
		isSelect = true;
		selectResourceName = name;
	}
	ImGui::Dummy(ImVec2(0.0f, 10.0f));
}

void ResourceViewer::TextureMode()
{
	for (auto& p : textureViewMap)
	{
		auto& name = p.first;
		auto& view = p.second;

		ResourceViewer::ShowResourceView(name, view);
	}
}

void ResourceViewer::ShaderMode()
{
	for (int i=0; i<shaderList.size(); i++)
	{
		auto& shaderName = shaderList[i];
		std::string buttonID = "Select##" + std::to_string(i);

		if (ImGui::Button(buttonID.c_str()))
		{
			isOpen = false;
			isSelect = true;
			selectResourceName = shaderName;
		}
		ImGui::SameLine();
		ImGui::Text(shaderName.c_str());
		ImGui::Dummy(ImVec2(0.0f, 10.0f));
	}
}

void ResourceViewer::MaterialMode()
{

}

void ResourceViewer::MeshMode()
{
	for (auto& p : meshViewMap)
	{
		auto& name = p.first;
		auto& view = p.second;

		ResourceViewer::ShowResourceView(name, view, false);
	}
}

void ResourceViewer::RenderUI()
{
	if (isOpen == false) return;

	ImGui::OpenPopup("Resource Viewer Pannel");

	// Always center this window when appearing
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal("Resource Viewer Pannel", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		if (currentMode == ResourceViewerOpenMode::EMaterial)
		{
			ResourceViewer::MaterialMode();
		}
		if (currentMode == ResourceViewerOpenMode::EMesh)
		{
			ResourceViewer::MeshMode();
		}
		if (currentMode == ResourceViewerOpenMode::EShader)
		{
			ResourceViewer::ShaderMode();
		}
		if (currentMode == ResourceViewerOpenMode::ETexture2D)
		{
			ResourceViewer::TextureMode();
		}

		ImGui::EndPopup();
	}
}

void ResourceViewer::ReleaseView()
{
	for (auto& p : meshViewMap)
	{
		delete p.second;
	}

	for (auto& p : materialViewMap)
	{
		delete p.second;
	}
}

bool ResourceViewer::GetSelectResourceName(std::string& o_resourceName, std::string buttonName)
{
	// not ready for query
	if (isOpen) return false;
	if (!isSelect) return false;

	// check if the caller try to fetch the select resource
	if (buttonName != bindButtonName) return false;

	o_resourceName = selectResourceName;
	isSelect = false;

	return true;
}

void ResourceViewer::Open(std::string buttonName, ResourceViewerOpenMode mode)
{
	bindButtonName = buttonName;
	isOpen = true;
	isSelect = false;
	currentMode = mode;
}
