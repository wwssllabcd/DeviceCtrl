#pragma once

#include "HeadBase.h"
#include <windows.h>         // HANDLE
#include <vector>

#define CDB_LEN (12)

using namespace std;

class DeviceIoUtility
{
public:
    DeviceIoUtility();
    ~DeviceIoUtility();

    BYTE scsi_pass_through_direct(HANDLE handle, BYTE* cdb, BYTE* buffer, ULONG bufferlen, BYTE direction, WORD timeout);
    BYTE scsi_pass_through_with_buffer(HANDLE handle, BYTE* cdb, BYTE* buffer, ULONG dataXferLen, BYTE direction, WORD timeout);
    eu32 get_bus_type(HANDLE handle);
    HANDLE open_device(estring devicePath);
    void close_device(HANDLE handle);


    vector<estring> gen_string_physical_drive();
    vector<estring> gen_string_drive_letter();
};

