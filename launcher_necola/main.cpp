#include <windows.h>
#include <string>
#include <filesystem>
#include <fstream>
#include <shellapi.h>
#include <locale>
#include <codecvt>

#include <detours/detours.h>
#include <inipp.h>

namespace fs = std::filesystem;
std::wstring game_name = L"left4dead2.exe";
std::wstring cmdline = L"";

void init_cfg() {
    std::ifstream is("kpatch.ini");
    inipp::Ini<char> ini;

    ini.parse(is);
    std::string tmp_name;
    inipp::extract(ini.sections["System"]["target"], tmp_name);
    std::string tmp_cmdline;
    inipp::extract(ini.sections["System"]["cmdline"], tmp_cmdline);
    game_name = std::wstring(tmp_name.begin(), tmp_name.end());
    cmdline = std::wstring(tmp_cmdline.begin(), tmp_cmdline.end());
}

int WINAPI wWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpwCmdLine,
    _In_ int nShowCmd
) {
    init_cfg();

    WCHAR working_path[MAX_PATH];
    GetModuleFileNameW(nullptr, working_path, MAX_PATH);

    // Change the working directory to the directory containing the DLL.
    fs::path path(working_path);
    SetCurrentDirectoryW(path.parent_path().wstring().c_str());

    LPCSTR dll_path = "necola.dll";

    LPCWSTR target_exe_path = game_name.c_str();
    LPWSTR final_cmd_line = cmdline.data();

    STARTUPINFOW si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi));

    si.cb = sizeof(si);
    // 
    DWORD dwFlags =  CREATE_SUSPENDED;
    //SetEnvironmentVariableW(L"FINAL_CMD_LINE", final_cmd_line);
    SetLastError(0);
    if (TRUE != DetourCreateProcessWithDllExW(
            target_exe_path,
            final_cmd_line,
            nullptr,
            nullptr,
            FALSE,
            dwFlags,
            nullptr,
            nullptr,
            &si,
            &pi,
            dll_path,
            nullptr)) {
        auto dwError = GetLastError();
        printf("DetourCreateProcessWithDllEx failed with error %ld\n", dwError);

        ExitProcess(9009);
    }
    ResumeThread(pi.hThread);

    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

    // ResumeThread(pi.hThread);
    //WaitForSingleObject(pi.hProcess, INFINITE);

    // CloseHandle(pi.hProcess);
    // CloseHandle(pi.hThread);

    return 0;
}
