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




#include "helper.hpp"
#include "vars.h"
#include "hooks_indices.h"
#include "hooks_dvb.h"
#include "hooks_indexbuffer.h"
#include "hooks_vertexbuffer.h"


const WCHAR* EVENT_NAME = L"Global\\L4D2_DLL_Completed";

void InitConsole() {
    AllocConsole();
    FILE *dummy;
    freopen_s(&dummy, "CONOUT$", "w", stdout);
}

void Logging() {
    if (cfg::System::debug) {
        InitConsole();
    }
    // Get this module path
    WCHAR thisModulePath[_MAX_PATH] = { 0 };
    GetModuleFileNameW(thisModule, thisModulePath, MAX_PATH);
    sThisModulePath = thisModulePath;
    sThisModulePath = sThisModulePath.remove_filename();

    // Get game name and exe path
    WCHAR exePath[_MAX_PATH] = { 0 };
    GetModuleFileNameW(baseModule, exePath, MAX_PATH);
    sExePath = exePath;
    sExeName = sExePath.filename().string();
    sExePath = sExePath.remove_filename();

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
        spdlog::info("Log file: {}", sThisModulePath.string() + sLogFile);
        spdlog::info("----------");

        // Log module details
        spdlog::info("Module Name: {0:s}", sExeName.c_str());
        spdlog::info("Module Path: {0:s}", sExePath.string());
        spdlog::info("Module Address: 0x{0:x}", (uintptr_t)baseModule);
        spdlog::info("Module Timestamp: {0:d}", Memory::ModuleTimestamp(baseModule));
        spdlog::info("----------");
    } catch (const spdlog::spdlog_ex &ex) {
        InitConsole();
    }
}



DWORD __stdcall Main(void*) {
    LoadIni();
    Logging();

    std::wstring cmdline = GetCommandLineW();
    spdlog::info(L"Startup commandline: {}", cmdline.c_str());

    std::wstring dllName = L"shaderapidx9.dll";
    bool isVulkan = false;

    std::transform(cmdline.cbegin(), cmdline.cend(), cmdline.begin(), ::tolower);
    if (cmdline.find(L"-vulkan") != std::wstring::npos) {
        dllName = L"shaderapivk.dll";
        spdlog::info(L"Detected Vulkan: using {}", dllName);
        isVulkan = true;
    } else {
        // Preload d3d9.dll
        LoadLibraryW(L"d3d9.dll");
        spdlog::info(L"Detected DirectX9: using {}", dllName);
    }

    auto relPath = std::filesystem::path(L"bin") / dllName;
    auto absPath = std::filesystem::absolute(relPath);

    std::wcout << absPath << std::endl;
    spdlog::info(L"Loading Dll from {}", absPath.wstring());

    HMODULE hDll = LoadLibraryExW(absPath.c_str(), nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);

    if (hDll == nullptr) {
        auto errCode = GetLastError();
        wchar_t buf[255];
        FormatMessageW(
            FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, errCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            buf, (sizeof(buf) / sizeof(wchar_t)), NULL);
        std::wcout << errCode << std::endl;
        std::wcout << buf << std::endl;
        spdlog::error(L"Failed to load {}!", dllName);
        exit(-1);
    }

    // Not known

    auto ret = 0;
    ret += PatchIndices(hDll, dllName);
    ret += hooks_dvb(hDll, dllName);

    // Experiments
    // ret += hooks_indexbuffer(hDll, dllName);
    // ret += VertexBuffer::hooks_vertexbuffer(hDll, dllName);

    if (ret != 0) {
        spdlog::error(L"未能正常应用补丁，patch 未生效");
    } 

    return TRUE;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    if (DetourIsHelperProcess()) {
        return TRUE;
    }
    thisModule = hModule;
    
   
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH: {
        // 获取主线程句柄
        Main(nullptr);   
        HANDLE hEvent = OpenEventW(EVENT_MODIFY_STATE, FALSE, EVENT_NAME);
        if (hEvent) {
            SetEvent(hEvent);
            CloseHandle(hEvent);
        }
        break;
    }
    case DLL_PROCESS_DETACH: {
        break;
    }}
    return TRUE;
}

// Detour needs at least on exported function
void __declspec(dllexport) kuriko_export() {}
