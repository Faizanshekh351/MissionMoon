@echo off
setlocal enabledelayedexpansion

echo ===================================================
echo Mission Moon Cinematic Showcase - Project Setup
echo ===================================================

:: Look for UnrealBuildTool or Epic Games install directory
set "UBT_PATH="
for %%D in (C D E) do (
    if exist "%%D:\Program Files\Epic Games\UE_5.4\Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe" (
        set "UBT_PATH=%%D:\Program Files\Epic Games\UE_5.4\Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe"
    )
    if exist "%%D:\Program Files\Epic Games\UE_5.5\Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe" (
        set "UBT_PATH=%%D:\Program Files\Epic Games\UE_5.5\Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe"
    )
)

if defined UBT_PATH (
    echo Found Unreal Engine Build Tool at: !UBT_PATH!
    echo Generating Visual Studio Solution files...
    "!UBT_PATH!" -projectfiles -project="%~dp0MissionMoon.uproject" -game -engine
    echo Done.
) else (
    echo [INFO] Unreal Engine standard path not auto-detected.
    echo Right-click 'MissionMoon.uproject' and select 'Generate Visual Studio project files'
    echo or double-click to launch directly in Unreal Engine 5.
)

pause
