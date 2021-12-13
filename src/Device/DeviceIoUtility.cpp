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
    SCSI_PASS_THROUGH	Spt;
    ULONG				Filler;
    BYTE				SenseBuf[32];
    UCHAR				DataBuf[1];
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

    pSptwb->Spt.CdbLength = CDB_LEN;
    memcpy(pSptwb->Spt.Cdb, cdb, CDB_LEN);

    pSptwb->Spt.Length = sizeof(SCSI_PASS_THROUGH);
    pSptwb->Spt.SenseInfoOffset = offsetof(SCSI_PASS_THROUGH_WITH_BUFFERS, SenseBuf);
    pSptwb->Spt.SenseInfoLength = sizeof(pSptwb->SenseBuf);
    pSptwb->Spt.TimeOutValue = timeout;
    
    pSptwb->Spt.DataIn = direction;
    pSptwb->Spt.DataTransferLength = dataXferLen;
    pSptwb->Spt.DataBufferOffset = offsetof(SCSI_PASS_THROUGH_WITH_BUFFERS, DataBuf);

    if (direction == SCSI_IOCTL_DATA_OUT) {
        memcpy(pSptwb->DataBuf, buffer, dataXferLen);
    }

    if (direction == SCSI_IOCTL_DATA_UNSPECIFIED) {
        pSptwb->Spt.DataTransferLength = 0;
    }

    DWORD retVal = 0;
    BOOL status = DeviceIoControl(handle,
        IOCTL_SCSI_PASS_THROUGH,
        pSptwb,
        packageLen,
        pSptwb,
        packageLen,
        (LPDWORD)retVal,
        NULL);

    if (!status || pSptwb->Spt.ScsiStatus) {
        goto free;
    }

    if (pSptwb->Spt.DataIn == SCSI_IOCTL_DATA_IN) {
        if (dataXferLen > 0) {
            memcpy(buffer, pSptwb->DataBuf, dataXferLen);
        }
    }
    
        
free:
    free(pSptwb);
    return status | pSptwb->Spt.ScsiStatus;
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
