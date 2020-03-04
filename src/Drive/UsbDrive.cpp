//Copyright © 2006~ EricWang(wwssllabcd@gmail.com). All rights reserved

#include "stdafx.h"
#include "UsbDrive.h"
#include "DefineFiles\DefineFile.h"

#include <algorithm>
#include <winioctl.h>  
#include <setupapi.h>             // SP_DEVICE_INTERFACE_DATA
#pragma comment(lib,"setupapi")   // import setupapi.lib

#include "ddk\Wnaspi32.h"
#include "ddk\ntddscsi.h"
#include "ddk\spti.h"

using namespace EricCore;

DEFINE_GUID(GUID_DISK_CLASS, 0x53f56307, 0xb6bf, 0x11d0, 0x94, 0xf2, 0x00, 0xa0, 0xc9, 0x1e, 0xfb, 0x8b);

UsbDrive::UsbDrive() {
}

UsbDrive::~UsbDrive(void) {
}

bool UsbDrive::SendSptiCommand(HANDLE drive_handle, eu8* cdb, eu8* iobuf, DWORD buflen, eu8 flags, eu16 timeout) {
	BOOL status = 0;
	SCSI_PASS_THROUGH_WITH_BUFFERS *Sptwb;
	eu32 length = 0, bytes_returned = 0;
	eu32 input_buffer_length, output_buffer_length;

	Sptwb = (SCSI_PASS_THROUGH_WITH_BUFFERS *)malloc(sizeof(SCSI_PASS_THROUGH_WITH_BUFFERS) + _128K);
	if (Sptwb == NULL) {
		return false;
	}

	ZeroMemory(Sptwb, sizeof(SCSI_PASS_THROUGH_WITH_BUFFERS));

	if (buflen) {
		if (!(flags&SCSI_IOCTL_DATA_IN)) {
			memcpy((PUCHAR)&Sptwb->ucDataBuf[0], (PUCHAR)iobuf, buflen);
		} else if (buflen < 512) {
			ZeroMemory(Sptwb->ucDataBuf, buflen);
		}
	}

	Sptwb->spt.Length = sizeof(SCSI_PASS_THROUGH);
	Sptwb->spt.PathId = 0;
	Sptwb->spt.TargetId = 0;
	Sptwb->spt.Lun = 0;
	Sptwb->spt.CdbLength = 12;
	Sptwb->spt.SenseInfoLength = 0;
	Sptwb->spt.DataIn = flags;

	Sptwb->spt.DataTransferLength = (buflen == 1) ? 2 : buflen;
	Sptwb->spt.TimeOutValue = timeout;
	Sptwb->spt.DataBufferOffset = offsetof(SCSI_PASS_THROUGH_WITH_BUFFERS, ucDataBuf);
	Sptwb->spt.SenseInfoOffset = NULL;
	memcpy(Sptwb->spt.Cdb, cdb, 12);

	//for read 16
	if (cdb[0] == 0x88) {
		memcpy(Sptwb->spt.Cdb, cdb, 16);
		Sptwb->spt.CdbLength = 16;
	}


	if (buflen) {
		if (flags & SCSI_IOCTL_DATA_IN) {
			input_buffer_length = sizeof(SCSI_PASS_THROUGH);
			output_buffer_length = offsetof(SCSI_PASS_THROUGH_WITH_BUFFERS, ucDataBuf) +
				Sptwb->spt.DataTransferLength;
		} else {
			input_buffer_length = offsetof(SCSI_PASS_THROUGH_WITH_BUFFERS, ucDataBuf) +
				Sptwb->spt.DataTransferLength;
			output_buffer_length = sizeof(SCSI_PASS_THROUGH);
		}
	} else {
		input_buffer_length = output_buffer_length = sizeof(SCSI_PASS_THROUGH);
	}

	status = DeviceIoControl(drive_handle, //    HANDLE  設備句柄：是從CreateFile返回的VxD句柄。
		IOCTL_SCSI_PASS_THROUGH, //DWORD  控制碼：是用來制定VxD將要進行的操作。你應該在你要選用那種操作之前得到可能的dwIoControlCode值得列表。
		Sptwb,// 輸入數據緩衝區指針：是包含了VxD完成dwIoControlCode所制定操作的數據的緩衝區地址。如果這個操作不需要數據，你可以傳為NULL。
		input_buffer_length,// 輸入數據緩衝區長度：是由lpInBuffer所指向的緩衝區的地址的大小（byte）。
		Sptwb,// 輸出數據緩衝區指針：是VxD程序在操作成功之後要將輸出數據輸出到的緩衝區。如果這個操作沒有任何返回值，這個值可以為NULL。
		output_buffer_length,// 是lpOutBuffer所指向的緩衝區的大小（byte）。
		&bytes_returned,// 是一個dword型變量的地址。這個變量用來接收VxD在lpOutBuffer中寫入數據的大小。
		FALSE);// 重疊操作結構指針：lpOverlapped是一個OVERLAPPED結構的指針。如果你要一直等直到操作完成，這個值為NULL。

	if ((Sptwb->spt.ScsiStatus == 0) && (status != 0)) {

		if (buflen && (flags & SCSI_IOCTL_DATA_IN)) { //data in						 
			memcpy((PUCHAR)iobuf, (PUCHAR)&Sptwb->ucDataBuf[0], buflen);
		}
		free(Sptwb);
		return true;
	} else {
		free(Sptwb);
		return false;
	}
}

