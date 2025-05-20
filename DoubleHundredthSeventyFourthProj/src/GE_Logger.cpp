#include "GE_pch.h"

#include "GE_Logger.h"

std::vector<LogEntry> Logger::m_messages;

void Logger::Log(const std::string& messageToDisplay)
{
	LogEntry logEntry;
	logEntry.m_type = LogType::info;
	logEntry.m_message = "LOG : [" + CurrentDateTimeToString() +"]: " + messageToDisplay;
	
	std::cout << "\x1B[32m" << logEntry.m_message << "\033[0m" << std::endl;
	
	m_messages.push_back(logEntry);
}

void Logger::Error(const std::string& messageToDisplay)
{
	LogEntry logEntry;
	logEntry.m_type = LogType::error;
	logEntry.m_message = "ERR : [" + CurrentDateTimeToString() + "]: " + messageToDisplay;
	
	std::cerr << "\x1B[91m" << logEntry.m_message << "\033[0m" << std::endl;

	m_messages.push_back(logEntry);
}

void Logger::InitInfo(const std::string& messageToDisplay)
{
	LogEntry logEntry;
	logEntry.m_type = LogType::initInfo;
	logEntry.m_message = "INIT INFO: [" + CurrentDateTimeToString() + "]: " + messageToDisplay;

	std::cout << "\x1B[02m" << logEntry.m_message << "\033[0m" << std::endl;

	m_messages.push_back(logEntry);
}

std::string Logger::CurrentDateTimeToString()
{
	std::string output(30, '\0');

	std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	std::strftime(&output[0], output.size(), "%d-%b-%Y %H:%M:%S", std::localtime(&now));

	return output;
}


