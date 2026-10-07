#include "Sidebar.h"
#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdio>
#include <string>
#include <utility>

namespace
{
    constexpr float kExpandedW = 248.0f;
    constexpr float kCollapsedW = 76.0f;
    constexpr float kPadX = 12.0f;
    constexpr float kItemH = 44.0f;
    constexpr float kHeaderH = 76.0f;

    const ImU32 kBg = IM_COL32(17, 20, 26, 255);
    const ImU32 kBorder = IM_COL32(255, 255, 255, 18);
    const ImU32 kAccent = IM_COL32(64, 120, 184, 255);
    const ImU32 kAccentSoft = IM_COL32(64, 120, 184, 52);
    const ImU32 kHover = IM_COL32(255, 255, 255, 14);
    const ImU32 kTextActive = IM_COL32(255, 255, 255, 255);
    const ImU32 kTextNormal = IM_COL32(186, 194, 208, 255);
    const ImU32 kTextMuted = IM_COL32(120, 130, 148, 255);
    const ImU32 kBadge = IM_COL32(214, 70, 70, 255);

    ImU32 withAlpha(ImU32 c, float a01)
    {
        const int a = static_cast<int>(((c >> 24) & 0xFF) * std::min(1.0f, std::max(0.0f, a01)));
        return (c & 0x00FFFFFFu) | (static_cast<ImU32>(a) << 24);
    }

    using IconFn = void (*)(ImDrawList *, ImVec2, float, ImU32);
    constexpr float kT = 1.7f;

    void iconDashboard(ImDrawList *dl, ImVec2 c, float s, ImU32 col)
    {
        const float a = s * 0.04f, b = s * 0.46f;
        dl->AddRect(ImVec2(c.x - b, c.y - b), ImVec2(c.x - a, c.y - a), col, 3.0f, 0, kT);
        dl->AddRect(ImVec2(c.x + a, c.y - b), ImVec2(c.x + b, c.y - a), col, 3.0f, 0, kT);
        dl->AddRect(ImVec2(c.x - b, c.y + a), ImVec2(c.x - a, c.y + b), col, 3.0f, 0, kT);
        dl->AddRect(ImVec2(c.x + a, c.y + a), ImVec2(c.x + b, c.y + b), col, 3.0f, 0, kT);
    }

    void iconLive(ImDrawList *dl, ImVec2 c, float s, ImU32 col)
    {
        dl->AddRect(ImVec2(c.x - s * 0.5f, c.y - s * 0.28f), ImVec2(c.x + s * 0.12f, c.y + s * 0.28f), col, 4.0f, 0, kT);
        dl->AddTriangle(ImVec2(c.x + s * 0.20f, c.y), ImVec2(c.x + s * 0.50f, c.y - s * 0.24f),
                        ImVec2(c.x + s * 0.50f, c.y + s * 0.24f), col, kT);
    }

    void iconPlayback(ImDrawList *dl, ImVec2 c, float s, ImU32 col)
    {
        dl->AddCircle(c, s * 0.46f, col, 28, kT);
        dl->AddTriangleFilled(ImVec2(c.x - s * 0.12f, c.y - s * 0.20f), ImVec2(c.x - s * 0.12f, c.y + s * 0.20f),
                              ImVec2(c.x + s * 0.22f, c.y), col);
    }

    void iconEvents(ImDrawList *dl, ImVec2 c, float s, ImU32 col)
    {
        dl->AddBezierCubic(ImVec2(c.x - s * 0.36f, c.y + s * 0.22f), ImVec2(c.x - s * 0.36f, c.y - s * 0.62f),
                           ImVec2(c.x + s * 0.36f, c.y - s * 0.62f), ImVec2(c.x + s * 0.36f, c.y + s * 0.22f), col, kT);
        dl->AddLine(ImVec2(c.x - s * 0.46f, c.y + s * 0.22f), ImVec2(c.x + s * 0.46f, c.y + s * 0.22f), col, kT);
        dl->AddCircleFilled(ImVec2(c.x, c.y + s * 0.40f), s * 0.07f, col, 12);
    }

    void iconCameras(ImDrawList *dl, ImVec2 c, float s, ImU32 col)
    {
        dl->AddCircle(c, s * 0.46f, col, 28, kT);
        dl->AddCircle(c, s * 0.20f, col, 20, kT);
        dl->AddCircleFilled(ImVec2(c.x + s * 0.30f, c.y - s * 0.30f), s * 0.05f, col, 8);
    }