#define	FLAG_DATA_IN			0x00000002
#define	FLAG_DATA_OUT			0x00000004
bool UsbDrive::sendCommand(HANDLE hUsb, PUCHAR cdb, PUCHAR DataBuffer, eu32  DataTransferLen, UCHAR  fDirection) {
	UCHAR flags = 0;
	if (DataTransferLen == 0) {
		fDirection = 0;
	}

	if (fDirection & FLAG_DATA_IN) {
		flags |= SCSI_IOCTL_DATA_IN;
	}

	if (fDirection & FLAG_DATA_OUT) {
		flags |= SCSI_IOCTL_DATA_OUT;
	}
	return  SendSptiCommand(hUsb, cdb, DataBuffer, DataTransferLen, flags, 30);
}

bool UsbDrive::GetDeviceViaInterface(GUID* pGuid, eu32 instance, echar_p DevicePath) {
	DWORD ReqLen;
	HDEVINFO info;

	bool blReturn = false;
	info = SetupDiGetClassDevs(pGuid, NULL, NULL, DIGCF_PRESENT | DIGCF_INTERFACEDEVICE);
	if (info == INVALID_HANDLE_VALUE) {
		return blReturn;
	}

	// Get interface data for the requested instance  
	SP_DEVICE_INTERFACE_DATA ifdata;
	ifdata.cbSize = sizeof(ifdata);
	if (!SetupDiEnumDeviceInterfaces(info, NULL, pGuid, instance, &ifdata)) {
		SetupDiDestroyDeviceInfoList(info);
		return blReturn;
	}

	// Get size of symbolic link name                                                                
	SetupDiGetDeviceInterfaceDetail(info, &ifdata, NULL, 0, &ReqLen, NULL);
	PSP_INTERFACE_DEVICE_DETAIL_DATA ifDetail = (PSP_INTERFACE_DEVICE_DETAIL_DATA)(malloc(ReqLen));

	if (ifDetail == NULL) {
		SetupDiDestroyDeviceInfoList(info);
		return blReturn;
	}

	// Get symbolic link name                                                                        
	ifDetail->cbSize = sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA);
	if (!SetupDiGetDeviceInterfaceDetail(info, &ifdata, ifDetail, ReqLen, NULL, NULL)) {
		SetupDiDestroyDeviceInfoList(info);
		free(ifDetail);
		return blReturn;
	}

	//strcpy(DevicePath, ifDetail->DevicePath);
    _tcscpy_s(DevicePath, 256, ifDetail->DevicePath);
	blReturn = TRUE;//TRUE                                                                                             
	free(ifDetail);
	SetupDiDestroyDeviceInfoList(info);
	return blReturn;
}

vector<estring>& UsbDrive::getInterfaceString(vector<estring>& colls) {
	echar DevicePath[256] = { 0 };
	eu32 lID;
	for (lID = 0; lID < 26; lID++) {
		if (GetDeviceViaInterface((LPGUID)&GUID_DISK_CLASS, lID, DevicePath) == FALSE) //FALSE
		{
			continue;
		}
		colls.push_back(DevicePath);
	}
	return colls;
}

void UsbDrive::UsbStorGetDiskHandle(vector<HandleAndName>& colls, estring DevStringNeeded) {
	HANDLE fileHandle = NULL;
	vector<estring> interfaceColls;
	getInterfaceString(interfaceColls);
	std::transform(DevStringNeeded.begin(), DevStringNeeded.end(), DevStringNeeded.begin(), ::tolower);
	for (size_t i = 0; i < interfaceColls.size(); i++) {
		estring ifstring = interfaceColls[i];
		if (_tcsstr(ifstring.c_str(), DevStringNeeded.c_str()) == NULL) {
			continue;
		}

		fileHandle = CreateFile(ifstring.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		//ifstring = ifstring.substr(ifstring.find('#') + 1);
		if (fileHandle != INVALID_HANDLE_VALUE) {
			HandleAndName d;
			d.first = fileHandle;
			d.second = ifstring;
			colls.push_back(d);
		}
	}
}

void UsbDrive::scanByDriveletter(int deviceType, vector<HandleAndName>& colls) {
	HANDLE handle;
	for (echar drive = _ET('A'); drive < _ET('Z'); drive++) {
        estring driverLetter(1, drive);
        driverLetter += _ET(':');
		if (GetDriveType(driverLetter.c_str()) != DRIVE_REMOVABLE) {
			continue;
		}
        
        estring fullName = _ET("\\\\.\\") + driverLetter;
		handle = CreateFile(fullName.c_str(),
			GENERIC_WRITE | GENERIC_READ,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			0,
			NULL);

		if (handle == INVALID_HANDLE_VALUE) {
			continue;
		}

		HandleAndName han;
		han.first = handle;
		han.second = drive;
		colls.push_back(han);
	}
}

void UsbDrive::lock_volumn(HANDLE handle) {
    DWORD dwBytesReturned = 0;
    DeviceIoControl(handle, FSCTL_LOCK_VOLUME, NULL, 0, NULL, 0, &dwBytesReturned, NULL);
}


