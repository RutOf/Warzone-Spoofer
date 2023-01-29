#include "utility.h"

NTSTATUS find_driver_object(PDRIVER_OBJECT* DriverObject, PUNICODE_STRING DriverName)
{
    UNICODE_STRING dir_name = {};
    RtlInitUnicodeString(&dir_name, L"\\Driver");

    OBJECT_ATTRIBUTES attrib = {};
    InitializeObjectAttributes(&attrib, &dir_name, OBJ_CASE_INSENSITIVE, nullptr, nullptr);

    HANDLE dir_handle = {};
    NTSTATUS status = ZwOpenDirectoryObject(&dir_handle, DIRECTORY_ALL_ACCESS, &attrib);
    if (!NT_SUCCESS(status))
        return status;

    PVOID dir = {};
    status = ObReferenceObjectByHandle(dir_handle, DIRECTORY_ALL_ACCESS, nullptr, KernelMode, &dir, nullptr);
    if (!NT_SUCCESS(status))
    {
        ZwClose(dir_handle);
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
