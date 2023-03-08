#include "hook_handler.h"

PDRIVER_DISPATCH hook_handler::original_dispatch = nullptr;

NTSTATUS DiskCompletionRoutine(PDEVICE_OBJECT DeviceObject, PIRP Irp, PIO_COMPLETION_CONTEXT Context)
{
    UNREFERENCED_PARAMETER(Irp);
    UNREFERENCED_PARAMETER(DeviceObject);

    if (Context->RequestBuffer->SerialNumberOffset > 0 && Context->RequestBuffer->SerialNumberOffset < Context->OutBufferLength)
    {
        char* CurrentSerialNumber = ((char*)Context->RequestBuffer) + Context->RequestBuffer->SerialNumberOffset;
        char SpoofedSerial[] = "zpes-was-here";

        memcpy(CurrentSerialNumber, SpoofedSerial, sizeof(SpoofedSerial));
    }

    return STATUS_SUCCESS;
}

typedef struct _IO_COMPLETION_STRUCT {
    PIO_COMPLETION_ROUTINE oldCompletionRoutine; // the original completion routine
    PVOID oldContext; // the original context
    ULONG signature; // a signature to validate the context
    PSTORAGE_DEVICE_DESCRIPTOR requestBuffer; // pointer to the request buffer
    ULONG OutBufferLength; // output buffer length
} IO_COMPLETION_STRUCT, *PIO_COMPLETION_STRUCT;

// Define the hooked device control function
NTSTATUS hook_handler::hooked_device_control(PDEVICE_OBJECT pDeviceObject, PIRP Irp)
{
    // Get the current I/O stack location
    PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);

    // If the control code is not IOCTL_STORAGE_QUERY_PROPERTY, return control to the original driver function
    if (stack->Parameters.DeviceIoControl.IoControlCode != IOCTL_STORAGE_QUERY_PROPERTY)
    {
        return hook_handler::original_dispatch(pDeviceObject, Irp);
    }

    // Get the storage property query request buffer
    PSTORAGE_PROPERTY_QUERY requestBuffer = (PSTORAGE_PROPERTY_QUERY)Irp->AssociatedIrp.SystemBuffer;

    // If the property ID is not StorageDeviceProperty, return control to the original driver function
    if (requestBuffer->PropertyId != StorageDeviceProperty)
    {
        return hook_handler::original_dispatch(pDeviceObject, Irp);
    }

    // Allocate memory for the spoofed context
    PIO_COMPLETION_STRUCT spoofed_context = (PIO_COMPLETION_STRUCT)ExAllocatePoolWithTag(NonPagedPool, sizeof(IO_COMPLETION_STRUCT), 'hook');

    if (spoofed_context == NULL)
    {
        // If memory allocation fails, return an error code
        Irp->IoStatus.Status = STATUS_INSUFFICIENT_RESOURCES;
        IoCompleteRequest(Irp, IO_NO_INCREMENT);
        return STATUS_INSUFFICIENT_RESOURCES;
    }
    
    // Modify the IRP completion routine
    stack->CompletionRoutine = disk_completion_routine;

    // Set the IRP context to point to the spoofed context
    stack->Context = spoofed_context;

    // Set the IRP control flags to invoke the completion routine only on success
    stack->Control |= SL_INVOKE_ON_SUCCESS;

    // Pass the IRP on to the original driver function
    return hook_handler::original_dispatch(pDeviceObject, Irp);
}

