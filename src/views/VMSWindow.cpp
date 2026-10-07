#include "VMSWindow.h"
#include "core/styles/Theme.h"
#include <imgui.h>
#include <algorithm>
#include <cfloat>
#include <ctime>
#include <string>
#include <utility>

namespace
{
    ImU32 toU32(const ImVec4 &c) { return ImGui::ColorConvertFloat4ToU32(c); }

    const char *subtitle(Page page)
    {
        switch (page)
        {
        case Page::Dashboard:
            return "Resumen general del sistema";
        case Page::LiveView:
            return "Video en vivo de tus cámaras";
        case Page::Playback:
            return "Busca y reproduce grabaciones";
        case Page::Events:
            return "Eventos y detecciones recientes";
        case Page::Cameras:
            return "Administra cámaras y transmisiones";
        case Page::Persons:
            return "Personas conocidas y reconocimiento facial";
        case Page::Alerts:
            return "Reglas de alerta y notificaciones";
        case Page::Users:
            return "Usuarios, roles y permisos";
        case Page::Audit:
            return "Registro de acciones del sistema";
        case Page::Settings:
            return "Configuración general";
        default:
            return "";
        }
    }

    void drawStatCard(ImDrawList *dl, ImVec2 p0, ImVec2 size, const char *label, const std::string &value,
                      const char *caption, const ImVec4 &accent)
    {
        const ImVec2 p1(p0.x + size.x, p0.y + size.y);
        dl->AddRectFilled(p0, p1, toU32(Theme::Colors::Surface), 12.0f);
        dl->AddRect(p0, p1, toU32(Theme::Colors::Border), 12.0f, 0, 1.0f);

        dl->AddRectFilled(ImVec2(p0.x, p0.y + 14.0f), ImVec2(p0.x + 3.0f, p1.y - 14.0f), toU32(accent), 2.0f);

        dl->AddText(ImVec2(p0.x + 20.0f, p0.y + 16.0f), toU32(Theme::Colors::TextMuted), label);
        dl->AddText(ImGui::GetFont(), 30.0f, ImVec2(p0.x + 20.0f, p0.y + 38.0f), toU32(Theme::Colors::Text), value.c_str());
        dl->AddText(ImVec2(p0.x + 20.0f, p1.y - 26.0f), toU32(Theme::Colors::TextDisabled), caption);
    }

    std::string numberOrDash(int value)
    {
        return value < 0 ? std::string("--") : std::to_string(value);
    }
}

VMSWindow::VMSWindow() = default;

void VMSWindow::setUser(const std::string &username, const std::string &role)
{
    sidebar_.setUser(username, role);
}

void VMSWindow::setPermissions(const std::unordered_set<std::string> &permissions)
{
    sidebar_.setPermissions(permissions);
}

void VMSWindow::setAllowAllPermissions(bool allow)
{
    sidebar_.setAllowAll(allow);
}

void VMSWindow::setOnLogout(std::function<void()> callback)
{
    sidebar_.setOnLogout(std::move(callback));
}

void VMSWindow::setPageRenderer(Page page, PageRenderer renderer)
{
    const size_t i = static_cast<size_t>(page);
    if (i < renderers_.size())
        renderers_[i] = std::move(renderer);
}

void VMSWindow::setPendingEvents(int count)
{
    pendingEvents_ = count;
    sidebar_.setBadge(Page::Events, std::max(0, count));
}

void VMSWindow::render()
{
    sidebar_.render();

    const ImVec2 display = ImGui::GetIO().DisplaySize;
    const float sw = sidebar_.width();
    ImGui::SetNextWindowPos(ImVec2(sw, 0.0f));
    ImGui::SetNextWindowSize(ImVec2(std::max(0.0f, display.x - sw), display.y));

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(32.0f, 26.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, Theme::Colors::Background);

    const ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                                   ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus;
    ImGui::Begin("##vms_content", nullptr, flags);
    ImGui::PopStyleColor();
    ImGui::PopStyleVar(3);

    renderHeader();
    renderBody();

    ImGui::End();
}

void VMSWindow::renderHeader()
{
    ImDrawList *dl = ImGui::GetWindowDrawList();
    ImFont *font = ImGui::GetFont();
    const ImVec2 p = ImGui::GetCursorScreenPos();
    const float availW = ImGui::GetContentRegionAvail().x;
    const Page page = sidebar_.current();

    dl->AddText(font, 28.0f, p, toU32(Theme::Colors::Text), Sidebar::title(page));
    dl->AddText(ImVec2(p.x, p.y + 36.0f), toU32(Theme::Colors::TextMuted), subtitle(page));

    const std::time_t now = std::time(nullptr);
    const std::tm *lt = std::localtime(&now);
    if (lt)
    {
        char timeText[16];
        char dateText[16];
        std::strftime(timeText, sizeof(timeText), "%H:%M", lt);
        std::strftime(dateText, sizeof(dateText), "%d/%m/%Y", lt);

        const ImVec2 tsz = font->CalcTextSizeA(24.0f, FLT_MAX, 0.0f, timeText);
        const ImVec2 dsz = ImGui::CalcTextSize(dateText);
        dl->AddText(font, 24.0f, ImVec2(p.x + availW - tsz.x, p.y), toU32(Theme::Colors::Text), timeText);
        dl->AddText(ImVec2(p.x + availW - dsz.x, p.y + 36.0f), toU32(Theme::Colors::TextMuted), dateText);
    }

    ImGui::Dummy(ImVec2(availW, 62.0f));

    const ImVec2 q = ImGui::GetCursorScreenPos();
    dl->AddLine(q, ImVec2(q.x + availW, q.y), toU32(Theme::Colors::Separator), 1.0f);
    ImGui::Dummy(ImVec2(availW, 22.0f));
}

