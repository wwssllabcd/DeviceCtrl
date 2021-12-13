#pragma once

#include "HeadBase.h"
#include "DeviceIoUtility.h"
#include <windows.h>     //HANDLE
#include <vector>


using namespace std;

enum ScanType {
    driveLetter = 0,
    physicalDriver = 1,
};

typedef struct {
    estring devicePath;
    HANDLE handle;
    eu32 busType;
    estring description;
}DeviceInfo;

typedef bool(*CheckFun)(DeviceInfo&);

class DeviceHandle
{
public:
    DeviceHandle(void);
    ~DeviceHandle();

    HANDLE m_handle;
    estring m_devicePath;

    

    vector<estring> get_device_path(ScanType scanType);

    vector<DeviceInfo> get_device_handle_colls(vector<estring> devicePaths);
    vector<DeviceInfo> filter(vector<DeviceInfo>& inputColls, CheckFun checkFun);

private:
    DeviceIoUtility m_diu;
};

