#include "GraphEditor.h"

#include "../Library/imgui/imgui.h"
#include "../Library/imgui/imgui_impl_win32.h"
#include "../Library/imgui/imgui_impl_dx12.h"
#include "../Library/imgui/imnodes.h"

#include <random>

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
    for (int i = 0; i < edges.size(); ++i)
    {
        const std::pair<int, int> p = edges[i];
        // in this case, we just use the array index of the link
        // as the unique identifier
        ImNodes::Link(i, p.first, p.second);
    }

    ImNodes::EndNodeEditor();
    ImGui::End();

    //check links
    for (auto start_pin_id : usedPinID)
    {
        for (auto end_pin_id : usedPinID)
        {
            if (start_pin_id == end_pin_id) continue;

            // query if connection
            if (ImNodes::IsLinkCreated(&start_pin_id, &end_pin_id))
            {
                edges.push_back(std::make_pair(start_pin_id, end_pin_id));
            }
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

