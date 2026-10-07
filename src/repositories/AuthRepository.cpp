#include "AuthRepository.h"
#include "../config/connection/Database.h"
#include <mysql/mysql.h>
#include <cstring>

std::optional<AuthRepository::UserWithHash>
AuthRepository::findByUsername(const std::string &username)
{
    MYSQL *conn = Database::getInstance()->getConnection();
    if (!conn)
        return std::nullopt;

    MYSQL_STMT *stmt = mysql_stmt_init(conn);
    if (!stmt)
        return std::nullopt;

    const char *sql = "SELECT id, username, full_name, password_hash "
                      "FROM users WHERE username = ? LIMIT 1";

    if (mysql_stmt_prepare(stmt, sql, strlen(sql)))
    {
        mysql_stmt_close(stmt);
        return std::nullopt;
    }

    MYSQL_BIND bindParam{};
    unsigned long usernameLen = username.length();

    bindParam.buffer_type = MYSQL_TYPE_STRING;
    bindParam.buffer = const_cast<char *>(username.c_str());
    bindParam.buffer_length = usernameLen;
    bindParam.length = &usernameLen;

    if (mysql_stmt_bind_param(stmt, &bindParam))
    {
        mysql_stmt_close(stmt);
        return std::nullopt;
    }

    if (mysql_stmt_execute(stmt))
    {
        mysql_stmt_close(stmt);
        return std::nullopt;
    }

    uint64_t id = 0;
    char usernameBuf[128]{};
    char fullNameBuf[256]{};
    char hashBuf[128]{};

    unsigned long usernameLenRes = 0;
    unsigned long fullNameLenRes = 0;
    unsigned long hashLenRes = 0;

    MYSQL_BIND bindResult[4]{};
    memset(bindResult, 0, sizeof(bindResult));

    bindResult[0].buffer_type = MYSQL_TYPE_LONGLONG;
    bindResult[0].buffer = &id;

    bindResult[1].buffer_type = MYSQL_TYPE_STRING;
    bindResult[1].buffer = usernameBuf;
    bindResult[1].buffer_length = sizeof(usernameBuf);
    bindResult[1].length = &usernameLenRes;

    bindResult[2].buffer_type = MYSQL_TYPE_STRING;
    bindResult[2].buffer = fullNameBuf;
    bindResult[2].buffer_length = sizeof(fullNameBuf);
    bindResult[2].length = &fullNameLenRes;

    bindResult[3].buffer_type = MYSQL_TYPE_STRING;
    bindResult[3].buffer = hashBuf;
    bindResult[3].buffer_length = sizeof(hashBuf);
    bindResult[3].length = &hashLenRes;

    if (mysql_stmt_bind_result(stmt, bindResult))
    {
        mysql_stmt_close(stmt);
        return std::nullopt;
    }

    if (mysql_stmt_store_result(stmt))
    {
        mysql_stmt_close(stmt);
        return std::nullopt;
    }

    if (mysql_stmt_fetch(stmt) != 0)
    {
        mysql_stmt_close(stmt);
        return std::nullopt;
    }

    UserWithHash result;
    result.user.id = id;
    result.user.username = std::string(usernameBuf, usernameLenRes);
    result.user.fullName = std::string(fullNameBuf, fullNameLenRes);
    result.passwordHash = std::string(hashBuf, hashLenRes);

    mysql_stmt_close(stmt);
    return result;
}

std::string AuthRepository::getRole(uint64_t userId)
{
    MYSQL *conn = Database::getInstance()->getConnection();
    if (!conn)
        return "";

    MYSQL_STMT *stmt = mysql_stmt_init(conn);
    if (!stmt)
        return "";

    const char *sql = "SELECT r.name "
                      "FROM users u "
                      "JOIN roles r ON u.role_id = r.id "
                      "WHERE u.id = ? LIMIT 1";

    if (mysql_stmt_prepare(stmt, sql, strlen(sql)))
    {
        mysql_stmt_close(stmt);
        return "";
    }

    MYSQL_BIND bindParam{};
    bindParam.buffer_type = MYSQL_TYPE_LONGLONG;
    bindParam.buffer = &userId;

    if (mysql_stmt_bind_param(stmt, &bindParam) || mysql_stmt_execute(stmt))
    {
        mysql_stmt_close(stmt);
        return "";
    }

    char roleBuf[64]{};
    unsigned long roleLen = 0;

    MYSQL_BIND bindResult{};
    bindResult.buffer_type = MYSQL_TYPE_STRING;
    bindResult.buffer = roleBuf;
    bindResult.buffer_length = sizeof(roleBuf);
    bindResult.length = &roleLen;

    if (mysql_stmt_bind_result(stmt, &bindResult) ||
        mysql_stmt_store_result(stmt) ||
        mysql_stmt_fetch(stmt) != 0)
    {
        mysql_stmt_close(stmt);
        return "";
    }

    std::string role(roleBuf, roleLen);
    mysql_stmt_close(stmt);
    return role;
}

std::vector<std::string> AuthRepository::getPermissions(uint64_t userId)
{
    std::vector<std::string> permissions;

    MYSQL *conn = Database::getInstance()->getConnection();
    if (!conn)
        return permissions;

    MYSQL_STMT *stmt = mysql_stmt_init(conn);
    if (!stmt)
        return permissions;

    const char *sql = "SELECT p.name "
                      "FROM permissions p "
                      "JOIN role_permissions rp ON p.id = rp.permission_id "
                      "JOIN users u ON u.role_id = rp.role_id "
                      "WHERE u.id = ?";

    if (mysql_stmt_prepare(stmt, sql, strlen(sql)))
    {
        mysql_stmt_close(stmt);
        return permissions;
    }

    MYSQL_BIND bindParam{};
    bindParam.buffer_type = MYSQL_TYPE_LONGLONG;
    bindParam.buffer = &userId;

    if (mysql_stmt_bind_param(stmt, &bindParam) || mysql_stmt_execute(stmt))
    {
        mysql_stmt_close(stmt);
        return permissions;
    }

    char permBuf[128]{};
    unsigned long permLen = 0;

    MYSQL_BIND bindResult{};
    bindResult.buffer_type = MYSQL_TYPE_STRING;
    bindResult.buffer = permBuf;
    bindResult.buffer_length = sizeof(permBuf);
    bindResult.length = &permLen;

    if (mysql_stmt_bind_result(stmt, &bindResult) ||
        mysql_stmt_store_result(stmt))
    {
        mysql_stmt_close(stmt);
        return permissions;
    }

    while (mysql_stmt_fetch(stmt) == 0)
    {
        permissions.emplace_back(permBuf, permLen);
    }

    mysql_stmt_close(stmt);
    return permissions;
}