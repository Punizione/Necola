-- add_requires("microsoft-proxy 2.4.0")
add_rules("plugin.compile_commands.autoupdate")
add_rules("mode.debug", "mode.release")
set_policy("package.requires_lock", true)

set_languages("cxx20", "c++20")

set_plat("windows")
set_arch("x86")  -- Use "x64" for 64-bit builds

add_requires("spdlog")
add_requires("minhook")
add_requires("vcpkg::detours")
add_requires("vcpkg::inipp")

-- local name = "kpatch"
-- target(name)
--     set_kind("shared")
--     add_files("kpatch/dllmain.cpp")
--     add_packages("spdlog", "vcpkg::detours", "minhook", "vcpkg::inipp")
--     add_links("user32", "gdi32")

-- local name = "necola_s"
-- target(name)
--     set_kind("shared")
--     add_files("necola_s/dllmain.cpp")
--     add_files("necola_s/sdk/*.cpp")
--     add_files("necola_s/sdk/l4d2/*.cpp")
--     add_packages("spdlog", "vcpkg::detours", "minhook", "vcpkg::inipp")
--     add_links("user32", "gdi32")

local name = "necola"
target(name)
    set_kind("shared")
    add_files("necola/dllmain.cpp")
    add_files("necola/sdk/*.cpp")
    add_files("necola/hook/*.cpp")
    add_files("necola/sdk/l4d2/*.cpp")
    add_packages("spdlog", "vcpkg::detours", "minhook", "vcpkg::inipp")
    add_links("user32", "gdi32")

local name = "left4dead2_necola"
target(name)
    set_kind("binary")
    add_files("launcher_necola/main.cpp")
    add_files("assets/app.rc")
    add_links("user32", "gdi32")
    add_packages("vcpkg::detours")

-- local name = "left4dead2_fix"
-- target(name)
--     set_kind("binary")
--     add_files("launcher_with_fix/main.cpp")
--     add_files("assets/app.rc")
--     add_links("user32", "gdi32")
--     add_packages("vcpkg::detours")
