#pragma once

#include <string>
#include <vector>
#include <cstdint>

struct User
{
    uint64_t id = 0;
    std::string username;
    std::string fullName;
    std::string role;
    std::vector<std::string> permissions;
};