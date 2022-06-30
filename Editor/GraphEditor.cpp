#include "GraphEditor.h"

#include "../Library/imgui/imgui.h"
#include "../Library/imgui/imgui_impl_win32.h"
#include "../Library/imgui/imgui_impl_dx12.h"
#include "../Library/imgui/imnodes.h"

#include <random>

std::set<int> GraphEditor::usedNodeID;
std::set<int> GraphEditor::usedPinID;

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
    renderPassNodes = { RegisterNewNode<RenderPassNode>("BasePass") };
    /**/
    blitPassNodes = { 
        RegisterNewNode<BlitPassNode>("lightpass"), 
        RegisterNewNode<BlitPassNode>("finalPass") 
    };
    textureNodes = { RegisterNewNode<TextureNode>("Tex_0") };
    renderTextureNodes = { RegisterNewNode<RenderTextureNode>("RT_0") };
}

void GraphEditor::RenderUI()
{
    ImGui::Begin("Pipeline editor");
    ImNodes::BeginNodeEditor();
    
    // render nodes
    for(auto& node : renderPassNodes)
    {
        node.Render();
    }
    for (auto& node : blitPassNodes)
    {
        node.Render();
    }
    for (auto& node : textureNodes)
    {
        node.Render();
    }
    for (auto& node : renderTextureNodes)
    {
        node.Render();
    }

    // render links
    ImNodes::PushColorStyle(ImNodesCol_Link, IM_COL32(11, 109, 191, 255));
    ImNodes::PushColorStyle(ImNodesCol_LinkHovered, IM_COL32(255, 255, 128, 255));
    ImNodes::PushColorStyle(ImNodesCol_LinkSelected, IM_COL32(81, 148, 204, 255));
    for (int i = 0; i < edges.size(); ++i)
    {
        const std::pair<int, int> p = edges[i];

        int start_pin_id = p.first;
        int end_pin_id = p.second;
        int link_id = i;

        ImNodes::Link(link_id, start_pin_id, end_pin_id);
    }
    ImNodes::PopColorStyle();
    ImNodes::PopColorStyle();
    ImNodes::PopColorStyle();

    ImNodes::EndNodeEditor();
    ImGui::End();

    //create links
    int start_pin_id, end_pin_id;
    if (ImNodes::IsLinkCreated(&start_pin_id, &end_pin_id))
    {
        edges.push_back(std::make_pair(start_pin_id, end_pin_id));
    }

    // delete links
    int hovered_link_id;
    if (ImNodes::IsLinkHovered(&hovered_link_id))
    {
        ImGui::SetTooltip("Press \"Delete\" To Remove this Link");
        
        if (ImGui::IsKeyPressed(ImGuiKey_::ImGuiKey_Delete))
        {
            edges.erase(edges.begin() + hovered_link_id);
        }
    }
}


template <typename T_NODE>
T_NODE GraphEditor::RegisterNewNode(std::string nodeName)
{
    // node id
    int id;
    do
    {
        id = rand();
    } while (usedNodeID.find(id) != usedNodeID.end());
    usedNodeID.insert(id);

    T_NODE newNode;
    newNode.id = id;
    newNode.name = nodeName;

    // register pin id for all pins
    for (auto& pin : newNode.inputPins)
    {
        pin = RegisterNewPin(pin.name);
    }
    for (auto& pin : newNode.outputPins)
    {
        pin = RegisterNewPin(pin.name);
    }

    return newNode;
}

Pin GraphEditor::RegisterNewPin(std::string pinName)
{
    int id;
    do
    {
        id = rand();
    } while (usedPinID.find(id) != usedPinID.end());
    usedPinID.insert(id);

    Pin newPin;
    newPin.id = id;
    newPin.name = pinName;

    return newPin;
}

