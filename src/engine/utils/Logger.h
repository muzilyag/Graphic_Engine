#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <vector>
#include <memory>
#include <fstream>

enum class LogLevel
{
    INFO,
    WARNING,
    ERR
};

// Interface
class ILogHandler
{
public:
    virtual ~ILogHandler() = default;
    virtual void handle(LogLevel level, const std::string& message) = 0;
};

// Console Output
class ConsoleHandler : public ILogHandler
{
public:
    virtual void handle(LogLevel level, const std::string& message) override;
};

// File Output
class FileHandler : public ILogHandler
{
    std::ofstream file;
public:
    explicit FileHandler(const std::string& fileName);
    ~FileHandler();
    virtual void handle(LogLevel level, const std::string& message) override;
};

// Core System (Static)
class LogSystem
{
    static std::vector<std::unique_ptr<ILogHandler>>& getHandlers();
    static std::string getTimestamp();

public:
    static void addHandler(std::unique_ptr<ILogHandler> handler);
    static void dispatch(LogLevel level, const std::string& context, const std::string& message);
};

// Component (To be used in Mesh, Window, etc.)
class Logger
{
    std::string contextName;
public:
    Logger() = delete;
    explicit Logger(std::string name);
    void info(const std::string& message) const;
    void warn(const std::string& message) const;
    void error(const std::string& message) const;
};

#endif