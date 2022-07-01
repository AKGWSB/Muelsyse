#include "GraphNode.h"
#include "ResourceViewer.h"
#include "GraphEditor.h"

#include "../Library/imgui/imgui.h"
#include "../Library/imgui/imgui_impl_win32.h"
#include "../Library/imgui/imgui_impl_dx12.h"
#include "../Library/imgui/imnodes.h"

#include <random>
#include <fstream>
#include <iostream>
#include <sstream>


// ----------------------------------------------------------------------- //

Pin::Pin()
{

}

Pin::Pin(PinType t, std::string n) :
    type(t),
    name(n)
{

}

Json Pin::to_json() const
{
    return Json::object{
        { "type", type },
        { "name", name },
        { "runtimeID", runtimeID }
    };
}

// ----------------------------------------------------------------------- //

Node::Node()
{

}

void Node::RenderPins()
{
    // in pins
    for (auto& id : inputPins)
    {
        Pin& ipin = GraphEditor::GetPinByID(id);
        ImNodes::BeginInputAttribute(ipin.runtimeID);
        ImGui::Text(ipin.name.c_str());
        ImNodes::EndInputAttribute();
    }

    // out pins
    for (auto& id : outputPins)
    {
        Pin& opin = GraphEditor::GetPinByID(id);
        ImNodes::BeginOutputAttribute(opin.runtimeID);
        ImGui::Text(opin.name.c_str());
        ImNodes::EndOutputAttribute();
    }
}

void Node::Render()
{
    if (type == NodeType::RenderPassNode)
    {
        ImNodes::PushColorStyle(ImNodesCol_TitleBar, IM_COL32(75, 120, 25, 255));
        ImNodes::PushColorStyle(ImNodesCol_TitleBarHovered, IM_COL32(95, 150, 55, 255));
        ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, IM_COL32(95, 150, 55, 255));

        ImNodes::BeginNode(runtimeID);
        ImNodes::BeginNodeTitleBar();
        ImGui::TextUnformatted("Render Pass");
        ImGui::SameLine();
        if (ImGui::Button("X"))
        {
            GraphEditor::RemoveNode(runtimeID);
        }
        ImNodes::EndNodeTitleBar();

        RenderPins();

        ImNodes::PopColorStyle();
        ImNodes::PopColorStyle();
        ImNodes::PopColorStyle();

        ImNodes::EndNode();
    }

    if (type == NodeType::BlitPassNode)
    {
        ImNodes::PushColorStyle(ImNodesCol_TitleBar, IM_COL32(75, 120, 25, 255));
        ImNodes::PushColorStyle(ImNodesCol_TitleBarHovered, IM_COL32(95, 150, 55, 255));
        ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, IM_COL32(95, 150, 55, 255));

        ImNodes::BeginNode(runtimeID);
        ImNodes::BeginNodeTitleBar();
        ImGui::TextUnformatted("Blit Pass");
        ImGui::SameLine();
        if (ImGui::Button("X"))
        {
            GraphEditor::RemoveNode(runtimeID);
        }
        ImNodes::EndNodeTitleBar();

        RenderPins();

        ImNodes::PopColorStyle();
        ImNodes::PopColorStyle();
        ImNodes::PopColorStyle();

        ImNodes::EndNode();
    }

    if (type == NodeType::RenderTextureNode)
    {
        ImNodes::BeginNode(runtimeID);
        ImNodes::BeginNodeTitleBar();
        ImGui::TextUnformatted("Render Texture");
        ImGui::SameLine();
        if (ImGui::Button("X"))
        {
            GraphEditor::RemoveNode(runtimeID);
        }
        ImNodes::EndNodeTitleBar();

        RenderPins();

        // convert to char* array
        std::vector<std::string> renderTextureList = RenderTexture::GetNameList();
        renderTextureList.insert(renderTextureList.begin(), "None");
        int n_rt = renderTextureList.size();
        std::vector<const char*> chars;
        for (auto& s : renderTextureList)
        {
            chars.push_back(s.c_str());
        }
        // draw select box
        static int rtNodeSelectIndex = 0;
        ImGui::PushItemWidth(128.0f);
        ImGui::Combo("##rt_combo", &rtNodeSelectIndex, chars.data(), n_rt);
        ImGui::PopItemWidth();
        resourceName = renderTextureList[rtNodeSelectIndex];

        // not select
        if (resourceName != "None")
        {
            Texture2D* tex = RenderTexture::Find(resourceName);
            auto hdptr = D3D12_GPU_DESCRIPTOR_HANDLE(tex->srvGpuHandle).ptr;
            ImGui::Image((ImTextureID)hdptr, ImVec2(128, 128), ImVec2(0, 0), ImVec2(1, 1));
        }

        ImGui::Dummy(ImVec2(0.0f, 10.0f));
        ImNodes::EndNode();
    }

    if (type == NodeType::Texture2DNode)
    {
        ImNodes::BeginNode(runtimeID);
        ImNodes::BeginNodeTitleBar();
        ImGui::TextUnformatted("Texture2D");
        ImGui::SameLine();
        if (ImGui::Button("X"))
        {
            GraphEditor::RemoveNode(runtimeID);
        }
        ImNodes::EndNodeTitleBar();

        RenderPins();

        // not select
        if (resourceName.length() != 0)
        {
            Texture2D* tex = Texture2D::Find(resourceName);
            auto hdptr = D3D12_GPU_DESCRIPTOR_HANDLE(tex->srvGpuHandle).ptr;
            ImGui::Image((ImTextureID)hdptr, ImVec2(128, 128), ImVec2(0, 1), ImVec2(1, 0));
        }

        // button
        std::string buttonName = "select texture";
        std::string buttonID = buttonName + std::to_string(runtimeID); // identifier
        if (ImGui::Button(buttonName.c_str()))
        {
            ResourceViewer::Open(buttonID.c_str());
        }
        ResourceViewer::GetSelectResourceName(resourceName, buttonID);

        ImNodes::EndNode();
    }

    if (type == NodeType::ScreenTargetNode)
    {

    }
}

Json Node::to_json() const
{
    return Json::object{
        { "type", type },
        { "name", name },
        { "runtimeID", runtimeID },
        { "resourceName", resourceName },
        { "inputPins", inputPins },
        { "outputPins", outputPins },
        { "position", Json::array { position.x, position.y} }
    };
}

// ----------------------------------------------------------------------- //

Link::Link()
{

}

Json Link::to_json() const
{
    return Json::object{
        { "srcNode", srcNode },
        { "dstNode", dstNode },
        { "srcPin", srcPin },
        { "dstPin", dstPin },
        { "runtimeID", runtimeID }
    };
}

// ----------------------------------------------------------------------- //
