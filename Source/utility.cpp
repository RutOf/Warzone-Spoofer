#include "utility.h"

OBJECT_ATTRIBUTES objAttribs = {};
NTSTATUS status = InitializeObjectAttributes(&objAttribs, &dirName, OBJ_CASE_INSENSITIVE, nullptr, nullptr);
if (!NT_SUCCESS(status))
{
    return status;
}

HANDLE dirHandle = {};
status = ZwOpenDirectoryObject(&dirHandle, DIRECTORY_ALL_ACCESS, &objAttribs);
if (!NT_SUCCESS(status))
{
    return status;
}

PVOID dir = nullptr;
status = ObReferenceObjectByHandle(dirHandle, DIRECTORY_ALL_ACCESS, nullptr, KernelMode, &dir, nullptr);
if (!NT_SUCCESS(status))
{
    ZwClose(dirHandle);
    return status;
}

ZwClose(dirHandle);


UNICODE_STRING objectName = {};
RtlInitUnicodeString(&objectName, DriverName->Buffer);

PVOID driverObject = nullptr;
status = ObOpenObjectByName(&objAttribs, dir, &objectName, 0, nullptr, KernelMode, &driverObject);
if (!NT_SUCCESS(status))
{
    ObDereferenceObject(dir);
    ZwClose(dirHandle);
    return status;
}

*DriverObject = (PDRIVER_OBJECT)driverObject;
ObDereferenceObject(dir);
ZwClose(dirHandle);
return status;
}

    auto dir_obj = static_cast<POBJECT_DIRECTORY>(dir);
    ExAcquirePushLockExclusiveEx(&dir_obj->Lock, 0);

    BOOLEAN success = FALSE;
    for (POBJECT_DIRECTORY_ENTRY entry : dir_obj->HashBuckets)
    {
        if (!entry)
            continue;

        while (entry && entry->Object)
        {
            auto driver = static_cast<PDRIVER_OBJECT>(entry->Object);
            if (RtlCompareUnicodeString(&driver->DriverName, DriverName, FALSE) == 0)
            {
                *DriverObject = driver;
                success = TRUE;
                break;
            }
            entry = entry->ChainLink;
        }
    }

    ExReleasePushLockExclusiveEx(&dir_obj->Lock, 0);
    ObDereferenceObject(dir);
    ZwClose(dir_handle);

    return success ? STATUS_SUCCESS : STATUS_NOT_FOUND;
}
