#pragma once

#include <functional>
#include <string>
#include <imgui.h>

class VideoPlayer;

class LoginWindow
{
public:
    using LoginCallback = std::function<bool(const std::string &user, const std::string &password)>;

    explicit LoginWindow(LoginCallback onLogin);

    void setBackground(const VideoPlayer *video) { background_ = video; }

    void render();
    bool isLoggedIn() const { return loggedIn_; }
    const std::string &username() const { return loggedUser_; }
    void logout();

private:
    void submit();
    void drawGlass(const ImVec2 &p0, const ImVec2 &p1) const;

    LoginCallback onLogin_;
    const VideoPlayer *background_ = nullptr;
    char usernameBuf_[64] = "";
    char passwordBuf_[64] = "";
    std::string error_;
    std::string loggedUser_;
    bool loggedIn_ = false;
    bool focusUser_ = true;
};