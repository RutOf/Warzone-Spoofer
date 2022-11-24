@echo off
title HWID Checker by Tuga

@echo SMBIOS
@echo off 
wmic csproduct get uuid

@echo BIOS
@echo off 
wmic bios get serialnumber

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
