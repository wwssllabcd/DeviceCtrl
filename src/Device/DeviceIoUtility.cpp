#include "stdafx.h"
#include "DeviceIoUtility.h"

#include <winioctl.h>        // STORAGE_PROPERTY_QUERY
#include <ntddscsi.h>        // SCSI_PASS_THROUGH_DIRECT



#define SPT_SENSE_LENGTH (32)


typedef struct {
    SCSI_PASS_THROUGH_DIRECT sptd;
    //WORD filler;
    BYTE sensebytes[SPT_SENSE_LENGTH];
} SPTDWB;


DeviceIoUtility::DeviceIoUtility() {
}


DeviceIoUtility::~DeviceIoUtility() {
}

HANDLE DeviceIoUtility::open_device(estring devicePath) {
    HANDLE handle = CreateFile(
        devicePath.c_str(),
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);
    return handle;
}

void DeviceIoUtility::close_device(HANDLE handle) {
    CloseHandle(handle);
}

eu32 DeviceIoUtility::get_bus_type(HANDLE handle) {
    STORAGE_PROPERTY_QUERY Query;
    STORAGE_DEVICE_DESCRIPTOR DevDesc;
    DWORD dwOutBytes;

    Query.PropertyId = StorageDeviceProperty;
    Query.QueryType = PropertyStandardQuery;

    BOOL result = ::DeviceIoControl(handle,		// device handle
        IOCTL_STORAGE_QUERY_PROPERTY,		    // info of device property
        &Query,
        sizeof(STORAGE_PROPERTY_QUERY),		    // input data buffer
        &DevDesc,
        sizeof(STORAGE_DEVICE_DESCRIPTOR),	    // output data buffer
        &dwOutBytes,						    // out's length
        (LPOVERLAPPED)NULL);

    return DevDesc.BusType;
}

BYTE DeviceIoUtility::scsi_pass_through_direct(HANDLE handle, BYTE cdb[16], BYTE* buffer, ULONG dataXferLen, BYTE direction, WORD timeout) {
    SPTDWB _scsi = { 0 };
    SPTDWB* scsi = &_scsi;

    scsi->sptd.Length = sizeof(SCSI_PASS_THROUGH_DIRECT);
    scsi->sptd.SenseInfoOffset = offsetof(SPTDWB, sensebytes);
    scsi->sptd.SenseInfoLength = SPT_SENSE_LENGTH;

    scsi->sptd.ScsiStatus = 0;
    scsi->sptd.PathId = 0;
    scsi->sptd.TargetId = 0;
    scsi->sptd.Lun = 0;

    scsi->sptd.DataBuffer = buffer; //buffer ptr
    scsi->sptd.DataTransferLength = dataXferLen;

    scsi->sptd.TimeOutValue = timeout;
    scsi->sptd.DataIn = direction;

    scsi->sptd.CdbLength = 16;
    memcpy(scsi->sptd.Cdb, cdb, 16);

    DWORD bytesReturned = 0; // data returned
    DWORD objSize = sizeof(SPTDWB);
    BOOL status = DeviceIoControl(handle,  // device to be queried
        IOCTL_SCSI_PASS_THROUGH_DIRECT,  // operation to perform
        scsi,                            // in buffer 
        objSize,                         // in buffer size 
        scsi,                            // out buffer 
        objSize,                         // out buffer size
        &bytesReturned,                  // # bytes returned
        (LPOVERLAPPED)NULL);             // synchronous I/O

    BYTE ScsiStatus = scsi->sptd.ScsiStatus;
    if ((ScsiStatus == 0) && (status == TRUE)) {
        return 0;
    }
    return ScsiStatus;
}

vector<estring> DeviceIoUtility::gen_string_physical_drive() {
    vector<estring> devicePaths;
    for (eu32 i = 0; i < 32; i++) {
        estring fullName = _ET("\\\\.\\PhysicalDrive") + to_tstring(i);
        devicePaths.push_back(fullName);
    }
    return devicePaths;
}

vector<estring> DeviceIoUtility::gen_string_drive_letter() {
    vector<estring> devicePaths;
    for (echar drive = _ET('A'); drive < _ET('Z'); drive++) {
        estring driverLetter(1, drive);
        estring fullName = _ET("\\\\.\\") + driverLetter + _ET(':');
        devicePaths.push_back(fullName);
    }
    return devicePaths;
}
