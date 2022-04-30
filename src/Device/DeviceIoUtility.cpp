#include "stdafx.h"
#include "DeviceIoUtility.h"

#include <winioctl.h>        // STORAGE_PROPERTY_QUERY
#include <ntddscsi.h>        // SCSI_PASS_THROUGH_DIRECT



#define SPT_SENSE_LENGTH (32)


typedef struct {
    SCSI_PASS_THROUGH_DIRECT sptd;
    ULONG filler;
    BYTE senseBuf[SPT_SENSE_LENGTH];
} SPTDWB;

typedef struct _SCSI_PASS_THROUGH_WITH_BUFFERS {
    SCSI_PASS_THROUGH	spt;
    ULONG				filler;
    BYTE				senseBuf[SPT_SENSE_LENGTH];
    UCHAR				dataBuf[1];
} SCSI_PASS_THROUGH_WITH_BUFFERS, * PSCSI_PASS_THROUGH_WITH_BUFFERS;


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

BYTE DeviceIoUtility::scsi_pass_through_direct(HANDLE handle, BYTE* cdb, BYTE* buffer, ULONG dataXferLen, BYTE direction, WORD timeout) {
    SPTDWB _scsi;
    SPTDWB* scsi = &_scsi;
    ZeroMemory(scsi, sizeof(SPTDWB));
    
    scsi->sptd.CdbLength = CDB_LEN;
    memcpy(scsi->sptd.Cdb, cdb, CDB_LEN);

    scsi->sptd.Length = sizeof(SCSI_PASS_THROUGH_DIRECT);
    scsi->sptd.SenseInfoOffset = offsetof(SPTDWB, senseBuf);
    scsi->sptd.SenseInfoLength = sizeof(scsi->senseBuf);
    scsi->sptd.TimeOutValue = timeout;

    scsi->sptd.DataIn = direction;
    scsi->sptd.DataTransferLength = dataXferLen;
    scsi->sptd.DataBuffer = buffer; //buffer ptr

    DWORD retVal = 0;
    DWORD objSize = sizeof(SPTDWB);
    BOOL status = DeviceIoControl(handle,  // device to be queried
        IOCTL_SCSI_PASS_THROUGH_DIRECT,  // operation to perform
        scsi,                            // in buffer 
        objSize,                         // in buffer size 
        scsi,                            // out buffer 
        objSize,                         // out buffer size
        &retVal,                  // # bytes returned
        (LPOVERLAPPED)NULL);             // synchronous I/O

    BYTE ScsiStatus = scsi->sptd.ScsiStatus;
    if ((ScsiStatus == 0) && (status == TRUE)) {
        return 0;
    }
    return ScsiStatus;
}

BYTE DeviceIoUtility::scsi_pass_through_with_buffer(HANDLE handle, BYTE* cdb, BYTE* buffer, ULONG dataXferLen, BYTE direction, WORD timeout) {
    ULONG packageLen = sizeof(SCSI_PASS_THROUGH) + sizeof(ULONG) + 32 + (dataXferLen);
    PSCSI_PASS_THROUGH_WITH_BUFFERS pSptwb = (PSCSI_PASS_THROUGH_WITH_BUFFERS)malloc(packageLen);
    ZeroMemory(pSptwb, packageLen);

    pSptwb->spt.CdbLength = CDB_LEN;
    memcpy(pSptwb->spt.Cdb, cdb, CDB_LEN);

    pSptwb->spt.Length = sizeof(SCSI_PASS_THROUGH);
    pSptwb->spt.SenseInfoOffset = offsetof(SCSI_PASS_THROUGH_WITH_BUFFERS, senseBuf);
    pSptwb->spt.SenseInfoLength = sizeof(pSptwb->senseBuf);
    pSptwb->spt.TimeOutValue = timeout;
    
    pSptwb->spt.DataIn = direction;
    pSptwb->spt.DataTransferLength = dataXferLen;
    pSptwb->spt.DataBufferOffset = offsetof(SCSI_PASS_THROUGH_WITH_BUFFERS, dataBuf);

    if (direction == SCSI_IOCTL_DATA_OUT) {
        memcpy(pSptwb->dataBuf, buffer, dataXferLen);
    }

    if (direction == SCSI_IOCTL_DATA_UNSPECIFIED) {
        pSptwb->spt.DataTransferLength = 0;
    }

    DWORD retVal = 0;
    BOOL status = DeviceIoControl(handle,
        IOCTL_SCSI_PASS_THROUGH,
        pSptwb,
        packageLen,
        pSptwb,
        packageLen,
        &retVal,
        NULL);

    if (!status || pSptwb->spt.ScsiStatus) {
        goto free;
    }

    if (pSptwb->spt.DataIn == SCSI_IOCTL_DATA_IN) {
        if (dataXferLen > 0) {
            memcpy(buffer, pSptwb->dataBuf, dataXferLen);
        }
    }
    
        
free:
    free(pSptwb);
    return status | pSptwb->spt.ScsiStatus;
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
