#include "PasswordHasher.h"
#include <bcrypt/BCrypt.hpp>

std::string PasswordHasher::hash(const std::string &password)
{
    return BCrypt::generateHash(password, 12);
}

bool PasswordHasher::verify(const std::string &password, const std::string &hash)
{
    return BCrypt::validatePassword(password, hash);
}