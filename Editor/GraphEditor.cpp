#include "GraphEditor.h"

#include "../Library/imgui/imgui.h"
#include "../Library/imgui/imgui_impl_win32.h"
#include "../Library/imgui/imgui_impl_dx12.h"
#include "../Library/imgui/imnodes.h"

GraphEditor::GraphEditor()
{
    ImNodes::CreateContext();
}

GraphEditor::~GraphEditor()
{
    ImNodes::DestroyContext();
}

void GraphEditor::RenderUI()
{
    const int hardcoded_node_id = 1;

    ImNodes::BeginNodeEditor();

    {
        ImNodes::BeginNode(hardcoded_node_id);

        const int input_attr_id = 1;
        ImNodes::BeginInputAttribute(input_attr_id);
        ImGui::Text("Intput pin");
        ImNodes::EndInputAttribute();

        const int output_attr_id = 2;
        ImNodes::BeginOutputAttribute(output_attr_id);
        ImGui::Text("output pin");
        ImNodes::EndOutputAttribute();

        ImNodes::EndNode();
    }

    {
        ImNodes::BeginNode(2);

        const int input_attr_id = 3;
        ImNodes::BeginInputAttribute(input_attr_id);
        ImGui::Text("Intput pin");
        ImNodes::EndInputAttribute();

        const int output_attr_id = 4;
        ImNodes::BeginOutputAttribute(output_attr_id);
        ImGui::Text("output pin");
        ImNodes::EndOutputAttribute();

        ImNodes::EndNode();
    }

    ImNodes::EndNodeEditor();
}