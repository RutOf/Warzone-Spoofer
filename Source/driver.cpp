#include "driver.h"
#pragma warning (disable : 6001)

NTSTATUS driver::driver_entry()
{
    print_success_message("driver loaded");
    init_hook();

    return STATUS_SUCCESS;
}

NTSTATUS Driver::InitHook()
{
    NTSTATUS status = STATUS_SUCCESS;
    UNICODE_STRING driverName = {};
    RtlInitUnicodeString(&driverName, L"\\Driver\\disk");
    PDRIVER_OBJECT pDiskDriverObject = nullptr;

    status = Utility::FindDriverObject(&pDiskDriverObject, &driverName);
    if (!NT_SUCCESS(status) || !pDiskDriverObject)
    {
        print_error_message("Could not find disk driver object. NTSTATUS: 0x%08X", status);
        return STATUS_UNSUCCESSFUL;
    }

    print_success_message("Found disk driver object: %p", pDiskDriverObject);

    HookHandler::OriginalDispatch = pDiskDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL];
    pDiskDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = HookHandler::HookedDeviceControl;

    print_success_message("Stored original dispatch: %p", HookHandler::OriginalDispatch);
    print_success_message("Swapped device control dispatch: %p -> %p", HookHandler::OriginalDispatch, HookHandler::HookedDeviceControl);

    return STATUS_SUCCESS;
}
