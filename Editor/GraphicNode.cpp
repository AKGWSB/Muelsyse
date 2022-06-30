#include "GraphicNode.h"
#include "ResourceViewer.h"

#include "../Library/imgui/imgui.h"
#include "../Library/imgui/imgui_impl_win32.h"
#include "../Library/imgui/imgui_impl_dx12.h"
#include "../Library/imgui/imnodes.h"

#include "../Resource/Texture2D.h"

#include <algorithm>
#include <iostream>

// ----------------------------------------------------------------------- //

Node::Node()
{

}

Node::~Node()
{

}

void Node::RenderPins()
{
    // title
    ImNodes::BeginNodeTitleBar();
    ImGui::TextUnformatted(name.c_str());
    ImNodes::EndNodeTitleBar();

    // in pins
    for (auto& ipin : inputPins)
    {
        ImNodes::BeginInputAttribute(ipin.id);
        ImGui::Text(ipin.name.c_str());
        ImNodes::EndInputAttribute();
    }
    ImGui::Dummy(ImVec2(0.0f, 10.0f));

    // out pins
    for (auto& opin : outputPins)
    {
        ImNodes::BeginOutputAttribute(opin.id);
        ImGui::Text(opin.name.c_str());
        ImNodes::EndOutputAttribute();
    }
    ImGui::Dummy(ImVec2(0.0f, 10.0f));
    
    /*
    int n_min = min(inputPins.size(), outputPins.size());
    int n_max = max(inputPins.size(), outputPins.size());
    int i = 0;
    for (; i < n_min; i++)
    {
        auto& ipin = inputPins[i];
        auto& opin = outputPins[i];

        ImNodes::BeginInputAttribute(ipin.id);
        ImGui::Text(ipin.name.c_str());
        ImGui::SameLine(ImGui::GetWindowWidth() - 30);
        ImNodes::EndInputAttribute();

        ImNodes::BeginOutputAttribute(opin.id);
        ImGui::Text(opin.name.c_str());
        ImNodes::EndOutputAttribute();
    }*/
}

void Node::Render()
{
    ImNodes::BeginNode(id);

    RenderPins();

    ImNodes::EndNode();
}

// ----------------------------------------------------------------------- //

TextureNode::TextureNode()
{
    Pin op;
    op.name = "read";

    outputPins.push_back(op);
}

TextureNode::~TextureNode()
{

}

void TextureNode::Render()
{
    ImNodes::BeginNode(id);
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
    std::string buttonID = buttonName + std::to_string(id); // identifier
    if (ImGui::Button(buttonName.c_str()))
    {
        ResourceViewer::Open(buttonID.c_str());
    }
    ResourceViewer::GetSelectResourceName(resourceName, buttonID);

    ImGui::Dummy(ImVec2(0.0f, 10.0f));
    ImNodes::EndNode();
}

// ----------------------------------------------------------------------- //

RenderTextureNode::RenderTextureNode()
{

    Pin ip;
    ip.name = "write";
    inputPins.push_back(ip);

    Pin op;
    op.name = "read";
    outputPins.push_back(op);
}

RenderTextureNode::~RenderTextureNode()
{

}

void RenderTextureNode::Render()
{
    ImNodes::BeginNode(id);
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

// ----------------------------------------------------------------------- //

RenderPassNode::RenderPassNode()
{
    Pin op;
    op.name = "RT 0";
    outputPins.push_back(op);
}

RenderPassNode::~RenderPassNode()
{

}

void RenderPassNode::Render()
{
    ImNodes::BeginNode(id);
    RenderPins();

    ImGui::SameLine();
    if (ImGui::Button("Add output"))
    {

    }
    
    ImGui::Dummy(ImVec2(0.0f, 10.0f));
    ImNodes::EndNode();
}

// ----------------------------------------------------------------------- //

BlitPassNode::BlitPassNode()
{
    Pin ip;
    ip.name = "src tex 0";
    inputPins.push_back(ip);

    Pin op;
    op.name = "dst tex 0";
    outputPins.push_back(op);
}

BlitPassNode::~BlitPassNode()
{

}

void BlitPassNode::Render()
{
    ImNodes::BeginNode(id);
    RenderPins();

    if (ImGui::Button("Add input"))
    {

    }
    ImGui::SameLine();
    if (ImGui::Button("Add output"))
    {

    }

    ImNodes::EndNode();
}