void VMSWindow::renderBody()
{
    const Page page = sidebar_.current();
    const size_t i = static_cast<size_t>(page);

    if (i < renderers_.size() && renderers_[i])
        renderers_[i]();
    else if (page == Page::Dashboard)
        renderDashboard();
    else
        renderPlaceholder();
}

void VMSWindow::renderDashboard()
{
    ImDrawList *dl = ImGui::GetWindowDrawList();
    const float gap = 16.0f;
    const float availW = ImGui::GetContentRegionAvail().x;
    const int cols = std::max(1, std::min(4, static_cast<int>((availW + gap) / (210.0f + gap))));
    const float cardW = (availW - gap * static_cast<float>(cols - 1)) / static_cast<float>(cols);
    const float cardH = 104.0f;
    const ImVec2 origin = ImGui::GetCursorScreenPos();

    std::string online = "--";
    if (stats_.camerasOnline >= 0)
        online = std::to_string(stats_.camerasOnline) +
                 (stats_.camerasTotal >= 0 ? " / " + std::to_string(stats_.camerasTotal) : std::string());

    struct Card
    {
        const char *label;
        std::string value;
        const char *caption;
        ImVec4 accent;
    };
    const Card cards[] = {
        {"Cámaras en línea", online, "Conectadas / total", Theme::Colors::Success},
        {"Grabando", numberOrDash(stats_.recording), "Cámaras con grabación activa", Theme::Colors::Accent},
        {"Eventos hoy", numberOrDash(stats_.eventsToday), "Detecciones y alertas", Theme::Colors::Info},
        {"Eventos pendientes", numberOrDash(pendingEvents_), "Sin revisar", Theme::Colors::Warning},
    };

    for (int i = 0; i < 4; ++i)
    {
        const int col = i % cols;
        const int row = i / cols;
        drawStatCard(dl, ImVec2(origin.x + static_cast<float>(col) * (cardW + gap), origin.y + static_cast<float>(row) * (cardH + gap)),
                     ImVec2(cardW, cardH), cards[i].label, cards[i].value, cards[i].caption, cards[i].accent);
    }

    const int rows = (4 + cols - 1) / cols;
    ImGui::Dummy(ImVec2(availW, static_cast<float>(rows) * cardH + static_cast<float>(rows - 1) * gap + 24.0f));

    const ImVec2 p0 = ImGui::GetCursorScreenPos();
    const float panelH = std::max(160.0f, ImGui::GetContentRegionAvail().y - 8.0f);
    const ImVec2 p1(p0.x + availW, p0.y + panelH);
    dl->AddRectFilled(p0, p1, toU32(Theme::Colors::Surface), 12.0f);
    dl->AddRect(p0, p1, toU32(Theme::Colors::Border), 12.0f, 0, 1.0f);
    dl->AddText(ImGui::GetFont(), 19.0f, ImVec2(p0.x + 22.0f, p0.y + 18.0f), toU32(Theme::Colors::Text), "Actividad reciente");
    dl->AddText(ImVec2(p0.x + 22.0f, p0.y + 48.0f), toU32(Theme::Colors::TextMuted),
                "Aquí se mostrarán los últimos eventos del sistema.");
    ImGui::Dummy(ImVec2(availW, panelH));
}

void VMSWindow::renderPlaceholder()
{
    const char *message = "Esta pantalla aún no está implementada.";
    const Page page = sidebar_.current();

    const ImVec2 avail = ImGui::GetContentRegionAvail();
    const float w = ImGui::CalcTextSize(message).x;
    const ImVec2 start = ImGui::GetCursorPos();

    ImGui::SetCursorPos(ImVec2(start.x + (avail.x - w) * 0.5f, start.y + std::max(40.0f, avail.y * 0.30f)));
    ImGui::PushStyleColor(ImGuiCol_Text, Theme::Colors::TextMuted);
    ImGui::TextUnformatted(message);
    ImGui::PopStyleColor();

    const char *hint = Sidebar::title(page);
    const float hw = ImGui::CalcTextSize(hint).x;
    ImGui::SetCursorPosX(start.x + (avail.x - hw) * 0.5f);
    ImGui::PushStyleColor(ImGuiCol_Text, Theme::Colors::TextDisabled);
    ImGui::TextUnformatted(hint);
    ImGui::PopStyleColor();
}