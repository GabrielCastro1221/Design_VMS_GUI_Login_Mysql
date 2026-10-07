#pragma once

#include <string>

class AuthService
{
public:
    bool login(const std::string &username, const std::string &password);
    void logout();
    bool isLoggedIn() const;
};