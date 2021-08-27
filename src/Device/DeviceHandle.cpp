
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

    for (auto devicePath : devicePaths) {
        HANDLE handle = m_diu.open_device(devicePath);
        if (handle == INVALID_HANDLE_VALUE) {
            continue;
        }

        DeviceInfo obj;
        obj.devicePath = devicePath;
        obj.handle = handle;
        obj.busType = m_diu.get_bus_type(handle);

        colls.push_back(obj);
    }
    return colls;
}

vector<DeviceInfo> DeviceHandle::filter(vector<DeviceInfo>& inputColls, CheckFun checkFun) {
    vector<DeviceInfo> resultColls;
    for (auto item : inputColls) {
        if (checkFun(item) == false) {
            m_diu.close_device(item.handle);
            continue;
        }
        resultColls.push_back(item);
    }
    return resultColls;
}

