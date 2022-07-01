#include "GraphEditor.h"
#include "ResourceViewer.h"

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
    return Json::object {
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

        }
        ImNodes::EndNodeTitleBar();

        RenderPins();

        ImNodes::PopColorStyle();
        ImNodes::PopColorStyle();
        ImNodes::PopColorStyle();
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

        }
        ImNodes::EndNodeTitleBar();

        RenderPins();

        ImNodes::PopColorStyle();
        ImNodes::PopColorStyle();
        ImNodes::PopColorStyle();
    }

    if (type == NodeType::RenderTextureNode)
    {
        ImNodes::BeginNode(runtimeID);
        ImNodes::BeginNodeTitleBar();
        ImGui::TextUnformatted("Render Texture");
        ImGui::SameLine();
        if (ImGui::Button("X"))
        {

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
    }

    if (type == NodeType::Texture2DNode)
    {
        ImNodes::BeginNode(runtimeID);
        ImNodes::BeginNodeTitleBar();
        ImGui::TextUnformatted("Texture2D");
        ImGui::SameLine();
        if (ImGui::Button("X"))
        {

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
    }

    if (type == NodeType::ScreenTargetNode)
    {

    }

    ImNodes::EndNode();
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

std::map<int, Node> GraphEditor::nodePool;
std::map<int, Pin> GraphEditor::pinPool;
std::vector<Link> GraphEditor::links;

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

    std::string cf = "Current Pipeline file: " + filepath;
    ImGui::Text(cf.c_str());

    if (ImGui::Button("Load Pipeline"))
    {
        
    }
    ImGui::SameLine();
    if (ImGui::Button("Save Pipeline"))
    {
        SaveToFile();
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
    ImNodes::PushColorStyle(ImNodesCol_LinkSelected, IM_COL32(255, 255, 128, 255));
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

void GraphEditor::SaveToFile()
{
    // write
    std::ofstream out(filepath);
    out << to_json().dump();
    out.close();
}

void GraphEditor::LoadFromFile(std::string filepath)
{
    this->filepath = filepath;

    nodePool.clear();
    pinPool.clear();
    links.clear();

    std::ifstream in(filepath);
    std::ostringstream tmp;
    tmp << in.rdbuf();

    std::string json_data = tmp.str();
    std::string err_msg;

    // load
    Json json_obj = Json::parse(json_data, err_msg);

    // load pins
    auto& pins = json_obj["pinList"].array_items();
    for (auto& pin_obj : pins)
    {
        Pin pin;
        pin.name = pin_obj["name"].string_value();
        pin.runtimeID = pin_obj["runtimeID"].int_value();
        pin.type = PinType(pin_obj["type"].int_value());

        pinPool[pin.runtimeID] = pin;
    }

    // load nodes
    auto& nodes = json_obj["nodeList"].array_items();
    for (auto& node_obj : nodes)
    {
        Node node;
        node.name = node_obj["name"].string_value();
        node.runtimeID = node_obj["runtimeID"].int_value();
        node.type = NodeType(node_obj["type"].int_value());
        node.resourceName = node_obj["resourceName"].string_value();

        // input pin's id list
        auto& in_pins = node_obj["inputPins"].array_items();
        for (auto& in_pin_obj : in_pins)
        {
            node.inputPins.push_back(in_pin_obj.int_value());
        }

        // output pin's id list
        auto& out_pins = node_obj["outputPins"].array_items();
        for (auto& out_pin_obj : out_pins)
        {
            node.outputPins.push_back(out_pin_obj.int_value());
        }

        // set position
        auto& pos_obj = node_obj["position"].array_items();
        ImVec2 pos(pos_obj[0].int_value(), pos_obj[1].int_value());
        ImNodes::SetNodeEditorSpacePos(node.runtimeID, pos);

        nodePool[node.runtimeID] = node;
    }

    // load links
    auto& j_links = json_obj["links"].array_items();
    for (auto& link_obj : j_links)
    {
        Link link;
        link.runtimeID = link_obj["runtimeID"].int_value();
        link.dstNode = link_obj["dstNode"].int_value();
        link.dstPin = link_obj["dstPin"].int_value();
        link.srcNode = link_obj["srcNode"].int_value();
        link.srcPin = link_obj["srcPin"].int_value();

        links.push_back(link);
    }
}

void GraphEditor::Execute()
{

}

Json GraphEditor::to_json() const
{
    std::vector<Node> nodeList;
    for (auto& p : nodePool)
    {
        int id = p.first;
        Node& node = p.second;

        node.position = ImNodes::GetNodeEditorSpacePos(id);
        nodeList.push_back(node);
    }

    std::vector<Pin> pinList;
    for (auto& p : pinPool)
    {
        pinList.push_back(p.second);
    }

    return Json::object{
        { "links", links },
        { "nodeList", nodeList },
        { "pinList", pinList}
    };
}