    void iconPerson(ImDrawList *dl, ImVec2 c, float s, ImU32 col)
    {
        dl->AddCircle(ImVec2(c.x, c.y - s * 0.20f), s * 0.22f, col, 24, kT);
        dl->AddBezierCubic(ImVec2(c.x - s * 0.40f, c.y + s * 0.45f), ImVec2(c.x - s * 0.40f, c.y + s * 0.08f),
                           ImVec2(c.x + s * 0.40f, c.y + s * 0.08f), ImVec2(c.x + s * 0.40f, c.y + s * 0.45f), col, kT);
    }

    void iconAlerts(ImDrawList *dl, ImVec2 c, float s, ImU32 col)
    {
        dl->AddTriangle(ImVec2(c.x, c.y - s * 0.46f), ImVec2(c.x - s * 0.52f, c.y + s * 0.38f),
                        ImVec2(c.x + s * 0.52f, c.y + s * 0.38f), col, kT);
        dl->AddLine(ImVec2(c.x, c.y - s * 0.12f), ImVec2(c.x, c.y + s * 0.10f), col, kT);
        dl->AddCircleFilled(ImVec2(c.x, c.y + s * 0.24f), s * 0.05f, col, 8);
    }

    void iconUsers(ImDrawList *dl, ImVec2 c, float s, ImU32 col)
    {
        dl->AddCircle(ImVec2(c.x - s * 0.14f, c.y - s * 0.18f), s * 0.18f, col, 20, kT);
        dl->AddBezierCubic(ImVec2(c.x - s * 0.50f, c.y + s * 0.42f), ImVec2(c.x - s * 0.50f, c.y + s * 0.10f),
                           ImVec2(c.x + s * 0.22f, c.y + s * 0.10f), ImVec2(c.x + s * 0.22f, c.y + s * 0.42f), col, kT);
        dl->AddCircle(ImVec2(c.x + s * 0.26f, c.y - s * 0.10f), s * 0.15f, col, 20, kT);
        dl->AddBezierCubic(ImVec2(c.x + s * 0.34f, c.y + s * 0.18f), ImVec2(c.x + s * 0.54f, c.y + s * 0.20f),
                           ImVec2(c.x + s * 0.54f, c.y + s * 0.30f), ImVec2(c.x + s * 0.54f, c.y + s * 0.42f), col, kT);
    }

    void iconAudit(ImDrawList *dl, ImVec2 c, float s, ImU32 col)
    {
        dl->AddRect(ImVec2(c.x - s * 0.34f, c.y - s * 0.46f), ImVec2(c.x + s * 0.34f, c.y + s * 0.46f), col, 4.0f, 0, kT);
        dl->AddLine(ImVec2(c.x - s * 0.18f, c.y - s * 0.16f), ImVec2(c.x + s * 0.18f, c.y - s * 0.16f), col, kT);
        dl->AddLine(ImVec2(c.x - s * 0.18f, c.y + s * 0.04f), ImVec2(c.x + s * 0.18f, c.y + s * 0.04f), col, kT);
        dl->AddLine(ImVec2(c.x - s * 0.18f, c.y + s * 0.24f), ImVec2(c.x + s * 0.06f, c.y + s * 0.24f), col, kT);
    }

    void iconSettings(ImDrawList *dl, ImVec2 c, float s, ImU32 col)
    {
        dl->AddCircle(c, s * 0.18f, col, 20, kT);
        dl->AddCircle(c, s * 0.34f, col, 28, kT);
        for (int i = 0; i < 8; ++i)
        {
            const float a = static_cast<float>(i) * 0.7853982f;
            const float ca = std::cos(a), sa = std::sin(a);
            dl->AddLine(ImVec2(c.x + ca * s * 0.34f, c.y + sa * s * 0.34f),
                        ImVec2(c.x + ca * s * 0.48f, c.y + sa * s * 0.48f), col, kT + 0.6f);
        }
    }

    void iconChevronLeft(ImDrawList *dl, ImVec2 c, float s, ImU32 col)
    {
        dl->AddLine(ImVec2(c.x + s * 0.14f, c.y - s * 0.30f), ImVec2(c.x - s * 0.16f, c.y), col, kT);
        dl->AddLine(ImVec2(c.x - s * 0.16f, c.y), ImVec2(c.x + s * 0.14f, c.y + s * 0.30f), col, kT);
    }

    void iconChevronRight(ImDrawList *dl, ImVec2 c, float s, ImU32 col)
    {
        dl->AddLine(ImVec2(c.x - s * 0.14f, c.y - s * 0.30f), ImVec2(c.x + s * 0.16f, c.y), col, kT);
        dl->AddLine(ImVec2(c.x + s * 0.16f, c.y), ImVec2(c.x - s * 0.14f, c.y + s * 0.30f), col, kT);
    }

