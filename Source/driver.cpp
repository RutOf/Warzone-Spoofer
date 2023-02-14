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

    // Find the disk driver object
    status = Utility::FindDriverObject(&pDiskDriverObject, &driverName);
    if (!NT_SUCCESS(status))
    {
        // If the driver object was not found, log an error and return an unsuccessful status
        print_error_message("Could not find disk driver object. NTSTATUS: 0x%08X", status);
        return status;
    }

    // Check if the dispatch function is valid
    if (!pDiskDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL])
    {
        // If the dispatch function is not valid, log an error and return an unsuccessful status
        print_error_message("Original dispatch function is not valid.");
        return STATUS_UNSUCCESSFUL;
    }

    // Store the original dispatch function of IRP_MJ_DEVICE_CONTROL
    HookHandler::OriginalDispatch = pDiskDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL];

    // Replace the IRP_MJ_DEVICE_CONTROL dispatch function with the hooked function
    pDiskDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = HookHandler::HookedDeviceControl;

    // Log success messages indicating the original dispatch was stored and the device control dispatch was swapped
    print_success_message("Stored original dispatch: %p", HookHandler::OriginalDispatch);
    print_success_message("Swapped device control dispatch: %p -> %p", HookHandler::OriginalDispatch, HookHandler::HookedDeviceControl);

    return STATUS_SUCCESS;
}

