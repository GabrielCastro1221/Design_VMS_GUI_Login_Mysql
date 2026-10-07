#pragma once

#include "../models/User.h"

class Session
{
private:
    Session() = default;

public:
    static Session &getInstance();

    User currentUser;

    bool isLoggedIn() const;
    void clear();
};