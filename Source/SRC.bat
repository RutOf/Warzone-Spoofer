@echo off

rem Display SMBIOS and BIOS serial number
echo SMBIOS:
wmic csproduct get uuid
echo BIOS:
wmic bios get serialnumber

rem Reset network adapters and settings
ipconfig /release
ipconfig /renew
ipconfig /flushdns
netsh int ip reset
netsh winsock reset

sc config Dhcp start=auto
sc config NlaSvc start=auto
sc config nsi start=auto
sc config RmSvc start=auto
sc config Wcmsvc start=auto
sc config Winmgmt start=auto

net start Dhcp
net start NlaSvc
net start nsi
net start RmSvc
net start Wcmsvc
net start Winmgmt



rem Disable netadapter with index number 0-5 (ipconfig /release)
wmic path win32_networkadapter where index=0 call disable
wmic path win32_networkadapter where index=1 call disable
wmic path win32_networkadapter where index=2 call disable
wmic path win32_networkadapter where index=3 call disable
wmic path win32_networkadapter where index=4 call disable
wmic path win32_networkadapter where index=5 call disable

rem Timeout to let the network adapter recover
timeout 6

rem Enable adapter with index number 0-5 (ipconfig /renew)
wmic path win32_networkadapter where index=0 call enable
wmic path win32_networkadapter where index=1 call enable
wmic path win32_networkadapter where index=2 call enable
wmic path win32_networkadapter where index=3 call enable
wmic path win32_networkadapter where index=4 call enable
wmic path win32_networkadapter where index=5 call enable

rem Reset winsock, adapters and firewall (ignore the errors)
arp -d *
route -f
nbtstat -R
nbtstat -RR
netsh advfirewall reset

netcfg -d
netsh winsock reset
netsh int 6to4 reset all
netsh int httpstunnel reset all
netsh int ip reset
netsh int isatap reset all
netsh int portproxy reset all
netsh int tcp reset all
netsh int teredo reset all
ipconfig /release
ipconfig /renew
ipconfig /flushdns

rem OPTIONAL: Force restart in 1 minute
rem shutdown /r /t 60

rem Take ownership of the following key in registry if you get "Access denied" message, then set Permissions for the current user to "Allow Full Control" or try to run the bat in safe mode
rem HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\Nsi\{eb004a00-9b1a-11d4-9123-0050047759bc}\26
rem Default entries are 10, 11, 12, 16, 18, 26, 30, 4, 6, 7



@echo MOTHERBOARD
@echo off 
wmic baseboard get serialnumber

@echo Chassis
@echo off
wmic systemenclosure get serialnumber

@echo CPU
@echo off 
wmic cpu get serialnumber

@echo HDD/SSD
@echo off 
wmic diskdrive get serialnumber

@echo VOLUME
@echo off 
vol
echo.

@echo RAM
@echo off 
wmic memorychip get serialnumber

@echo WINDOWS PRODUCT ID
@echo off 
wmic os get serialnumber

@echo MAC ADRESSES
@echo off 
getmac
echo.

cmd /k



---

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
rem Check if Call of Duty: Modern Warfare is installed
if exist "D:\Program Files (x86)\Call of Duty Modern Warfare" (
  rem Force close the Agent.exe process
  taskkill /IM Agent.exe /F /T

  rem Delete cache and configuration files
  del /f "D:\Program Files (x86)\Call of Duty Modern Warfare\main\data0.dcache"
  del /f "D:\Program Files (x86)\Call of Duty Modern Warfare\main\data1.dcache"
  del /f "D:\Program Files (x86)\Call of Duty Modern Warfare\main\toc0.dcache"
  del /f "D:\Program Files (x86)\Call of Duty Modern Warfare\main\toc1.dcache"
  del /f "D:\Program Files (x86)\Call of Duty Modern Warfare\Data\data\shmem"
  del /f "D:\Program Files (x86)\Call of Duty Modern Warfare\main\recipes\cmr_hist"
)

rem Check if Call of Duty: Black Ops Cold War is installed
if exist "D:\Program Files (x86)\Call of Duty Black Ops Cold War" (
  rem Force close the Battle.net.exe process
  taskkill /IM Battle.net.exe /F /T

  rem Delete cache and configuration files
  del /f "D:\Program Files (x86)\Call of Duty Black Ops Cold War\Data\data\shmen"
  del /f "D:\Program Files (x86)\Call of Duty Black Ops Cold War\telescopeCache\telescope_index.bat"

  rem Delete user-specific directories and files
  if exist "%USERPROFILE%\Documents\Call of Duty Black Ops Cold War" (
    rmdir /s /q "%USERPROFILE%\Documents\Call of Duty Black Ops Cold War\archive"
    rmdir /s /q "%USERPROFILE%\Documents\Call of Duty Black Ops Cold War\players"
    rmdir /s /q "%USERPROFILE%\Documents\Call of Duty Black Ops Cold War\report"
    rmdir /s /q "%USERPROFILE%\Documents\Call of Duty Black Ops Cold War"
  )
)

