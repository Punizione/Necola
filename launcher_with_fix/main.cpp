
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

// const wchar_t* game_name = L"left4dead2.exe";
bool debug = false;
std::wstring game_name = L"left4dead2.exe";
std::wstring cmdline = L"";
const WCHAR* EVENT_NAME = L"Global\\L4D2_DLL_Completed";

void init_cfg() {
    std::ifstream is("kpatch.ini");
    inipp::Ini<char> ini;

    ini.parse(is);
    inipp::extract(ini.sections["System"]["debug"], debug);
    std::string tmp_name;
    inipp::extract(ini.sections["System"]["target"], tmp_name);
    std::string tmp_cmdline;
    inipp::extract(ini.sections["System"]["cmdline"], tmp_cmdline);
    game_name = std::wstring(tmp_name.begin(), tmp_name.end());
    cmdline = std::wstring(tmp_cmdline.begin(), tmp_cmdline.end());
}



BOOL InjectDLL(DWORD pid, const WCHAR* dllPath) {
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (!hProcess) return FALSE;

    LPVOID pDllPath = VirtualAllocEx(hProcess, NULL, (wcslen(dllPath)+1)*2, MEM_COMMIT, PAGE_READWRITE);
    if (!pDllPath) {
        CloseHandle(hProcess);
        return FALSE;
    }

    WriteProcessMemory(hProcess, pDllPath, dllPath, (wcslen(dllPath)+1)*2, NULL);
    LPTHREAD_START_ROUTINE pLoadLibrary = 
        (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryW");

    HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, pLoadLibrary, pDllPath, 0, NULL);
    if (!hThread) {
        VirtualFreeEx(hProcess, pDllPath, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return FALSE;
    }
    WaitForSingleObject(hThread, INFINITE);
    VirtualFreeEx(hProcess, pDllPath, 0, MEM_RELEASE);
    CloseHandle(hThread);
    CloseHandle(hProcess);
    return TRUE;
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

    fs::path path(working_path);
    SetCurrentDirectoryW(path.parent_path().wstring().c_str());

    LPCSTR dll_path = "kpatch.dll";

    LPCWSTR target_exe_path = game_name.c_str();
    LPWSTR final_cmd_line = cmdline.data();

    STARTUPINFOW si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi));

    si.cb = sizeof(si);

    DWORD dwFlags = CREATE_SUSPENDED;
    HANDLE hEvent = CreateEventW(NULL, TRUE, FALSE, EVENT_NAME);
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
        CloseHandle(hEvent);
        ExitProcess(9009);
    }
    ResumeThread(pi.hThread);
    WaitForSingleObject(hEvent, INFINITE);
    
    ResetEvent(hEvent);
    CloseHandle(hEvent);

    WCHAR dll2_path[MAX_PATH] = L"necola.dll";
    // inject necola
    InjectDLL(pi.dwProcessId, dll2_path);
    
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

    // ResumeThread(pi.hThread);
    // WaitForSingleObject(pi.hProcess, INFINITE);

    return 0;
}
