#pragma once

#include <cstdlib>
#include <fstream>
#include <string>

namespace env
{
    inline std::string trim(const std::string &s)
    {
        const char *ws = " \t\r\n";
        const auto start = s.find_first_not_of(ws);
        if (start == std::string::npos)
            return "";
        const auto end = s.find_last_not_of(ws);
        return s.substr(start, end - start + 1);
    }

    inline bool load(const std::string &path)
    {
        std::ifstream file(path);
        if (!file.is_open())
            return false;

        std::string line;
        while (std::getline(file, line))
        {
            line = trim(line);
            if (line.empty() || line[0] == '#')
                continue;

            if (line.rfind("export ", 0) == 0)
                line = trim(line.substr(7));

            const auto eq = line.find('=');
            if (eq == std::string::npos)
                continue;

            const std::string key = trim(line.substr(0, eq));
            std::string value = trim(line.substr(eq + 1));

            if (value.size() >= 2 &&
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\'')))
            {
                value = value.substr(1, value.size() - 2);
            }

            if (!key.empty())
                setenv(key.c_str(), value.c_str(), 0);
        }
        return true;
    }

    inline std::string get(const char *name, const std::string &fallback = "")
    {
        const char *v = std::getenv(name);
        return v ? std::string(v) : fallback;
    }

    inline unsigned int getInt(const char *name, unsigned int fallback)
    {
        const char *v = std::getenv(name);
        if (!v)
            return fallback;
        char *end = nullptr;
        const unsigned long n = std::strtoul(v, &end, 10);
        return (end != v && *end == '\0') ? static_cast<unsigned int>(n) : fallback;
    }
}