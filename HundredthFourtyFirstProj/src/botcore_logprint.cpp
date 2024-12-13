#include "botcore_logprint.hxx"
#include <iomanip>
#include <sstream>
#include <fstream>
#include <chrono>
#include <cstring>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>

#define DEFAULT_COLOR "\033[0m"
#define ERROR_COLOR "\033[31m"
#define END_STRING "\033[0m\n"

const char *logprint::getTimestamp()
{
    static char buffer[20];

    auto now = std::chrono::system_clock::now();
    std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);
    std::tm now_tm = *std::localtime(&now_time_t);

    std::ostringstream oss;
    oss << std::put_time(&now_tm, "%Y-%m-%d %H:%M:%S");

    std::strncpy(buffer, oss.str().c_str(), sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';

    return buffer;
}

char *logprint::generateFormattedString(const char *format, va_list args)
{
    size_t buffer_size = 256;
    char *buffer = (char *)malloc(buffer_size);

    if (buffer == nullptr)
    {
        return nullptr;
    }

    while (true)
    {
        int n = vsnprintf(buffer, buffer_size, format, args);

        if (n < 0)
        {
            free(buffer);
            return nullptr;
        }
        else if (n < (int)buffer_size)
        {
            break;
        }
        else
        {
            buffer_size = n + 1;
            char *new_buffer = (char *)realloc(buffer, buffer_size);
            if (new_buffer == nullptr)
            {
                free(buffer);
                return nullptr;
            }
            buffer = new_buffer;
        }
    }

    size_t finalSize = strlen(buffer) + strlen(tag) + strlen(getTimestamp()) + 50;
    char *finalMessage = (char *)malloc(finalSize);
    if (finalMessage != nullptr)
    {
        snprintf(finalMessage, finalSize, "%s > [%s]: %s", getTimestamp(), tag, buffer);
    }

    free(buffer);
    return finalMessage;
}

void logprint::writeLog(const char *message)
{
    std::ofstream logFile(logPath, std::ios::app);

    if (logFile.is_open())
    {
        logFile << message << "\n";
        logFile.close();
    }
    else
    {
        printf(ERROR_COLOR "Failed to open file for writing." END_STRING);
    }
}

void logprint::LOGI(const char *message, ...)
{
    va_list args;
    va_start(args, message);

    char *finalMessage = generateFormattedString(message, args);
    va_end(args);

    if (finalMessage != nullptr)
    {
        printf(DEFAULT_COLOR "%s" END_STRING, finalMessage);
        writeLog(finalMessage);
        free(finalMessage);
    }
    else
    {
        printf(ERROR_COLOR "Error generating log message" END_STRING);
    }
}

void logprint::LOGE(const char *message, ...)
{
    va_list args;
    va_start(args, message);

    char *finalMessage = generateFormattedString(message, args);
    va_end(args);

    if (finalMessage != nullptr)
    {
        printf(ERROR_COLOR "%s" END_STRING, finalMessage);
        writeLog(finalMessage);
        free(finalMessage);
    }
    else
    {
        printf(ERROR_COLOR "Error generating log message" END_STRING);
    }
}