    void iconLogout(ImDrawList *dl, ImVec2 c, float s, ImU32 col)
    {
        dl->AddLine(ImVec2(c.x - s * 0.05f, c.y - s * 0.42f), ImVec2(c.x - s * 0.42f, c.y - s * 0.42f), col, kT);
        dl->AddLine(ImVec2(c.x - s * 0.42f, c.y - s * 0.42f), ImVec2(c.x - s * 0.42f, c.y + s * 0.42f), col, kT);
        dl->AddLine(ImVec2(c.x - s * 0.42f, c.y + s * 0.42f), ImVec2(c.x - s * 0.05f, c.y + s * 0.42f), col, kT);
        // Flecha
        dl->AddLine(ImVec2(c.x - s * 0.08f, c.y), ImVec2(c.x + s * 0.46f, c.y), col, kT);
        dl->AddLine(ImVec2(c.x + s * 0.24f, c.y - s * 0.20f), ImVec2(c.x + s * 0.46f, c.y), col, kT);
        dl->AddLine(ImVec2(c.x + s * 0.24f, c.y + s * 0.20f), ImVec2(c.x + s * 0.46f, c.y), col, kT);
    }

    void drawLogoMark(ImDrawList *dl, ImVec2 c, float r)
    {
        dl->AddRectFilled(ImVec2(c.x - r, c.y - r), ImVec2(c.x + r, c.y + r), kAccent, r * 0.38f);
        dl->AddCircle(c, r * 0.52f, kTextActive, 28, 2.0f);
        dl->AddCircleFilled(c, r * 0.24f, kTextActive, 20);
    }

    struct Item
    {
        Page page;
        const char *label;
        const char *permission;
        IconFn icon;
    };

    const Item kMonitoring[] = {
        {Page::Dashboard, "Panel", "", iconDashboard},
        {Page::LiveView, "En vivo", "live.view", iconLive},
        {Page::Playback, "Grabaciones", "playback.view", iconPlayback},
        {Page::Events, "Eventos", "events.view", iconEvents},
    };
    const Item kManagement[] = {
        {Page::Cameras, "Cámaras", "cameras.manage", iconCameras},
        {Page::Persons, "Personas", "settings.manage", iconPerson},
        {Page::Alerts, "Alertas", "settings.manage", iconAlerts},
    };
    const Item kAdministration[] = {
        {Page::Users, "Usuarios", "users.manage", iconUsers},
        {Page::Audit, "Auditoría", "audit.view", iconAudit},
        {Page::Settings, "Configuración", "settings.manage", iconSettings},
    };

    struct SectionRef
    {
        const char *title;
        const Item *items;
        int count;
    };

    template <size_t N>
    SectionRef makeSection(const char *title, const Item (&items)[N])
    {
        return SectionRef{title, items, static_cast<int>(N)};
    }

    const SectionRef kSections[] = {
        makeSection("MONITOREO", kMonitoring),
        makeSection("GESTIÓN", kManagement),
        makeSection("ADMINISTRACIÓN", kAdministration),
    };

    bool navRow(ImDrawList *dl, const char *id, IconFn icon, const char *label, bool active,
                int badge, float x, float y, float w, float t, float winLeft)
    {
        ImGui::SetCursorScreenPos(ImVec2(x, y));
        ImGui::InvisibleButton(id, ImVec2(w, kItemH));
        const bool hovered = ImGui::IsItemHovered();
        const bool clicked = ImGui::IsItemClicked();

        const ImVec2 p0(x, y);
        const ImVec2 p1(x + w, y + kItemH);

        if (active)
        {
            dl->AddRectFilled(p0, p1, kAccentSoft, 10.0f);
            dl->AddRectFilled(ImVec2(winLeft, y + 9.0f), ImVec2(winLeft + 3.0f, y + kItemH - 9.0f), kAccent, 2.0f);
        }
        else if (hovered)
        {
            dl->AddRectFilled(p0, p1, kHover, 10.0f);
        }

        const ImU32 color = active ? kTextActive : (hovered ? kTextActive : kTextNormal);
        const float cy = y + kItemH * 0.5f;
        icon(dl, ImVec2(x + 26.0f, cy), 20.0f, active ? kAccent : color);

        if (t > 0.02f)
        {
            dl->PushClipRect(ImVec2(x + 50.0f, y), ImVec2(p1.x - (badge > 0 ? 38.0f : 8.0f), p1.y), true);
            dl->AddText(ImVec2(x + 52.0f, cy - ImGui::GetFontSize() * 0.5f), withAlpha(color, t), label);
            dl->PopClipRect();
        }

        if (badge > 0)
        {
            const std::string text = badge > 99 ? "99+" : std::to_string(badge);
            if (t > 0.6f)
            {
                const ImVec2 ts = ImGui::CalcTextSize(text.c_str());
                const float bw = std::max(22.0f, ts.x + 12.0f);
                const ImVec2 b0(p1.x - 10.0f - bw, cy - 10.0f);
                const ImVec2 b1(p1.x - 10.0f, cy + 10.0f);
                dl->AddRectFilled(b0, b1, withAlpha(kBadge, t), 10.0f);
                dl->AddText(ImVec2(b0.x + (bw - ts.x) * 0.5f, cy - ts.y * 0.5f), withAlpha(kTextActive, t), text.c_str());
            }
            else
            {
                dl->AddCircleFilled(ImVec2(x + 36.0f, cy - 10.0f), 5.0f, kBadge, 12);
            }
        }

        if (hovered)
        {
            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
            if (t < 0.5f)
                ImGui::SetTooltip("%s", label);
        }
        return clicked;
    }
}

