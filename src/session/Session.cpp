#include "Session.h"

Session &Session::getInstance()
{
    static Session instance;
    return instance;
}

bool Session::isLoggedIn() const
{
    return currentUser.id != 0;
}

void Session::clear()
{
    currentUser = User{};
}