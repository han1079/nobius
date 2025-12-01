#include <algorithm>
#include <pch.h>

void DebugConsole::render(bool* p_open) {
    if (!ImGui::Begin("Variable Monitor", p_open, ImGuiWindowFlags_None)) {
        Debug::log("DebugConsole: ImGui::Begin returned false, skipping rendering.");
        ImGui::End();
        return;
    }
    
    if (m_variable_hooks.empty()) {
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "No variables hooked");
        ImGui::Text("Use hookVariable() to monitor variables");
        ImGui::End();
        return;
    }
    
    // Create a two-column table for variable name and value
    if (ImGui::BeginTable("VariableTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY,
                          ImVec2(0, 400))) {  // Fixed height, scrollable when more than 400px of content
        ImGui::TableSetupColumn("Variable", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_None);
        ImGui::TableHeadersRow();
 
        for (const auto& var : m_variable_hooks) {
            if (!m_variable_visibility[var.first]) {
                continue; // Skip hidden variables
            }
            ImGui::TableNextRow();
            
            // Variable name column
            ImGui::TableSetColumnIndex(0);
            ImGui::TextColored(ImVec4(0.8f, 0.9f, 1.0f, 1.0f), "%s", var.first.c_str());
            
            // Variable value column
            ImGui::TableSetColumnIndex(1);
            try {
                std::string value = getVariableValueAsString(var.first);
                ImGui::Text("%s", value.c_str());
            } catch (...) {
                ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "Error");
            }
        }
        
        ImGui::EndTable();
    }


    
    // Add some controls at the bottom
    ImGui::Separator();
    ImGui::Text("Total Variables: %zu", m_variable_hooks.size());
    ImGui::Text("Visible Variables: %zu", std::count_if(m_variable_visibility.begin(), m_variable_visibility.end(),
                                                  [](const auto& pair) { return pair.second; }));
    

    ImGui::Separator();
    if (ImGui::CollapsingHeader("Variable Visibility")) {
        // Create a scrollable child window for the checkboxes
        if (ImGui::BeginChild("VisibilityScrollArea", ImVec2(0, 200), true, ImGuiWindowFlags_AlwaysVerticalScrollbar)) {
            for (auto& v : m_variable_hooks) {
                ImGui::Checkbox(v.first.c_str(), &m_variable_visibility[v.first]);
            }
        }
        ImGui::EndChild();
    }

    ImGui::Separator();

    ImGui::Text("Messages:");
    if (ImGui::BeginChild("MessageScrollArea", ImVec2(0, 150), true)) {
        for (const auto& message : m_messages) {
            ImGui::Text("%s", message.second.c_str());
        }
    }
    ImGui::EndChild();
    

    
    
    ImGui::End();
}