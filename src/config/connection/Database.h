#pragma once

#include <mysql/mysql.h>
#include <string>

class Database
{
private:
    static Database* instance;
    MYSQL* conn;

    Database();

public:
    ~Database();

    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;

    static Database* getInstance();

    bool connect(
        const std::string& host,
        const std::string& user,
        const std::string& password,
        const std::string& database,
        unsigned int port = 3306);

    MYSQL* getConnection();

    bool isConnected() const;

    void disconnect();
};