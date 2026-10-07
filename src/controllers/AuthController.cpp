#include "AuthController.h"
#include "../repositories/AuthRepository.h"
#include "../session/Session.h"
#include "../security/PasswordHasher.h"

bool AuthController::login(
    const std::string &username,
    const std::string &password)
{
    AuthRepository repository;

    auto result = repository.findByUsername(username);

    if (!result.has_value())
    {
        return false;
    }

    if (!PasswordHasher::verify(password, result->passwordHash))
    {
        return false;
    }

    User user = result->user;
    user.role = repository.getRole(user.id);
    user.permissions = repository.getPermissions(user.id);

    Session::getInstance().currentUser = user;

    return true;
}

void AuthController::logout()
{
    Session::getInstance().clear();
}