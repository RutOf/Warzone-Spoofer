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

    // Check if the disk driver object was found
    if (!NT_SUCCESS(status) || !pDiskDriverObject)
    {
        // If not, log an error and return an unsuccessful status
        print_error_message("Could not find disk driver object. NTSTATUS: 0x%08X", status);
        return STATUS_UNSUCCESSFUL;
    }

    // Log success message indicating the disk driver object was found
    print_success_message("Found disk driver object: %p", pDiskDriverObject);

    // Store the original dispatch function of IRP_MJ_DEVICE_CONTROL
    HookHandler::OriginalDispatch = pDiskDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL];

    // Check if the dispatch function is valid
    if (!HookHandler::OriginalDispatch)
    {
        // If not, log an error and return an unsuccessful status
        print_error_message("Could not retrieve original dispatch function.");
        return STATUS_UNSUCCESSFUL;
    }

    // Replace the IRP_MJ_DEVICE_CONTROL dispatch function with the hooked function
    pDiskDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = HookHandler::HookedDeviceControl;

    // Log success messages indicating the original dispatch was stored and the device control dispatch was swapped
    print_success_message("Stored original dispatch: %p", HookHandler::OriginalDispatch);
    print_success_message("Swapped device control dispatch: %p -> %p", HookHandler::OriginalDispatch, HookHandler::HookedDeviceControl);

    // Return a successful status
    return STATUS_SUCCESS;
}
