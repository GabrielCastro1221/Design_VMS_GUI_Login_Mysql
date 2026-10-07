#pragma once

#include <array>
#include <functional>
#include <string>
#include <unordered_set>
#include "components/Sidebar.h"

class VMSWindow
{
public:
    using PageRenderer = std::function<void()>;

    struct DashboardStats
    {
        int camerasOnline = -1;
        int camerasTotal = -1;
        int recording = -1;
        int eventsToday = -1;
    };

    VMSWindow();

    void setUser(const std::string &username, const std::string &role = "");
    void setPermissions(const std::unordered_set<std::string> &permissions);
    void setAllowAllPermissions(bool allow);
    void setOnLogout(std::function<void()> callback);
    void setPageRenderer(Page page, PageRenderer renderer);
    void setStats(const DashboardStats &stats) { stats_ = stats; }
    void setPendingEvents(int count);
    Page currentPage() const { return sidebar_.current(); }
    void navigateTo(Page page) { sidebar_.setCurrent(page); }

    void render();

private:
    void renderHeader();
    void renderBody();
    void renderDashboard();
    void renderPlaceholder();

    Sidebar sidebar_;
    std::array<PageRenderer, static_cast<size_t>(Page::Count)> renderers_;
    DashboardStats stats_;
    int pendingEvents_ = -1;
};