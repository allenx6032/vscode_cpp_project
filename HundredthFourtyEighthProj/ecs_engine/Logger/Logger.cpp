#include "Engine/Logger/Logger.h"
#include <iostream>
#include <iomanip>
#include <ctime>
#include <chrono>
#include <string>

#define ESC "\033["
#define GREEN_TEXT "32m"
#define RED_TEXT "31m"
#define RESET "\033[m"

std::vector<LogEntry> Logger::messages;

std::string CurrentDateTimeToString() {
    std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::string output(30, '\0');
    std::strftime(&output[0], output.size(), "%d-%b-%Y %H:%M:%S", std::localtime(&now));
    return output;
}

void Logger::Log(const std::string& message) {
    LogEntry logEntry;
    logEntry.type = LOG_INFO;
    logEntry.message = "LOG | " + CurrentDateTimeToString() + " - " + message; 
    std::cout << ESC << GREEN_TEXT << logEntry.message << RESET << std::endl;
    messages.push_back(logEntry);
}

void Logger::Err(const std::string& message) {
    LogEntry logEntry;
    logEntry.type = LOG_ERROR;
    logEntry.message = "ERR | " + CurrentDateTimeToString() + " - " + message;
    std::cout << ESC << RED_TEXT << logEntry.message << RESET << std::endl;
    messages.push_back(logEntry);
}