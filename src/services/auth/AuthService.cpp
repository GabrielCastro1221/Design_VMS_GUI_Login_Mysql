#include "AuthService.h"
#include "controllers/AuthController.h"
#include "session/Session.h"

bool AuthService::login(const std::string &username, const std::string &password)
{
    AuthController controller;
    return controller.login(username, password);
}

void AuthService::logout()
{
    AuthController controller;
    controller.logout();
}

bool AuthService::isLoggedIn() const
{
    return Session::getInstance().isLoggedIn();
}