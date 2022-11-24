@echo -off

AMIDEEFIx64.EFI /su [random UUID number (you can find one running the "AMIDEWINx64.EXE /SU AUTO" command inside the HWID folder)]
AMIDEEFIx64.EFI /bs [random baseboard serial number, 15 random digits]

exit