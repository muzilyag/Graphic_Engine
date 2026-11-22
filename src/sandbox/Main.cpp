#include "sandbox/SandboxApp.h"
#include "engine/utils/Logger.h"

int WINAPI WinMain(HINSTANCE h, HINSTANCE hp, LPSTR c, int s)
{
    LogSystem::addHandler(std::make_unique<ConsoleHandler>());
    LogSystem::addHandler(std::make_unique<FileHandler>("engine_log.log"));
    SandboxApp app;
    app.run();
    return 0;
}