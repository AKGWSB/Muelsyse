#include "ResourceViewer.h"
#include <filesystem>

#include "../Library/imgui/imgui.h"
#include "../Library/imgui/imgui_impl_win32.h"
#include "../Library/imgui/imgui_impl_dx12.h"

using std::filesystem::recursive_directory_iterator;


std::map<std::string, Texture2D*> ResourceViewer::textureViewMap;
std::map<std::string, Texture2D*> ResourceViewer::renderTextureViewMap;
std::map<std::string, RenderTexture*> ResourceViewer::meshViewMap;
std::map<std::string, RenderTexture*> ResourceViewer::materialViewMap;
bool ResourceViewer::isOpen = false;
bool ResourceViewer::isSelect = false;
std::string ResourceViewer::bindButtonName;
std::string ResourceViewer::selectResourceName;

ResourceViewer::ResourceViewer()
{

}

ResourceViewer::~ResourceViewer()
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
		for (auto& p : textureViewMap)
		{
			auto& name = p.first;
			auto& view = p.second;

			ImGui::Text(name.c_str());

			ImTextureID id = (ImTextureID)(D3D12_GPU_DESCRIPTOR_HANDLE(view->srvGpuHandle).ptr);

			ImVec2 size = ImVec2(64.0f, 64.0f);					// Size of the image we want to make visible
			ImVec2 uv0 = ImVec2(0.0f, 1.0f);                        // UV coordinates for lower-left
			ImVec2 uv1 = ImVec2(1, 0);								// UV coordinates for (32,32) in our texture
			ImVec4 bg_col = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);         // Black background
			ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);       // No tint

			if (ImGui::ImageButton(id, size, uv0, uv1, 3, bg_col, tint_col))
			{
				isOpen = false;
				isSelect = true;
				selectResourceName = name;
			}
			ImGui::Dummy(ImVec2(0.0f, 10.0f));
		}

		ImGui::EndPopup();
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

void ResourceViewer::Open(std::string buttonName)
{
	bindButtonName = buttonName;
	isOpen = true;
	isSelect = false;
}