Sidebar::Sidebar() = default;

const char *Sidebar::title(Page page)
{
    switch (page)
    {
    case Page::Dashboard:
        return "Panel";
    case Page::LiveView:
        return "En vivo";
    case Page::Playback:
        return "Grabaciones";
    case Page::Events:
        return "Eventos";
    case Page::Cameras:
        return "Cámaras";
    case Page::Persons:
        return "Personas";
    case Page::Alerts:
        return "Alertas";
    case Page::Users:
        return "Usuarios";
    case Page::Audit:
        return "Auditoría";
    case Page::Settings:
        return "Configuración";
    default:
        return "";
    }
}

void Sidebar::setUser(const std::string &username, const std::string &role)
{
    username_ = username;
    role_ = role;
}

void Sidebar::setPermissions(const std::unordered_set<std::string> &permissions)
{
    permissions_ = permissions;
    allowAll_ = false;
    ensureCurrentVisible();
}

void Sidebar::setBadge(Page page, int count)
{
    const int i = static_cast<int>(page);
    if (i >= 0 && i < static_cast<int>(Page::Count))
        badges_[i] = std::max(0, count);
}

void Sidebar::setCurrent(Page page)
{
    if (!isVisible(page))
        return;
    if (page != current_)
    {
        current_ = page;
        if (onNavigate_)
            onNavigate_(page);
    }
}

bool Sidebar::isVisible(Page page) const
{
    for (const SectionRef &section : kSections)
    {
        for (int i = 0; i < section.count; ++i)
        {
            const Item &item = section.items[i];
            if (item.page != page)
                continue;
            if (item.permission[0] == '\0' || allowAll_)
                return true;
            return permissions_.count(item.permission) > 0;
        }
    }
    return false;
}

void Sidebar::ensureCurrentVisible()
{
    if (isVisible(current_))
        return;
    for (const SectionRef &section : kSections)
        for (int i = 0; i < section.count; ++i)
            if (isVisible(section.items[i].page))
            {
                current_ = section.items[i].page;
                return;
            }
}

