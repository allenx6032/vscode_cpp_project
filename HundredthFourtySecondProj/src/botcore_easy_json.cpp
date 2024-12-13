#include <iostream>
#include <fstream>
#include <botcore_json.hpp>
#include <sstream>
#include <botcore_easy_json.hxx>

using json = nlohmann::json;

namespace local_easy_json_functions
{
    std::vector<std::string> split(const std::string &str, char delimiter)
    {
        std::vector<std::string> tokens;
        std::stringstream ss(str);
        std::string token;
        while (std::getline(ss, token, delimiter))
        {
            tokens.push_back(token);
        }
        return tokens;
    }
};

namespace easy_json
{

    int file_get_value_string(const char *file_path, const char *key_path, std::string &value)
    {
        std::ifstream file(file_path);
        if (!file.is_open())
        {
            return ERROR_LOADING_FILE;
        }

        json j;
        try
        {
            file >> j;
        }
        catch (const json::parse_error &e)
        {
            return ERROR_PARSING;
        }

        std::vector<std::string> keys = local_easy_json_functions::split(key_path, '.');
        json current = j;
        for (const std::string &key : keys)
        {
            if (current.contains(key))
            {
                current = current[key];
            }
            else
            {
                return ERROR_KEY_PATH;
            }
        }

        try
        {
            value = current.get<std::string>();
        }
        catch (const json::type_error &e)
        {
            return ERROR_INVALID_TYPE_DATA;
        }

        return NO_ERROR;
    }

    int file_get_value_int(const char *file_path, const char *key_path, int &value)
    {
        std::ifstream file(file_path);
        if (!file.is_open())
        {
            return ERROR_LOADING_FILE;
        }

        json j;
        try
        {
            file >> j;
        }
        catch (const json::parse_error &e)
        {
            return ERROR_PARSING;
        }

        std::vector<std::string> keys = local_easy_json_functions::split(key_path, '.');
        json current = j;
        for (const std::string &key : keys)
        {
            if (current.contains(key))
            {
                current = current[key];
            }
            else
            {
                return ERROR_KEY_PATH;
            }
        }

        try
        {
            value = current.get<int>();
        }
        catch (const json::type_error &e)
        {
            return ERROR_INVALID_TYPE_DATA;
        }

        return NO_ERROR;
    }

    int file_get_value_bool(const char *file_path, const char *key_path, bool &value)
    {
        std::ifstream file(file_path);
        if (!file.is_open())
        {
            return ERROR_LOADING_FILE;
        }

        json j;
        try
        {
            file >> j;
        }
        catch (const json::parse_error &e)
        {
            return ERROR_PARSING;
        }

        std::vector<std::string> keys = local_easy_json_functions::split(key_path, '.');
        json current = j;
        for (const std::string &key : keys)
        {
            if (current.contains(key))
            {
                current = current[key];
            }
            else
            {
                return ERROR_KEY_PATH;
            }
        }

        try
        {
            value = current.get<bool>();
        }
        catch (const json::type_error &e)
        {
            return ERROR_INVALID_TYPE_DATA;
        }

        return NO_ERROR;
    }

};
