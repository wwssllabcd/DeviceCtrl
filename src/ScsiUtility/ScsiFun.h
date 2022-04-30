#pragma once

#include "DefineFiles/DefineFile.h"
#include "ScsiIf.h"
#include "DefineScsi.h"

#include <vector>

#include "../Device/DeviceHandle.h"




class ScsiFun
{
public:

    ScsiFun();
    ~ScsiFun();
    vector<DeviceInfo> scan_all_device(ScanType scanType);
    vector<DeviceInfo> scan_device(ScanType scanType, CheckFun filterFun);

    vector<DeviceInfo> filter_device(vector<DeviceInfo> deviceInfo, CheckFun filterFun);
    
    void setup_singleton(vector<DeviceInfo> deviceInfos);
    ScsiIf get_form_singleton(eu32 num);
    vector<estring> get_device_description();
    void release();

    bool is_usb_bus_type(DeviceInfo& deviceInfo);
    estring get_device_info_string(vector<DeviceInfo> deviceInfos);

private:
    

};

