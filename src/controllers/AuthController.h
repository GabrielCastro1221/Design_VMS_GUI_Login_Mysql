#pragma once

#include <string>

class AuthController
{
public:
    bool login(
        const std::string &username,
        const std::string &password);

    void logout();
};