@echo off
title Tuga's COD Tracer Cleaner

:: BatchGotAdmin
:-------------------------------------

REM --> Check for permissions
IF "%PROCESSOR_ARCHITECTURE%" EQU "amd64" (
    >nul 2>&1 "%SYSTEMROOT%\SysWOW64\cacls.exe" "%SYSTEMROOT%\SysWOW64\config\system"
) ELSE (
    >nul 2>&1 "%SYSTEMROOT%\system32\cacls.exe" "%SYSTEMROOT%\system32\config\system"
)

REM --> If error flag set, we do not have admin.
if '%errorlevel%' NEQ '0' (
    echo Requesting administrative privileges...
    goto UACPrompt
) else ( goto gotAdmin )

:UACPrompt
echo Set UAC = CreateObject^("Shell.Application"^) > "%temp%\getadmin.vbs"
set params= %*
echo UAC.ShellExecute "cmd.exe", "/c ""%~s0"" %params:"=""%", "", "runas", 1 >> "%temp%\getadmin.vbs"

"%temp%\getadmin.vbs"
del "%temp%\getadmin.vbs"
exit /B

:gotAdmin
pushd "%CD%"
CD /D "%~dp0"
:--------------------------------------
cls
echo Cleaning...
timout 2
cls

REM --> Kill processes that may interfere with the deletion of files
taskkill /IM Agent.exe /F
taskkill /IM Battle.net.exe /F
"%temp%\getadmin.vbs"
del "%temp%\getadmin.vbs"
exit /B

@echo off

rem Change to the current directory
pushd "%CD%"
CD /D "%~dp0"

cls
echo Cleaning...
timeout 2
cls

taskkill /IM "Agent.exe" /F /T 2>nul
taskkill /IM "Battle.net.exe" /F /T 2>nul
taskkill /IM "Discord.exe" /F /T 2>nul
taskkill /IM "Steam.exe" /F /T 2>nul


rem Delete various files and directories
del "D:\Program Files (x86)\Call of Duty Modern Warfare\main\data0.dcache" 2>nul
del "D:\Program Files (x86)\Call of Duty Modern Warfare\main\data1.dcache" 2>nul
del "D:\Program Files (x86)\Call of Duty Modern Warfare\main\toc0.dcache" 2>nul
del "D:\Program Files (x86)\Call of Duty Modern Warfare\main\toc1.dcache" 2>nul
del "D:\Program Files (x86)\Call of Duty Modern Warfare\Data\data\shmem" 2>nul
del "D:\Program Files (x86)\Call of Duty Modern Warfare\main\recipes\cmr_hist" 2>nul
del /f ".\Data\data\shmem" 2>nul
del /f ".\main\recipes\cmr_hist" 2>nul
rmdir ".\main\recipes\cmr_hist" /s /q 2>nul
rmdir "%userprofile%\documents\Call of Duty Modern Warfare" /s /q 2>nul
rmdir "%localappdata%\Battle.net" /s /q 2>nul
rmdir "%localappdata%\Blizzard Entertainment" /s /q 2>nul
rmdir "%appdata%\Battle.net" /s /q 2>nul
rmdir "%programdata%\Battle.net" /s /q 2>nul
rmdir "%programdata%\Blizzard Entertainment" /s /q 2>nul
rmdir "%programdata%\Activision" /s /q 2>nul
rmdir "%Documents%\Call Of Duty Modern Warfare" /s /q 2>nul
rmdir "%Documents%\Call Of Duty Black Ops Cold War" /s /q 2>nul

rem Delete various registry keys
reg delete "HKCU\Software\Blizzard Entertainment\Battle.net" /f 2>nul
reg delete "HKEY_CURRENT_USER\Software\Blizzard Entertainment" /f 2>nul
reg delete "HKEY_LOCAL_MACHINE\SOFTWARE\WOW6432Node\Blizzard Entertainment" /f 2>nul
reg delete "HKLM\Software\WOW6432Node\Blizzard Entertainment" /f 2>nul
reg delete "HKEY_CURRENT_USER\Software\Blizzard Entertainment\Battle.net\Identity" /f 2>nul
reg delete "HKEY_LOCAL_MACHINE\Software\Microsoft\Windows NT\CurrentVersion\InstallTime" /f 2>nul

rem Kill any running instances of Agent.exe or Battle.net.exe
taskkill /IM Agent.exe /F 2>nul
taskkill /IM
