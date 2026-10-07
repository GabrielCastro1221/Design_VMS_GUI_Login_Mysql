#include "Theme.h"

void Theme::Apply()
{
    ImGuiStyle &s = ImGui::GetStyle();

    s.WindowRounding = 6.0f;
    s.ChildRounding = 4.0f;
    s.FrameRounding = 4.0f;
    s.PopupRounding = 4.0f;
    s.ScrollbarRounding = 4.0f;
    s.GrabRounding = 3.0f;
    s.TabRounding = 4.0f;

    s.WindowBorderSize = 1.0f;
    s.FrameBorderSize = 0.0f;
    s.PopupBorderSize = 1.0f;

    s.WindowPadding = ImVec2(14.0f, 12.0f);
    s.FramePadding = ImVec2(10.0f, 6.0f);
    s.ItemSpacing = ImVec2(8.0f, 8.0f);
    s.ItemInnerSpacing = ImVec2(6.0f, 4.0f);
    s.IndentSpacing = 18.0f;
    s.ScrollbarSize = 12.0f;
    s.GrabMinSize = 10.0f;

    s.WindowTitleAlign = ImVec2(0.0f, 0.5f);
    s.ButtonTextAlign = ImVec2(0.5f, 0.5f);

    ImVec4 *c = s.Colors;

    c[ImGuiCol_Text] = Colors::Text;
    c[ImGuiCol_TextDisabled] = Colors::TextDisabled;
    c[ImGuiCol_WindowBg] = Colors::Background;
    c[ImGuiCol_ChildBg] = Colors::Surface;
    c[ImGuiCol_PopupBg] = Colors::SurfaceAlt;
    c[ImGuiCol_Border] = Colors::Border;
    c[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    c[ImGuiCol_FrameBg] = Colors::Input;
    c[ImGuiCol_FrameBgHovered] = ImVec4(0.18f, 0.19f, 0.22f, 1.0f);
    c[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.22f, 0.26f, 1.0f);
    c[ImGuiCol_TitleBg] = Colors::Surface;
    c[ImGuiCol_TitleBgActive] = Colors::SurfaceAlt;
    c[ImGuiCol_TitleBgCollapsed] = Colors::Surface;
    c[ImGuiCol_MenuBarBg] = Colors::Surface;
    c[ImGuiCol_ScrollbarBg] = Colors::Background;
    c[ImGuiCol_ScrollbarGrab] = Colors::Border;
    c[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.38f, 0.40f, 0.45f, 1.0f);
    c[ImGuiCol_ScrollbarGrabActive] = Colors::Accent;
    c[ImGuiCol_CheckMark] = Colors::Accent;
    c[ImGuiCol_SliderGrab] = Colors::Accent;
    c[ImGuiCol_SliderGrabActive] = Colors::AccentHover;
    c[ImGuiCol_Button] = Colors::Accent;
    c[ImGuiCol_ButtonHovered] = Colors::AccentHover;
    c[ImGuiCol_ButtonActive] = Colors::AccentActive;
    c[ImGuiCol_Header] = ImVec4(0.25f, 0.47f, 0.72f, 0.35f);
    c[ImGuiCol_HeaderHovered] = ImVec4(0.25f, 0.47f, 0.72f, 0.55f);
    c[ImGuiCol_HeaderActive] = ImVec4(0.25f, 0.47f, 0.72f, 0.75f);
    c[ImGuiCol_Separator] = Colors::Separator;
    c[ImGuiCol_SeparatorHovered] = Colors::Accent;
    c[ImGuiCol_SeparatorActive] = Colors::AccentHover;
    c[ImGuiCol_ResizeGrip] = ImVec4(0.28f, 0.30f, 0.34f, 0.40f);
    c[ImGuiCol_ResizeGripHovered] = Colors::Accent;
    c[ImGuiCol_ResizeGripActive] = Colors::AccentHover;
    c[ImGuiCol_Tab] = Colors::Surface;
    c[ImGuiCol_TabHovered] = Colors::AccentHover;
    c[ImGuiCol_TabActive] = Colors::Accent;
    c[ImGuiCol_TabUnfocused] = Colors::Surface;
    c[ImGuiCol_TabUnfocusedActive] = Colors::SurfaceAlt;
    c[ImGuiCol_PlotLines] = Colors::Accent;
    c[ImGuiCol_PlotLinesHovered] = Colors::AccentHover;
    c[ImGuiCol_PlotHistogram] = Colors::Accent;
    c[ImGuiCol_PlotHistogramHovered] = Colors::AccentHover;
    c[ImGuiCol_TableHeaderBg] = Colors::SurfaceAlt;
    c[ImGuiCol_TableBorderStrong] = Colors::Border;
    c[ImGuiCol_TableBorderLight] = Colors::Separator;
    c[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    c[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f, 1.0f, 1.0f, 0.03f);
    c[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 0.47f, 0.72f, 0.35f);
    c[ImGuiCol_DragDropTarget] = Colors::Accent;
    c[ImGuiCol_NavHighlight] = Colors::Accent;
    c[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.70f);
    c[ImGuiCol_NavWindowingDimBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.50f);
    c[ImGuiCol_ModalWindowDimBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.55f);
}

void Theme::PushLogin()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(32.0f, 28.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12.0f, 9.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10.0f, 11.0f));

    ImGui::PushStyleColor(ImGuiCol_WindowBg, Colors::Surface);
    ImGui::PushStyleColor(ImGuiCol_Border, Colors::Border);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, Colors::Input);
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.17f, 0.18f, 0.21f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.19f, 0.20f, 0.24f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Button, Colors::Accent);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Colors::AccentHover);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, Colors::AccentActive);
    ImGui::PushStyleColor(ImGuiCol_Separator, Colors::Separator);
    ImGui::PushStyleColor(ImGuiCol_Text, Colors::Text);
}

void Theme::PopLogin()
{
    ImGui::PopStyleColor(10);
    ImGui::PopStyleVar(5);
}

int Theme::PushAccentButton()
{
    ImGui::PushStyleColor(ImGuiCol_Button, Colors::Accent);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Colors::AccentHover);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, Colors::AccentActive);
    return 3;
}

void Theme::PopAccentButton()
{
    ImGui::PopStyleColor(3);
}