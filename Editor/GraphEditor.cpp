#include "GraphEditor.h"
#include "ResourceViewer.h"

#include "../Library/imgui/imgui.h"
#include "../Library/imgui/imgui_impl_win32.h"
#include "../Library/imgui/imgui_impl_dx12.h"
#include "../Library/imgui/imnodes.h"

#include <random>

// ----------------------------------------------------------------------- //

Pin::Pin()
{

}

Pin::Pin(PinType t, std::string n) : 
    type(t), 
    name(n)
{

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
    ImNodes::BeginNode(runtimeID);

    if (type == NodeType::RenderPassNode)
    {
        ImNodes::BeginNodeTitleBar();
        ImGui::TextUnformatted(name.c_str());
        ImNodes::EndNodeTitleBar();

        RenderPins();
    }

    if (type == NodeType::BlitPassNode)
    {
        ImNodes::BeginNodeTitleBar();
        ImGui::TextUnformatted(name.c_str());
        ImNodes::EndNodeTitleBar();

        RenderPins();
    }

    if (type == NodeType::RenderTextureNode)
    {
        ImNodes::BeginNodeTitleBar();
        ImGui::TextUnformatted(name.c_str());
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
    }

    if (type == NodeType::Texture2DNode)
    {
        ImNodes::BeginNodeTitleBar();
        ImGui::TextUnformatted(name.c_str());
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
    }

    if (type == NodeType::ScreenTargetNode)
    {

    }

    ImNodes::EndNode();
}

// ----------------------------------------------------------------------- //

std::map<int, Node> GraphEditor::nodePool;
std::map<int, Pin> GraphEditor::pinPool;

GraphEditor::GraphEditor()
{
    ImNodes::CreateContext();
}

GraphEditor::~GraphEditor()
{
    ImNodes::DestroyContext();
}

void GraphEditor::Init()
{
    Node basePassNode;
    basePassNode.type = NodeType::RenderPassNode;
    basePassNode.name = "basePass";
    basePassNode.outputPins = { GraphEditor::RegisterNewPin(Pin(PinType::RenderTargetOutput, "RT_0")) };
    GraphEditor::RegisterNewNode(basePassNode);

    Node blitPassNode;
    blitPassNode.type = NodeType::BlitPassNode;
    blitPassNode.name = "blitPass";
    blitPassNode.inputPins = { GraphEditor::RegisterNewPin(Pin(PinType::RenderTargetOutput, "MainTex_0")) };
    blitPassNode.outputPins = { GraphEditor::RegisterNewPin(Pin(PinType::RenderTargetOutput, "RT_0")) };
    GraphEditor::RegisterNewNode(blitPassNode);

    Node rtNode;
    rtNode.type = NodeType::RenderTextureNode;
    rtNode.name = "RT_BasePass";
    rtNode.inputPins = { GraphEditor::RegisterNewPin(Pin(PinType::TextureWrite, "write")) };
    rtNode.outputPins = { GraphEditor::RegisterNewPin(Pin(PinType::TextureRead, "read")) };
    GraphEditor::RegisterNewNode(rtNode);

    Node texNode;
    texNode.type = NodeType::Texture2DNode;
    texNode.name = "Tex_0";
    texNode.outputPins = { GraphEditor::RegisterNewPin(Pin(PinType::TextureRead, "read")) };
    GraphEditor::RegisterNewNode(texNode);
}

void GraphEditor::RenderUI()
{
    ImGui::SetNextWindowBgAlpha(0.35f);
    ImGui::Begin("Pipeline editor");

    if (ImGui::Button("Save Pipeline"))
    {
        SaveToFile("Asset/e_pipeline.pipeline");
    }

    ImNodes::BeginNodeEditor();
    
    // render nodes
    for(auto& p : nodePool)
    {
        Node& node = p.second;
        node.Render();
    }

    /**/
    // render links
    ImNodes::PushColorStyle(ImNodesCol_Link, IM_COL32(11, 109, 191, 255));
    ImNodes::PushColorStyle(ImNodesCol_LinkHovered, IM_COL32(255, 255, 128, 255));
    ImNodes::PushColorStyle(ImNodesCol_LinkSelected, IM_COL32(81, 148, 204, 255));
    for (int i = 0; i < links.size(); ++i)
    {
        Link& link = links[i];

        int start_pin_id = link.srcPin;
        int end_pin_id = link.dstPin;
        int link_id = i;

        ImNodes::Link(link_id, start_pin_id, end_pin_id);
    }
    ImNodes::PopColorStyle();
    ImNodes::PopColorStyle();
    ImNodes::PopColorStyle();

    ImNodes::EndNodeEditor();
    ImGui::End();

    /**/
    //create links
    int start_pin_id, end_pin_id;
    if (ImNodes::IsLinkCreated(&start_pin_id, &end_pin_id))
    {
        Link newLink;
        newLink.srcPin = start_pin_id;
        newLink.dstPin = end_pin_id;
        links.push_back(newLink);
    }

    // delete links
    int hovered_link_id;
    if (ImNodes::IsLinkHovered(&hovered_link_id))
    {
        ImGui::SetTooltip("Press \"Delete\" To Remove this Link");
        
        if (ImGui::IsKeyPressed(ImGuiKey_::ImGuiKey_Delete))
        {
            links.erase(links.begin() + hovered_link_id);
        }
    }
}

int GraphEditor::RegisterNewNode(const Node& node)
{
    int id;
    do
    {
        id = rand();
    } while (nodePool.find(id) != nodePool.end());

    nodePool[id] = node;
    nodePool[id].runtimeID = id;

    return id;
}

int GraphEditor::RegisterNewPin(const Pin& pin)
{
    int id;
    do
    {
        id = rand();
    } while (pinPool.find(id) != pinPool.end());

    pinPool[id] = pin;
    pinPool[id].runtimeID = id;

    return id;
}

Node& GraphEditor::GetNodeByID(int runtimeID)
{
    if (nodePool.find(runtimeID) == nodePool.end())
    {
        throw std::exception();
    }
    return nodePool[runtimeID];
}

Pin& GraphEditor::GetPinByID(int runtimeID)
{
    if (pinPool.find(runtimeID) == pinPool.end())
    {
        throw std::exception();
    }
    return pinPool[runtimeID];
}

void GraphEditor::SaveToFile(std::string filepath)
{
    
}

void GraphEditor::LoadFromFile(std::string filepath)
{

}

