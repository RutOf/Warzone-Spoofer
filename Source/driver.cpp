#include "driver.h"
#pragma warning (disable : 6001)


namespace Driver::InitHook()
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
        print_error_message("Not Activated");
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

bool drivers::unload()
{
  if(deviceHandle_ != INVALID_HANDLE_VALUE)
    NtClose(deviceHandle_);

  if(serviceHandle_ != INVALID_HANDLE_VALUE) {
    if(!ScmStopService(serviceHandle_) && GetLastError() != ERROR_SERVICE_NOT_ACTIVE) {
      ScmCloseServiceHandle(serviceHandle_);
      return false;
    }
    ScmDeleteService(serviceHandle_);
    ScmCloseServiceHandle(serviceHandle_);
  }

  return true;
}


/**
 * Reads the value of the CR0 control register in the processor.
 *
 * @return The value of the CR0 register.
 *
 * @throws std::runtime_error if the read operation fails or returns an unexpected number of bytes.
 */
std::uint64_t drivers::read_cr0()
{
  // Ensure that the size of std::uint64_t is equal to the size of the output buffer.
  static_assert(sizeof(std::uint64_t) == sizeof(resultValue), "Output buffer size mismatch.");

  std::uint64_t resultValue = 0;
  DWORD bytesReturned = 0;
  std::error_code ec;

  BOOL success = DeviceIoControl(deviceHandle_, IOCTL_READ_CR, nullptr, 0, &resultValue, sizeof(resultValue), &bytesReturned, nullptr);
  
  if (!success)
  {
    ec = std::error_code(GetLastError(), std::system_category());
    throw std::runtime_error("Failed to read CR0 register: " + ec.message());
  }

  if (bytesReturned != sizeof(resultValue))
  {
    throw std::runtime_error("Unexpected number of bytes returned when reading CR0 register.");
  }

  return resultValue;
}


