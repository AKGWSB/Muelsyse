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

        // viewport
        {
            Json j_nodeDesc = GraphEditor::GetNodeDescriptorByID(runtimeID);

            auto& vparr = j_nodeDesc["viewPort"].array_items();
            ImVec4 vp = ImVec4(vparr[0].int_value(), vparr[1].int_value(), vparr[2].int_value(), vparr[3].int_value());

            ImGui::Text("ViewPort");
            ImGui::Text("%d %d", vp.x, vp.y);
            ImGui::Text("%d %d", vp.z, vp.w);
        }

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

        // button
        /*
        std::string buttonName = "select texture";
        std::string buttonID = buttonName + std::to_string(runtimeID); // identifier
        if (ImGui::Button(buttonName.c_str()))
        {
            ResourceViewer::Open(buttonID.c_str());
        }
        ResourceViewer::GetSelectResourceName(resourceName, buttonID);
        */

        ImNodes::EndNode();
    }

    if (type == NodeType::ScreenTargetNode)
    {
        ImNodes::PushColorStyle(ImNodesCol_TitleBar, IM_COL32(125, 75, 25, 255));
        ImNodes::PushColorStyle(ImNodesCol_TitleBarHovered, IM_COL32(150, 95, 55, 255));
        ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, IM_COL32(150, 95, 55, 255));

        ImNodes::BeginNode(runtimeID);
        ImNodes::BeginNodeTitleBar();
        ImGui::TextUnformatted("Screen");
        ImNodes::EndNodeTitleBar();

        RenderPins();

        ImNodes::PopColorStyle();
        ImNodes::PopColorStyle();
        ImNodes::PopColorStyle();

        ImNodes::EndNode();
    }
}

Json Node::to_json() const
{
    return Json::object{
        { "type", type },
        { "name", name },
        { "runtimeID", runtimeID },
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
        { "node1_ID", node1_ID },
        { "pin1_ID", pin1_ID },
        { "node2_ID", node2_ID },
        { "pin2_ID", pin2_ID },
        { "runtimeID", runtimeID }
    };
}

// ----------------------------------------------------------------------- //
