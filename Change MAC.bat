@echo off
title Windows-MAC-Address-Spoofer ^| v8.0

rem Check for admin privileges
if not "%PROCESSOR_ARCHITECTURE%"=="AMD64" if not "%PROCESSOR_ARCHITEW6432%"=="AMD64" (
    net session >nul 2>&1
    if %errorlevel% neq 0 (
        echo Administrator privileges are required.
        pause
        exit /b 1
    )
) else (
    if not "%USERNAME%"=="%USERDOMAIN%\%USERNAME%" (
        echo Administrator privileges are required.
        pause
        exit /b 1
    )
)

rem Enumerate available NICs
setlocal EnableDelayedExpansion
set "count=0"
echo Select NIC # to spoof:
for /f "skip=2 tokens=2 delims=," %%A in ('wmic nic get netconnectionid /format:csv') do (
    set /a "count+=1"
    set "nic[!count!]=%%A"
    echo   !count! - %%A
)

rem Get user selection
set /p "nic_selection=.  # "
set /a "nic_selection-=1"
if %nic_selection% lss 0 goto :INVALID_SELECTION
if %nic_selection% gtr %count% goto :INVALID_SELECTION
set "NetworkAdapter=!nic[%nic_selection%]!"

rem Get current MAC address of the selected NIC
for /f "tokens=2 delims=:" %%A in ('getmac ^| findstr "!NetworkAdapter!"') do set "MAC=%%A"

rem Generate new MAC address
set "new_MAC=00:00:00:00:00:00"
for /l %%A in (1,1,6) do call :RANDOM_BYTE

rem Update the MAC address of the selected NIC
echo Updating MAC address for NIC !NetworkAdapter!
echo Current MAC address: !MAC!
echo New MAC address: !new_MAC!
netsh interface set interface !NetworkAdapter! admin=disable
netsh interface set interface !NetworkAdapter! admin=enable
netsh interface set interface !NetworkAdapter! newmac=!new_MAC!

echo Operation completed. Press any key to continue...
pause
goto :EOF

:INVALID_SELECTION
echo Invalid selection.
goto :EOF

:RANDOM_BYTE
set /a "rand=%random% %% 255"
set "rand=00%rand%"
set "rand=!rand:~-2!"
set "new_MAC=!new_MAC:~0,2!-!rand!"

