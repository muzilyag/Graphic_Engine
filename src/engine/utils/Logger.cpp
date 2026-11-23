#include "engine/utils/Logger.h"
#include <chrono>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>
#include <utility>

namespace
{
    std::mutex& getLogMutex()
    {
        static std::mutex mtx;
        return mtx;
    }
}

void ConsoleHandler::handle(LogLevel level, const std::string& message)
{
    (void)level;
    std::cout << message << '\n';
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
    (void)level;
    if (file.is_open())
    {
        file << message << '\n';
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
    std::tm buf{};

#ifdef _WIN32
    localtime_s(&buf, &in_time_t);
#else
    localtime_r(&in_time_t, &buf);
#endif

    std::stringstream ss;
    ss << std::put_time(&buf, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

void LogSystem::addHandler(std::unique_ptr<ILogHandler> handler)
{
    std::lock_guard<std::mutex> lock(getLogMutex());
    getHandlers().push_back(std::move(handler));
}

void LogSystem::dispatch(LogLevel level, const std::string& context, const std::string& message)
{
    const char* levelStr = "INFO";
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
        break;
    default:
        levelStr = "UNKNOWN";
        break;
    }

    std::string finalMsg = "[" + getTimestamp() + "] [" + levelStr + "] [" + context + "] " + message;

    std::lock_guard<std::mutex> lock(getLogMutex());
    for (auto& h : getHandlers())
    {
        if (h)
        {
            h->handle(level, finalMsg);
        }
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