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

NTSTATUS FindDriverObjectByName(const UNICODE_STRING* DriverName, PDRIVER_OBJECT* DriverObject)
{
    // Check for valid input parameters
    if (!DriverName || !DriverObject)
    {
        return STATUS_INVALID_PARAMETER;
    }

    // Open a handle to the object directory
    OBJECT_ATTRIBUTES obj_attrs;
    InitializeObjectAttributes(&obj_attrs, NULL, OBJ_KERNEL_HANDLE, NULL, NULL);
    HANDLE dir_handle;
    NTSTATUS status = ZwOpenDirectoryObject(&dir_handle, DIRECTORY_QUERY, &obj_attrs);
    if (!NT_SUCCESS(status))
    {
        return status;
    }

    // Find the object directory entry that contains the driver object with the specified name
    ULONG hash = HashUnicodeString(DriverName);
    POBJECT_DIRECTORY dir_obj = NULL;
    status = ObReferenceObjectByHandle(dir_handle, DIRECTORY_QUERY, *ObpDirectoryObjectType, KernelMode, (PVOID*)&dir_obj, NULL);
    if (!NT_SUCCESS(status))
    {
        ZwClose(dir_handle);
        return status;
    }
    
    // Acquire a shared lock on the directory object's hash table
    ExAcquirePushLockSharedEx(&dir_obj->Lock, 0);

    BOOLEAN success = FALSE;
    POBJECT_DIRECTORY_ENTRY entry = dir_obj->HashBuckets[hash];
    while (entry && entry->Object)
    {
        if (entry->Object->Type == IoDriverObjectType)
        {
            PDRIVER_OBJECT driver = (PDRIVER_OBJECT)entry->Object;
            if (RtlEqualUnicodeString(&driver->DriverName, DriverName, TRUE))
            {
                // Found the matching driver object
                *DriverObject = driver;
                ObReferenceObject(driver);
                success = TRUE;
                break;
            }
        }
        entry = entry->ChainLink;
    }

    ExReleasePushLock(&dir_obj->Lock);
    ObDereferenceObject(dir_obj);
    ZwClose(dir_handle);

    if (success)
    {
        return STATUS_SUCCESS;
    }
    else
    {
        *DriverObject = NULL;
        return STATUS_NOT_FOUND;
    }
}
