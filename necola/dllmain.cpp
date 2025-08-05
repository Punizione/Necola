#define SPDLOG_WCHAR_TO_UTF8_SUPPORT

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

#include <windows.h>

#include <cstdint>
#include <cstddef>
#include <iostream>
#include <filesystem>
#include <fstream>


#include <algorithm>
#include <detours/detours.h>
#include <MinHook.h>
#include <inipp.h>


#include "hook/Entry.h"


#include "vars.h"




void InitConsole() {
    AllocConsole();
    FILE *dummy;
    freopen_s(&dummy, "CONOUT$", "w", stdout);
}

void Logging() {
    if (cfg::System::debug) {
        InitConsole();
    }

    // spdlog initialisation
    try {
        auto logger = spdlog::basic_logger_mt(sFixName, sLogFile, true);
        spdlog::set_default_logger(logger);

        auto start_time = std::chrono::system_clock::now();

        if (cfg::System::debug) {
            spdlog::set_level(spdlog::level::debug);
        }
        spdlog::flush_on(spdlog::level::debug);
        spdlog::info("----------");
        spdlog::info("Date: {}", std::chrono::system_clock::to_time_t(start_time));
        spdlog::info("{} v{} loaded.", sFixName.c_str(), sFixVer.c_str());
        spdlog::info("----------");


    } catch (const spdlog::spdlog_ex &ex) {
        InitConsole();
    }
}


DWORD __stdcall Hook_necola(LPVOID lpParam)
{
    HANDLE hProcess = OpenProcess(SYNCHRONIZE, FALSE, GetCurrentProcessId());
    if (!hProcess) return 1;

    LoadIni();
    Logging();
    std::wstring cmdline = cfg::System::cmdLine;
    if(cmdline.empty())
    {
        std::wstring cmdline = GetCommandLineW();
    }
    spdlog::info(L"Startup commandline: {}", cmdline.c_str());
    // InitConsole();
    spdlog::info("Necola Start!");
    G::ModuleEntry.Load();

    CloseHandle(hProcess);
    return 0;
}

void Undo_necola()
{
    spdlog::info("Stop L4D2 inject process");
    G::ModuleEntry.undo();
}


BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    if (DetourIsHelperProcess()) {
        return TRUE;
    }

    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH: {
        DisableThreadLibraryCalls(hModule);
        if (auto h = CreateThread(NULL, 0, Hook_necola, NULL, 0, NULL)) {
            CloseHandle(h);
        }
            
        break;
    }
    //case DLL_PROCESS_DETACH: {
    //    Undo_necola();
    //    break;
    //}
}
    return TRUE;
}

// Detour needs at least on exported function
void __declspec(dllexport) necola_export() {}
