#pragma once

#include "HeadBase.h"

#include <windows.h>     //HANDLE
#include <vector>

#include "DeviceIoUtility.h"




using namespace std;

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

    

    vector<estring> get_device_path();

    vector<DeviceInfo> get_device_handle_colls(vector<estring> devicePaths);
    vector<DeviceInfo> filter(vector<DeviceInfo>& inputColls, CheckFun checkFun);

private:
    DeviceIoUtility m_diu;
};

