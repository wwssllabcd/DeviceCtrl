//Copyright © 2006~ EricWang(wwssllabcd@gmail.com). All rights reserved

#include "Stdafx.h"
#include ".\devicemanger.h"
#include "DefineFiles\DefineFile.h"

DeviceManger::DeviceManger(void)
	:maxDeviceCnt(32)
{
}

DeviceManger::~DeviceManger(void)
{
}

void DeviceManger::loadDeviceColl(void) {

	eu32 logicDevices = GetLogicalDrives();
	estring deviceName = _ET("A:");

	for(int i = 2; i < this->maxDeviceCnt; i++) {
		if(checkDeviceExist(logicDevices, (eu32)(1 << i)) == false) {
			continue;
		}

		// force char to implement 'A' +i's function
		echar tmp = 'A' + i;
		deviceName[0] = (TCHAR)tmp;
		if(isRemovableDevice(deviceName.c_str()) == false) {
			continue;
		}

		//Open the Drive's handle
		estring accessSymbol = _ET("\\\\.\\");
		accessSymbol += deviceName;
		HANDLE  drvhandle = CreateFile(accessSymbol.c_str(),
			GENERIC_WRITE | GENERIC_READ,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			0,
			NULL);


		if(drvhandle == INVALID_HANDLE_VALUE) {
			continue;
		}

		pair<estring, HANDLE> tmpPair;
		tmpPair.first = deviceName.at(0);
		tmpPair.second = drvhandle;
		deviceHandleColl.push_back(tmpPair);
	}
}

bool DeviceManger::checkDeviceExist(eu32 deviceRef, eu32 deviceNo) {
	if(deviceRef & deviceNo) {
		return true;
	} else {
		return false;
	}
}

bool DeviceManger::isRemovableDevice(const TCHAR* deviceName) {
	if(GetDriveType(deviceName) == DRIVE_REMOVABLE) {
		return true;
	} else {
		return false;
	}
}