rem Check if Battle.net is installed
if exist "%LOCALAPPDATA%\Battle.net" (
  rem Delete Battle.net-specific directories and files
  rmdir /s /q "%LOCALAPPDATA%\Battle.net\Cache"
  rmdir /s /q "%LOCALAPPDATA%\Blizzard Entertainment"
  rmdir /s /q "%APPDATA%\Battle.net"
  rmdir /s /q "%PROGRAMDATA%\Battle.net"
  rmdir /s /q "%PROGRAMDATA%\Blizzard Entertainment"
  rmdir /s /q "%PROGRAMDATA%\Activision"

  rem Delete Battle.net-specific registry keys
  reg delete "HKCU\Software\Blizzard Entertainment\Battle.net" /f
  reg delete "HKLM\Software\WOW6432Node\Blizzard Entertainment" /f
  reg delete "HKEY_CURRENT_USER\Software\Blizzard Entertainment\Battle.
reg delete "HKEY_LOCAL_MACHINE\Software\Microsoft\Windows NT\CurrentVersion\InstallTime" /f
taskkill /IM Agent.exe /F
taskkill /IM Battle.net.exe /F
del /f ".\Data\data\shmem"
del /f ".\main\recipes\cmr_hist"
del /f ".\Battle.net\Cache\index"
del /f ".\Call of Duty Black Ops Cold War\telescopeCache\telescope_index.bat"
del "D:\Program Files (x86)\Call of Duty Black Ops Cold War\telescopeCache\telescope_index.bat"
del "D:\Program Files (x86)\Call of Duty Black Ops Cold War\Data\data\shmen"
del "C:\Users\%username%\Documents\Call of Duty Black Ops Cold War\archive"
del "C:\Users\%username%\Documents\Call of Duty Black Ops Cold War\players"
del "C:\Users\%username%\Documents\Call of Duty Black Ops Cold War\report"
del "C:\Users\%username%\Documents\Call of Duty Black Ops Cold War"
del "C:\Users\%username%\AppData\Local\Battle.net\Account\802259981"
del "C:\Users\%username%\AppData\Local\Battle.net\Account\802299369"
del "C:\Users\%username%\AppData\Local\Battle.net\Account\803619482"
del "C:\Users\%username%\AppData\Local\Battle.net\BrowserCache"
del "C:\Users\%username%\AppData\Local\Battle.net\BrowserCache\GPUCache"
del "C:\Users\%username%\AppData\Local\Battle.net\BrowserCache\GPUCache\data_0.dcache"
del "C:\Users\%username%\AppData\Local\Battle.net\BrowserCache\GPUCache\data_1.dcache"
del "C:\Users\%username%\AppData\Local\Battle.net\BrowserCache\GPUCache\data_2.dcache"
del "C:\Users\%username%\AppData\Local\Battle.net\BrowserCache\GPUCache\data_3.dcache"
del "C:\Users\%username%\AppData\Local\Battle.net\BrowserCache\GPUCache\f_000001.dcache"
del "C:\Users\%username%\AppData\Local\Battle.net\BrowserCache\GPUCache\index.dcache"
del "C:\Users\%username%\AppData\Local\Battle.net\BrowserCache\Cache\index"
del "C:\Users\%username%\AppData\Local\Battle.net\BrowserCache\GPUCache\data_0"
del "C:\Users\%username%\AppData\Local\Battle.net\BrowserCache\GPUCache\data_1"
del "C:\Users\%username%\AppData\Local\Battle.net\BrowserCache\GPUCache\data_2"
del "C:\Users\%username%\AppData\Local\Battle.net\BrowserCache\GPUCache\data_3"
del "C:\Users\%username%\AppData\Local\Battle.net\BrowserCache\GPUCache\f_000001"
del "C:\Users\%username%\AppData\Local\Battle.net\BrowserCache\GPUCache\index"
del "C:\Users\%username%\AppData\Local\Battle.net\BrowserCache\Cache\index.dcache"
del "C:\Users\%username%\AppData\Local\Battle.net\BrowserCache\Cache\data_0.dcache"
del "C:\Users\%username%\AppData\Local\Battle.net\BrowserCache\Cache\data_1.dcache"
del "C:\Users\%username%\AppData\Local\Battle.net\BrowserCache\Cache\data_2.dcache"
del "C:\Users\%username%\AppData\Local\Battle.net\BrowserCache\Cache\data_3.dcache"
del "C:\Users\%username%\AppData\Local\Battle.net\BrowserCache\Cache\data_0"
del "C:\Users\%username%\AppData\Local\Battle.net\BrowserCache\Cache\data_1"
del "C:\Users\%username%\AppData\Local\Battle.net\BrowserCache\Cache\data_2"
del "C:\Users\%username%\AppData\Local\Battle.net\BrowserCache\Cache\data_3"
del "C:\Users\%username%\AppData\Local\Battle.net\Cache"
del "C:\Users\%username%\AppData\Local\Battle.net\Logs"
del "C:\Users\%username%\AppData\Local\Battle.net\WidevineCdm"
del "C:\Users\%username%\AppData\Local\Battle.net\CachedData"
del "C:\Users\%username%\AppData\Local\Blizzard Entertainment"
rundll32 user32.dll,MessageBeep
cls
echo Finished!
timeout 4

---

