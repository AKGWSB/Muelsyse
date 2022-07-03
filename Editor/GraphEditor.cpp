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

std::map<int, Json> GraphEditor::nodeDescriptorPool;
std::map<int, Node> GraphEditor::nodePool;
std::map<int, Pin> GraphEditor::pinPool;
std::vector<Link> GraphEditor::links;
std::vector<int> GraphEditor::deleteNodeList;

GraphEditor::GraphEditor()
{
    ImNodes::CreateContext();
}

GraphEditor::~GraphEditor()
{
    ImNodes::DestroyContext();
}

void GraphEditor::AddNode(NodeType type)
{
    if (type == NodeType::RenderPassNode)
    {
        // add node
        Node basePassNode;
        basePassNode.type = NodeType::RenderPassNode;
        basePassNode.name = "basePass";
        basePassNode.outputPins = { GraphEditor::RegisterNewPin(Pin(PinType::RenderTargetOutput, "RT_0")) };
        int id = GraphEditor::RegisterNewNode(basePassNode);

        // add descriptor
        nodeDescriptorPool[id] = Json::object {
            { "nodeID", id },
            { "viewPort", Json::array { 0, 0, 0, 0 } }
        };
    }

    if (type == NodeType::BlitPassNode)
    {
        // add node
        Node blitPassNode;
        blitPassNode.type = NodeType::BlitPassNode;
        blitPassNode.name = "blitPass";
        blitPassNode.inputPins = { GraphEditor::RegisterNewPin(Pin(PinType::RenderTargetOutput, "MainTex_0")) };
        blitPassNode.outputPins = { GraphEditor::RegisterNewPin(Pin(PinType::RenderTargetOutput, "RT_0")) };
        int id = GraphEditor::RegisterNewNode(blitPassNode);

        // add descriptor
        nodeDescriptorPool[id] = Json::object {
            { "nodeID", id },
            { "viewPort", Json::array { 0, 0, 0, 0 } },
            { "shaderName", "None" }
        };
    }

    if (type == NodeType::RenderTextureNode)
    {
        // add node
        Node rtNode;
        rtNode.type = NodeType::RenderTextureNode;
        rtNode.name = "RT_BasePass";
        rtNode.inputPins = { GraphEditor::RegisterNewPin(Pin(PinType::TextureWrite, "write")) };
        rtNode.outputPins = { GraphEditor::RegisterNewPin(Pin(PinType::TextureRead, "read")) };
        int id = GraphEditor::RegisterNewNode(rtNode);

        // add descriptor
        nodeDescriptorPool[id] = Json::object {
            { "nodeID", id },
            { "size", Json::array { 0, 0 } },
            { "format", DXGI_FORMAT_R8G8B8A8_UNORM }
        };
    }

    if (type == NodeType::Texture2DNode)
    {
        // add node
        Node texNode;
        texNode.type = NodeType::Texture2DNode;
        texNode.name = "Tex_0";
        texNode.outputPins = { GraphEditor::RegisterNewPin(Pin(PinType::TextureRead, "read")) };
        int id = GraphEditor::RegisterNewNode(texNode);

        // add descriptor
        nodeDescriptorPool[id] = Json::object {
            { "nodeID", id },
            { "textureName", "None" }
        };
    }

    if (type == NodeType::ScreenTargetNode)
    {
        // add node
        Node node;
        node.type = NodeType::ScreenTargetNode;
        node.name = "RT_Screen";
        node.inputPins = { GraphEditor::RegisterNewPin(Pin(PinType::TextureWrite, "write")) };
        int id = GraphEditor::RegisterNewNode(node);
    }
}

void GraphEditor::RemoveNode(int node_id)
{
    deleteNodeList.push_back(node_id);
}

void GraphEditor::RemoveNodeRightNow(int node_id)
{
    if (nodePool.find(node_id) == nodePool.end())
    {
        return;
    }

    Node& node = nodePool[node_id];

    // remove pins
    std::set<int> relate_pins;
    for (auto& pin_id : node.inputPins)
    {
        relate_pins.insert(pin_id);
        pinPool.erase(pin_id);
    }
    for (auto& pin_id : node.outputPins)
    {
        relate_pins.insert(pin_id);
        pinPool.erase(pin_id);
    }

    // record relate links
    std::set<int> link_delList;
    for (int i = 0; i < links.size(); i++)
    {
        auto& link = links[i];
        if (relate_pins.find(link.pin1_ID) != relate_pins.end() || relate_pins.find(link.pin2_ID) != relate_pins.end())
        {
            link_delList.insert(i);
        }
    }
    // remove links
    std::vector<Link> newLinkList;
    for (int i = 0; i < links.size(); i++)
    {
        if (link_delList.find(i) == link_delList.end())
        {
            newLinkList.push_back(links[i]);
        }
    }
    links = newLinkList;

    // remove node and it's descriptor
    nodePool.erase(node_id);
    nodeDescriptorPool.erase(node_id);
}

