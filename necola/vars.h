#pragma once
#define SPDLOG_WCHAR_TO_UTF8_SUPPORT
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

#include <windows.h>
#include <string>
#include <cstdlib>
#include <filesystem>

#include <inipp.h>

// Version
std::string sFixName = "L4D2-Necola";
std::string sLogFile = sFixName + ".log";
std::string sFixVer = "1.2beta";

// Logger
std::shared_ptr<spdlog::logger> logger;

// Ini Config
inipp::Ini<char> ini;

// Global config vars
namespace cfg {
namespace  System {
    bool debug;
    std::wstring target;
    std::wstring cmdLine;

}

}

void LoadIni() {
    using namespace cfg;

    std::ifstream is("kpatch.ini");
    ini.parse(is);
    {
        using namespace System;
        inipp::extract(ini.sections["System"]["debug"], debug);

        std::string tmp_name;
        inipp::extract(ini.sections["System"]["target"], tmp_name);
        target = std::wstring(tmp_name.begin(), tmp_name.end());

        std::string tmp_cmdline;
        inipp::extract(ini.sections["System"]["cmdline"], tmp_cmdline);
        cmdLine = std::wstring(tmp_cmdline.begin(), tmp_cmdline.end());
    }
}