void Sidebar::render()
{
    ImGuiIO &io = ImGui::GetIO();

    const float target = collapsed_ ? kCollapsedW : kExpandedW;
    width_ += (target - width_) * std::min(1.0f, io.DeltaTime * 14.0f);
    if (std::fabs(target - width_) < 0.5f)
        width_ = target;
    const float t = std::min(1.0f, std::max(0.0f, (width_ - kCollapsedW) / (kExpandedW - kCollapsedW)));

    ensureCurrentVisible();

    ImGuiViewport *vp = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(vp->Pos);
    ImGui::SetNextWindowSize(ImVec2(width_, vp->Size.y));

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImGui::ColorConvertU32ToFloat4(kBg));

    const ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                                   ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus |
                                   ImGuiWindowFlags_NoScrollWithMouse;
    ImGui::Begin("##vms_sidebar", nullptr, flags);
    ImGui::PopStyleColor();
    ImGui::PopStyleVar(3);

    ImDrawList *dl = ImGui::GetWindowDrawList();
    const ImVec2 origin = ImGui::GetWindowPos();
    const float H = ImGui::GetWindowHeight();
    const float x = origin.x + kPadX;
    const float rowW = width_ - 2.0f * kPadX;

    dl->AddLine(ImVec2(origin.x + width_ - 0.5f, origin.y), ImVec2(origin.x + width_ - 0.5f, origin.y + H), kBorder, 1.0f);

    {
        const float cy = origin.y + kHeaderH * 0.5f;
        drawLogoMark(dl, ImVec2(x + 26.0f, cy), 17.0f);
        if (t > 0.02f)
        {
            ImFont *font = ImGui::GetFont();
            dl->PushClipRect(ImVec2(x + 52.0f, origin.y), ImVec2(origin.x + width_ - 8.0f, origin.y + kHeaderH), true);
            dl->AddText(font, 21.0f, ImVec2(x + 54.0f, cy - 15.0f), withAlpha(kTextActive, t), "CYCLOPS");
            dl->AddText(ImVec2(x + 54.0f, cy + 8.0f), withAlpha(kTextMuted, t), "Video Management System");
            dl->PopClipRect();
        }
    }

    float y = origin.y + kHeaderH;
    int uid = 0;
    for (const SectionRef &section : kSections)
    {
        bool any = false;
        for (int i = 0; i < section.count; ++i)
            if (isVisible(section.items[i].page))
                any = true;
        if (!any)
            continue;

        if (t > 0.02f)
            dl->AddText(ImVec2(x + 10.0f, y + 8.0f), withAlpha(kTextMuted, t), section.title);
        if (t < 0.98f)
            dl->AddLine(ImVec2(x + 8.0f, y + 14.0f), ImVec2(x + rowW - 8.0f, y + 14.0f), withAlpha(kBorder, 1.0f - t), 1.0f);
        y += 30.0f;

        for (int i = 0; i < section.count; ++i)
        {
            const Item &item = section.items[i];
            if (!isVisible(item.page))
                continue;

            char id[32];
            std::snprintf(id, sizeof(id), "##nav%d", uid++);
            if (navRow(dl, id, item.icon, item.label, item.page == current_,
                       badges_[static_cast<int>(item.page)], x, y, rowW, t, origin.x))
            {
                setCurrent(item.page);
            }
            y += kItemH + 2.0f;
        }
        y += 6.0f;
    }

    {
        const float logoutY = origin.y + H - 12.0f - kItemH;
        const float userY = logoutY - 8.0f - 56.0f;
        const float collapseY = userY - 6.0f - kItemH;

        dl->AddLine(ImVec2(x + 8.0f, collapseY - 8.0f), ImVec2(x + rowW - 8.0f, collapseY - 8.0f), kBorder, 1.0f);

        if (navRow(dl, "##collapse", collapsed_ ? iconChevronRight : iconChevronLeft,
                   collapsed_ ? "Expandir menú" : "Contraer menú", false, 0, x, collapseY, rowW, t, origin.x))
        {
            collapsed_ = !collapsed_;
        }

        {
            const ImVec2 u0(x, userY);
            const ImVec2 u1(x + rowW, userY + 56.0f);
            dl->AddRectFilled(u0, u1, IM_COL32(255, 255, 255, 10), 12.0f);

            const ImVec2 ac(x + 26.0f, userY + 28.0f);
            dl->AddCircleFilled(ac, 17.0f, kAccent, 28);

            char initial[2] = {'?', '\0'};
            if (!username_.empty() && static_cast<unsigned char>(username_[0]) < 128)
                initial[0] = static_cast<char>(std::toupper(static_cast<unsigned char>(username_[0])));
            const ImVec2 its = ImGui::CalcTextSize(initial);
            dl->AddText(ImVec2(ac.x - its.x * 0.5f, ac.y - its.y * 0.5f), kTextActive, initial);

            if (t > 0.02f)
            {
                dl->PushClipRect(ImVec2(x + 52.0f, userY), ImVec2(u1.x - 6.0f, u1.y), true);
                const float nameY = role_.empty() ? ac.y - ImGui::GetFontSize() * 0.5f : userY + 11.0f;
                dl->AddText(ImVec2(x + 54.0f, nameY), withAlpha(kTextActive, t),
                            username_.empty() ? "Usuario" : username_.c_str());
                if (!role_.empty())
                    dl->AddText(ImVec2(x + 54.0f, userY + 30.0f), withAlpha(kTextMuted, t), role_.c_str());
                dl->PopClipRect();
            }
        }

        if (navRow(dl, "##logout", iconLogout, "Cerrar sesión", false, 0, x, logoutY, rowW, t, origin.x))
        {
            if (onLogout_)
                onLogout_();
        }
    }

    ImGui::End();
}