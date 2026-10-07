#include "LoginWindow.h"
#include "core/media/VideoPlayer.h"
#include "core/styles/Theme.h"
#include <imgui.h>
#include <cfloat>
#include <cstdint>
#include <cstring>
#include <utility>

namespace
{
    ImU32 toU32(const ImVec4 &c) { return ImGui::ColorConvertFloat4ToU32(c); }

    ImVec4 withAlpha(ImVec4 c, float a)
    {
        c.w = a;
        return c;
    }

    void bigCenteredText(const char *text, float size, const ImVec4 &color)
    {
        ImFont *font = ImGui::GetFont();
        const ImVec2 ts = font->CalcTextSizeA(size, FLT_MAX, 0.0f, text);
        const ImVec2 p = ImGui::GetCursorScreenPos();
        const float x = p.x + (ImGui::GetContentRegionAvail().x - ts.x) * 0.5f;
        ImGui::GetWindowDrawList()->AddText(font, size, ImVec2(x, p.y), toU32(color), text);
        ImGui::Dummy(ImVec2(0.0f, ts.y));
    }

    void centeredText(const char *text, const ImVec4 &color)
    {
        const float w = ImGui::CalcTextSize(text).x;
        ImGui::SetCursorPosX((ImGui::GetWindowWidth() - w) * 0.5f);
        ImGui::PushStyleColor(ImGuiCol_Text, color);
        ImGui::TextUnformatted(text);
        ImGui::PopStyleColor();
    }
}

LoginWindow::LoginWindow(LoginCallback onLogin) : onLogin_(std::move(onLogin)) {}

void LoginWindow::drawGlass(const ImVec2 &p0, const ImVec2 &p1) const
{
    ImDrawList *dl = ImGui::GetBackgroundDrawList();
    const float rounding = 16.0f;

    for (int i = 1; i <= 8; ++i)
    {
        const float e = static_cast<float>(i) * 3.0f;
        dl->AddRectFilled(ImVec2(p0.x - e, p0.y - e + 10.0f), ImVec2(p1.x + e, p1.y + e + 10.0f), IM_COL32(0, 0, 0, 18 - i * 2), rounding + e);
    }

    if (background_ && background_->isReady())
    {
        const ImVec2 disp = ImGui::GetIO().DisplaySize;
        const VideoPlayer::UVRect uv = background_->getCoverUV(disp.x, disp.y);
        const float du = uv.u1 - uv.u0;
        const float dv = uv.v1 - uv.v0;
        const ImVec2 a(uv.u0 + p0.x / disp.x * du, uv.v0 + p0.y / disp.y * dv);
        const ImVec2 b(uv.u0 + p1.x / disp.x * du, uv.v0 + p1.y / disp.y * dv);

        dl->AddImageRounded((ImTextureID)(std::intptr_t)background_->getBlurTexture(), p0, p1, a, b, IM_COL32_WHITE, rounding);
        dl->AddRectFilled(p0, p1, IM_COL32(8, 18, 36, 135), rounding);
    }
    else
    {
        dl->AddRectFilled(p0, p1, IM_COL32(20, 30, 50, 235), rounding);
    }

    dl->AddRect(p0, p1, IM_COL32(255, 255, 255, 50), rounding, 0, 1.0f);
}

