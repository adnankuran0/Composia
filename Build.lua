-- premake5.lua
workspace "Composia"
    architecture "x64"
    configurations { "Debug", "Release", "Dist" }
    startproject "App"

    filter "system:windows"
        buildoptions {"/EHsc", "/Zc:preprocessor", "/Zc:__cplusplus"} 

OutputDir = "%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}"

group "Composia"
    include "Composia/Build-Composia.lua"
group ""

include "App/Build-App.lua"