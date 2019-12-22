#pragma once

#include <windows.h>
#include <tchar.h>

#include <string>
#include <vector>
#include "DefineFiles/DefineFile.h"

using namespace std;


class DeviceManger
{
public:
	DeviceManger(void);
	~DeviceManger(void);

	void loadDeviceColl(void);
	bool isRemovableDevice(const TCHAR* deviceName);
	bool checkDeviceExist(eu32 deviceRef, eu32 deviceNo);


	const int maxDeviceCnt;

	vector< pair<estring, HANDLE> > deviceHandleColl;


};