void LoginWindow::render()
{
    ImGuiViewport *viewport = ImGui::GetMainViewport();
    const ImVec2 center = viewport->GetCenter();

    ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(420.0f, 0.0f), ImGuiCond_Always);

    const ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar |
                                   ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground;

    const ImVec4 text = ImVec4(0.95f, 0.96f, 0.98f, 1.0f);
    const ImVec4 muted = ImVec4(0.68f, 0.73f, 0.82f, 1.0f);
    const ImVec4 hint = ImVec4(0.55f, 0.61f, 0.72f, 1.0f);
    const ImVec4 brand = ImVec4(0.55f, 0.76f, 1.0f, 1.0f);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 16.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(36.0f, 32.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 9.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(14.0f, 11.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10.0f, 12.0f));

    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 1.0f, 1.0f, 0.20f));
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(1.0f, 1.0f, 1.0f, 0.07f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(1.0f, 1.0f, 1.0f, 0.12f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(1.0f, 1.0f, 1.0f, 0.16f));
    ImGui::PushStyleColor(ImGuiCol_Button, Theme::Colors::Accent);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Theme::Colors::AccentHover);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, Theme::Colors::AccentActive);
    ImGui::PushStyleColor(ImGuiCol_Text, text);
    ImGui::PushStyleColor(ImGuiCol_TextDisabled, hint);
    ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4(1.0f, 1.0f, 1.0f, 0.14f));
    ImGui::PushStyleColor(ImGuiCol_TextSelectedBg, withAlpha(Theme::Colors::Accent, 0.55f));

    ImGui::Begin("##CyclopsLogin", nullptr, flags);

    {
        const ImVec2 wp = ImGui::GetWindowPos();
        const ImVec2 ws = ImGui::GetWindowSize();
        drawGlass(wp, ImVec2(wp.x + ws.x, wp.y + ws.y));
    }

    bigCenteredText("CYCLOPS", 30.0f, brand);
    centeredText("Video Management System", muted);

    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    ImGui::Spacing();

    ImGui::PushStyleColor(ImGuiCol_Text, muted);
    ImGui::TextUnformatted("USUARIO");
    ImGui::PopStyleColor();

    if (focusUser_)
    {
        ImGui::SetKeyboardFocusHere();
        focusUser_ = false;
    }
    ImGui::SetNextItemWidth(-1.0f);
    ImGui::InputTextWithHint("##user", "nombre de usuario", usernameBuf_, sizeof(usernameBuf_));

    ImGui::Spacing();

    ImGui::PushStyleColor(ImGuiCol_Text, muted);
    ImGui::TextUnformatted("CONTRASEÑA");
    ImGui::PopStyleColor();

    ImGui::SetNextItemWidth(-1.0f);
    const bool enterPressed = ImGui::InputTextWithHint(
        "##password",
        "contraseña",
        passwordBuf_, sizeof(passwordBuf_),
        ImGuiInputTextFlags_Password | ImGuiInputTextFlags_EnterReturnsTrue);

    ImGui::Spacing();
    ImGui::Spacing();

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 13.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
    const bool clicked = ImGui::Button("ENTRAR", ImVec2(-1.0f, 0.0f));
    ImGui::PopStyleVar(2);

    if (enterPressed || clicked)
        submit();

    if (!error_.empty())
    {
        ImGui::Spacing();
        centeredText(error_.c_str(), ImVec4(1.0f, 0.55f, 0.55f, 1.0f));
    }

    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    centeredText("Acceso seguro  ·  Cyclops VMS", hint);

    ImGui::End();
    ImGui::PopStyleColor(11);
    ImGui::PopStyleVar(7);
}

void LoginWindow::submit()
{
    const std::string user = usernameBuf_;
    const std::string password = passwordBuf_;

    if (user.empty() || password.empty())
    {
        error_ = "Ingresa usuario y contraseña.";
        return;
    }

    if (onLogin_ && onLogin_(user, password))
    {
        loggedIn_ = true;
        loggedUser_ = user;
        error_.clear();
    }
    else
    {
        error_ = "Usuario o contraseña incorrectos.";
    }

    std::memset(passwordBuf_, 0, sizeof(passwordBuf_));
}

void LoginWindow::logout()
{
    loggedIn_ = false;
    loggedUser_.clear();
    error_.clear();
    std::memset(usernameBuf_, 0, sizeof(usernameBuf_));
    std::memset(passwordBuf_, 0, sizeof(passwordBuf_));
    focusUser_ = true;
}