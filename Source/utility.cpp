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

NTSTATUS GetDriverObjectByName(const UNICODE_STRING* DriverName, PDRIVER_OBJECT* DriverObject)
{
    // Check input parameters
    if (!DriverName || !DriverName->Buffer || !DriverObject)
    {
        return STATUS_INVALID_PARAMETER;
    }

    NTSTATUS status = STATUS_SUCCESS;
    HANDLE dirHandle = nullptr;
    OBJECT_ATTRIBUTES objAttribs = {};
    UNICODE_STRING dirPath = RTL_CONSTANT_STRING(L"\\Driver");

    InitializeObjectAttributes(&objAttribs, &dirPath, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, nullptr, nullptr);

    // Open the driver directory
    IO_STATUS_BLOCK ioStatus = {};
    status = ZwCreateFile(&dirHandle, FILE_GENERIC_READ, &objAttribs, &ioStatus, nullptr, FILE_ATTRIBUTE_DIRECTORY, 0, FILE_OPEN, FILE_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT, nullptr, 0);
    if (!NT_SUCCESS(status))
    {
        return status;
    }

    // Enumerate the directory to find the target driver object
    BYTE buffer[sizeof(FILE_DIRECTORY_INFORMATION) + MAX_PATH * sizeof(WCHAR)];
    PFILE_DIRECTORY_INFORMATION dirInfo = reinterpret_cast<PFILE_DIRECTORY_INFORMATION>(buffer);
    ULONG context = 0;
    BOOLEAN found = FALSE;

    while (TRUE)
    {
        status = ZwQueryDirectoryFile(dirHandle, nullptr, nullptr, nullptr, &ioStatus, dirInfo, sizeof(buffer), FileDirectoryInformation, TRUE, nullptr, FALSE);
        if (!NT_SUCCESS(status))
        {
            break;
        }

        if (RtlCompareUnicodeString(DriverName, &dirInfo->FileName, TRUE) == 0)
        {
            found = TRUE;
            break;
        }

        if (dirInfo->NextEntryOffset == 0)
        {
            break;
        }

        dirInfo = reinterpret_cast<PFILE_DIRECTORY_INFORMATION>(reinterpret_cast<PUCHAR>(dirInfo) + dirInfo->NextEntryOffset);
    }

    if (!found)
    {
        status = STATUS_NOT_FOUND;
        goto Cleanup;
    }

    // Open the driver object
    UNICODE_STRING objectName = {};
    RtlInitUnicodeString(&objectName, dirInfo->FileName.Buffer);

    PVOID driverObject = nullptr;
    status = ObOpenObjectByName(&objAttribs, nullptr, &objectName, nullptr, nullptr, OBJ_KERNEL_HANDLE, nullptr, &driverObject);
    if (!NT_SUCCESS(status))
    {
        goto Cleanup;
    }

    *DriverObject = static_cast<PDRIVER_OBJECT>(driverObject);

Cleanup:
    if (dirHandle)
    {
        ZwClose(dirHandle);
    }

    return status;
}


// Find a driver object by name in the Windows kernel's object directory
NTSTATUS FindDriverObjectByName(const UNICODE_STRING* DriverName, PDRIVER_OBJECT* DriverObject)
{
    // Check for valid input parameters
    if (!DriverName || !DriverObject)
    {
        return STATUS_INVALID_PARAMETER;
    }

    // Open a handle to the object directory
    HANDLE dir_handle;
    OBJECT_ATTRIBUTES obj_attrs = RTL_CONSTANT_OBJECT_ATTRIBUTES(&g_DosDevicesDirectory, OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE);
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

    // Search for the driver object by name
    BOOLEAN success = FALSE;
    POBJECT_DIRECTORY_ENTRY entry = dir_obj->HashBuckets[hash];
    while (entry && entry->Object)
    {
        if (entry->Object->Type == IO_TYPE_DRIVER)
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

    // Release the lock on the directory object's hash table
    ExReleasePushLock(&dir_obj->Lock);

    // Dereference the object directory and close the handle
    ObDereferenceObject(dir_obj);
    ZwClose(dir_handle);

    // Return the appropriate status code
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

