#pragma once

#include "../models/User.h"
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

class AuthRepository
{
public:
    struct UserWithHash
    {
        User user;
        std::string passwordHash;
    };

    std::optional<UserWithHash> findByUsername(const std::string &username);

    std::vector<std::string> getPermissions(uint64_t userId);
    std::string getRole(uint64_t userId);
};