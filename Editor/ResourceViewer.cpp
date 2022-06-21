#include "ResourceViewer.h"
#include <filesystem>

#include "../Library/imgui/imgui.h"
#include "../Library/imgui/imgui_impl_win32.h"
#include "../Library/imgui/imgui_impl_dx12.h"

using std::filesystem::recursive_directory_iterator;

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

	ImGui::Begin("Resource Viewer", 0, 0);

	for (auto& p : textureViewMap)
	{
		auto& name = p.first;
		auto& view = p.second;

		ImGui::Text(name.c_str());

		ImTextureID id = (ImTextureID)(D3D12_GPU_DESCRIPTOR_HANDLE(view->srvGpuHandle).ptr);

		ImVec2 size = ImVec2(128.0f, 128.0f);					// Size of the image we want to make visible
		ImVec2 uv0 = ImVec2(0.0f, 1.0f);                        // UV coordinates for lower-left
		ImVec2 uv1 = ImVec2(1, 0);								// UV coordinates for (32,32) in our texture
		ImVec4 bg_col = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);         // Black background
		ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);       // No tint

		if (ImGui::ImageButton(id, size, uv0, uv1, 3, bg_col, tint_col))
		{
			SelectAndClose(name);
		}
	}

	ImGui::End();
}

void ResourceViewer::Open()
{
	isOpen = true;
	isSelect = false;
	selectResourceName = "";
}

void ResourceViewer::SelectAndClose(std::string selectName)
{
	isOpen = false;
	isSelect = true;
	selectResourceName = selectName;
}
