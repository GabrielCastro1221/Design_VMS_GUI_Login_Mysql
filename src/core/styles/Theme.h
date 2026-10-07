#pragma once

#include <imgui.h>

class Theme
{
public:
    struct Colors
    {
        static constexpr ImVec4 Background = {0.11f, 0.12f, 0.14f, 1.00f};
        static constexpr ImVec4 Surface = {0.15f, 0.16f, 0.19f, 1.00f};
        static constexpr ImVec4 SurfaceAlt = {0.18f, 0.19f, 0.22f, 1.00f};
        static constexpr ImVec4 Input = {0.13f, 0.14f, 0.16f, 1.00f};

        static constexpr ImVec4 Border = {0.28f, 0.30f, 0.34f, 1.00f};
        static constexpr ImVec4 Separator = {0.28f, 0.30f, 0.34f, 0.60f};

        static constexpr ImVec4 Text = {0.90f, 0.91f, 0.93f, 1.00f};
        static constexpr ImVec4 TextMuted = {0.58f, 0.60f, 0.64f, 1.00f};
        static constexpr ImVec4 TextDisabled = {0.42f, 0.44f, 0.48f, 1.00f};

        static constexpr ImVec4 Accent = {0.25f, 0.47f, 0.72f, 1.00f};
        static constexpr ImVec4 AccentHover = {0.32f, 0.55f, 0.80f, 1.00f};
        static constexpr ImVec4 AccentActive = {0.20f, 0.40f, 0.62f, 1.00f};

        static constexpr ImVec4 Success = {0.30f, 0.65f, 0.45f, 1.00f};
        static constexpr ImVec4 Warning = {0.85f, 0.65f, 0.25f, 1.00f};
        static constexpr ImVec4 Error = {0.78f, 0.32f, 0.32f, 1.00f};
        static constexpr ImVec4 Info = {0.35f, 0.55f, 0.75f, 1.00f};
    };

    static void Apply();

    static void PushLogin();
    static void PopLogin();

    static int PushAccentButton();
    static void PopAccentButton();
};