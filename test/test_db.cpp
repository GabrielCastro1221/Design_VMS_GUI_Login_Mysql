#include "../src/config/Env.h"
#include "../src/config/connection/Database.h"
#include <iostream>
#include <string>

int main()
{
    env::load(".env");
    env::load("../.env");

    const std::string host = env::get("DB_HOST", "localhost");
    const std::string user = env::get("DB_USER");
    const std::string password = env::get("DB_PASSWORD");
    const std::string name = env::get("DB_NAME", "vms_db");
    const unsigned int port = env::getInt("DB_PORT", 3306);

    if (user.empty())
    {
        std::cerr << "Falta DB_USER. Revisa el archivo .env\n";
        return 1;
    }

    Database *db = Database::getInstance();

    bool ok = db->connect(host, user, password, name, port);

    if (!ok)
    {
        std::cerr << "Fallo la conexion a '" << name << "' en "
                  << host << ":" << port << "\n";
        return 1;
    }

    std::cout << "Conexion exitosa a " << name << "\n";
    db->disconnect();
    return 0;
}