void GraphEditor::RenderUI()
{
    ImGui::SetNextWindowBgAlpha(0.35f);
    ImGui::Begin("Pipeline editor", NULL, ImGuiWindowFlags_MenuBar);

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Add Node"))
        {
            if (ImGui::MenuItem("Render Pass Node"))
            {
                GraphEditor::AddNode(NodeType::RenderPassNode);
            }

            if (ImGui::MenuItem("Blit Pass Node"))
            {
                GraphEditor::AddNode(NodeType::BlitPassNode);
            }

            if (ImGui::MenuItem("Render Texture Node"))
            {
                GraphEditor::AddNode(NodeType::RenderTextureNode);
            }

            if (ImGui::MenuItem("Texture2D Node"))
            {
                GraphEditor::AddNode(NodeType::Texture2DNode);
            }

            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

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

    // delete node, delay to this moment
    for (auto id : GraphEditor::deleteNodeList)
    {
        GraphEditor::RemoveNodeRightNow(id);
    }
    GraphEditor::deleteNodeList.clear();

    /**/
    // render links
    ImNodes::PushColorStyle(ImNodesCol_Link, IM_COL32(11, 109, 191, 255));
    ImNodes::PushColorStyle(ImNodesCol_LinkHovered, IM_COL32(255, 255, 128, 255));
    ImNodes::PushColorStyle(ImNodesCol_LinkSelected, IM_COL32(255, 255, 128, 255));
    for (int i = 0; i < links.size(); ++i)
    {
        Link& link = links[i];
        ImNodes::Link(i, link.pin1_ID, link.pin2_ID);
    }
    ImNodes::PopColorStyle();
    ImNodes::PopColorStyle();
    ImNodes::PopColorStyle();

    ImNodes::MiniMap(0.2f, ImNodesMiniMapLocation_BottomRight);
    ImNodes::EndNodeEditor();
    ImGui::End();

    /**/
    //create links
    int pin1_ID, pin2_ID;
    if (ImNodes::IsLinkCreated(&pin1_ID, &pin2_ID))
    {
        Link newLink;
        newLink.pin1_ID = pin1_ID;
        newLink.pin2_ID = pin2_ID;

        // find Nodes that this link append to
        for (auto& p : nodePool)
        {
            Node& node = p.second;
            for (auto& pin_id : node.inputPins)
            {
                if (pin_id == pin1_ID) newLink.node1_ID = node.runtimeID;
                if (pin_id == pin2_ID) newLink.node2_ID = node.runtimeID;
            }
            for (auto& pin_id : node.outputPins)
            {
                if (pin_id == pin1_ID) newLink.node1_ID = node.runtimeID;
                if (pin_id == pin2_ID) newLink.node2_ID = node.runtimeID;
            }
        }
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

    ImNodes::SetNodeEditorSpacePos(id, ImVec2(500, 250));

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

Json& GraphEditor::GetNodeDescriptorByID(int runtimeID)
{
    if (nodeDescriptorPool.find(runtimeID) == nodeDescriptorPool.end())
    {
        throw std::exception();
    }
    return nodeDescriptorPool[runtimeID];
}

void GraphEditor::SetNodeDescriptorByID(int runtimeID, const Json& json_obj)
{
    if (nodeDescriptorPool.find(runtimeID) == nodeDescriptorPool.end())
    {
        throw std::exception();
    }
    nodeDescriptorPool[runtimeID] = json_obj;
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
        link.node1_ID = link_obj["node1_ID"].int_value();
        link.pin1_ID = link_obj["pin1_ID"].int_value();
        link.node2_ID = link_obj["node2_ID"].int_value();
        link.pin2_ID = link_obj["pin2_ID"].int_value();

        links.push_back(link);
    }

    // load descriptor
    auto& j_descs = json_obj["descriptorList"].array_items();
    for (auto& desc_obj : j_descs)
    {
        int id = desc_obj["nodeID"].int_value();
        nodeDescriptorPool[id] = desc_obj;
    }

    // check if there has Screen
    bool hasScreen = false;
    for (auto it = nodePool.begin(); it != nodePool.end(); hasScreen = hasScreen || (it->second.type == NodeType::ScreenTargetNode), it++) {}
    if (!hasScreen)
    {
        GraphEditor::AddNode(NodeType::ScreenTargetNode);
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

    std::vector<Json> descriptorList;
    for (auto& p : nodeDescriptorPool)
    {
        descriptorList.push_back(p.second);
    }

    return Json::object{
        { "links", links },
        { "nodeList", nodeList },
        { "pinList", pinList},
        { "descriptorList" , descriptorList }
    };
}

