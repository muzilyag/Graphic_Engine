#include "engine/utils/Logger.h"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <utility>

void ConsoleHandler::handle(LogLevel level, const std::string& message)
{
    std::cout << message << std::endl;
}

FileHandler::FileHandler(const std::string& fileName)
{
    file.open(fileName, std::ios::app);
}

FileHandler::~FileHandler()
{
    if (file.is_open())
    {
        file.close();
    }
}

void FileHandler::handle(LogLevel level, const std::string& message)
{
    if (file.is_open())
    {
        file << message << std::endl;
        file.flush();
    }
}

std::vector<std::unique_ptr<ILogHandler>>& LogSystem::getHandlers()
{
    static std::vector<std::unique_ptr<ILogHandler>> handlers;
    return handlers;
}

std::string LogSystem::getTimestamp()
{
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::tm buf;
    
    localtime_s(&buf, &in_time_t);
    
    std::stringstream ss;
    ss << std::put_time(&buf, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

void LogSystem::addHandler(std::unique_ptr<ILogHandler> handler)
{
    getHandlers().push_back(std::move(handler));
}

void LogSystem::dispatch(LogLevel level, const std::string& context, const std::string& message)
{
    std::string levelStr;
    switch (level)
    {
    case LogLevel::INFO:    
        levelStr = "INFO"; 
        break;
    case LogLevel::WARNING: 
        levelStr = "WARN"; 
        break;
    case LogLevel::ERR:     
        levelStr = "ERROR";
    }

    std::string finalMsg = "[" + getTimestamp() + "] [" + levelStr + "] [" + context + "] " + message;

    for (auto& h : getHandlers())
    {
        h->handle(level, finalMsg);
    }
}

Logger::Logger(std::string name) : contextName(std::move(name))
{
}

void Logger::info(const std::string& message) const
{
    LogSystem::dispatch(LogLevel::INFO, contextName, message);
}

void Logger::warn(const std::string& message) const
{
    LogSystem::dispatch(LogLevel::WARNING, contextName, message);
}

void Logger::error(const std::string& message) const
{
    LogSystem::dispatch(LogLevel::ERR, contextName, message);
}