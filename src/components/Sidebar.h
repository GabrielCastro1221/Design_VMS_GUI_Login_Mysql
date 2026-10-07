#pragma once

#include <functional>
#include <string>
#include <unordered_set>
#include <utility>
#include <imgui.h>

enum class Page
{
    Dashboard,
    LiveView,
    Playback,
    Events,
    Cameras,
    Persons,
    Alerts,
    Users,
    Audit,
    Settings,
    Count
};

class Sidebar
{
public:
    Sidebar();

    void setUser(const std::string &username, const std::string &role = "");
    void setPermissions(const std::unordered_set<std::string> &permissions);
    void setAllowAll(bool allow) { allowAll_ = allow; }
    void setBadge(Page page, int count);
    void setOnNavigate(std::function<void(Page)> callback) { onNavigate_ = std::move(callback); }
    void setOnLogout(std::function<void()> callback) { onLogout_ = std::move(callback); }

    Page current() const { return current_; }
    void setCurrent(Page page);

    bool isCollapsed() const { return collapsed_; }
    void setCollapsed(bool collapsed) { collapsed_ = collapsed; }

    float width() const { return width_; }

    static const char *title(Page page);

    void render();

private:
    bool isVisible(Page page) const;
    void ensureCurrentVisible();

    Page current_ = Page::Dashboard;
    bool collapsed_ = false;
    float width_ = 248.0f;

    std::string username_;
    std::string role_;
    std::unordered_set<std::string> permissions_;
    bool allowAll_ = false;
    int badges_[static_cast<int>(Page::Count)] = {};

    std::function<void(Page)> onNavigate_;
    std::function<void()> onLogout_;
};