#include "Database.h"

#include <iostream>

Database* Database::instance = nullptr;

Database::Database()
{
    conn = nullptr;
}

Database::~Database()
{
    disconnect();
}

Database* Database::getInstance()
{
    if (instance == nullptr)
    {
        instance = new Database();
    }

    return instance;
}

bool Database::connect(
    const std::string& host,
    const std::string& user,
    const std::string& password,
    const std::string& database,
    unsigned int port)
{
    if (conn != nullptr)
    {
        return true;
    }

    conn = mysql_init(nullptr);

    if (conn == nullptr)
    {
        std::cerr << "Error MYSQL: mysql_init()" << std::endl;
        return false;
    }

    mysql_options(conn, MYSQL_SET_CHARSET_NAME, "utf8mb4");

    if (!mysql_real_connect(
            conn,
            host.c_str(),
            user.c_str(),
            password.c_str(),
            database.c_str(),
            port,
            nullptr,
            0))
    {
        std::cerr << "MYSQL ERROR: "
                  << mysql_error(conn)
                  << std::endl;

        mysql_close(conn);
        conn = nullptr;

        return false;
    }

    std::cout << "Conexion exitosa a "
              << database
              << std::endl;

    return true;
}

MYSQL* Database::getConnection()
{
    return conn;
}

bool Database::isConnected() const
{
    return conn != nullptr;
}

void Database::disconnect()
{
    if (conn != nullptr)
    {
        mysql_close(conn);
        conn = nullptr;
    }
}