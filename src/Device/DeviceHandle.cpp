
#include "stdafx.h"
#include "DeviceHandle.h"
#include <fileapi.h>        //CreateFile
#include <handleapi.h>      //INVALID_HANDLE_VALUE



DeviceHandle::DeviceHandle(void) {
}

DeviceHandle::~DeviceHandle()
{
}

vector<estring> DeviceHandle::get_device_path() {
    //return m_diu.gen_string_physical_drive();
    return m_diu.gen_string_drive_letter();
}

vector<DeviceInfo> DeviceHandle::get_device_handle_colls(vector<estring> devicePaths) {
    vector<DeviceInfo> colls;
    for (eu32 i = 0; i < devicePaths.size(); i++) {
        HANDLE handle = m_diu.open_device(devicePaths[i]);
        if (handle == INVALID_HANDLE_VALUE) {
            continue;
        }

        DeviceInfo obj;
        obj.devicePath = devicePaths[i];
        obj.handle = handle;
        obj.busType = m_diu.get_bus_type(handle);

        colls.push_back(obj);
    }
    return colls;
}

vector<DeviceInfo> DeviceHandle::filter(vector<DeviceInfo>& inputColls, CheckFun checkFun) {
    vector<DeviceInfo> resultColls;
    for (eu32 i = 0; i < inputColls.size(); i++) {
        if (checkFun(inputColls[i]) == false) {
            m_diu.close_device(inputColls[i].handle);
            continue;
        }
        resultColls.push_back(inputColls[i]);
    }
    return resultColls;
}

