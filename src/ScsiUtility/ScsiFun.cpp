#include "stdafx.h"
#include "ScsiFun.h"
#include "DefineScsi.h"

#include "Utility/Singleton.h"
#include "winioctl.h"          //BusTypeUsb

ScsiFun::ScsiFun()
{
}

ScsiFun::~ScsiFun() {
}

#ifdef _ENABLE_PHYSICAL_DEVICE_


vector<DeviceInfo> ScsiFun::scan_all_device() {
    DeviceHandle deviceHandle;
    vector<DeviceInfo> deviceInfo = deviceHandle.get_device_handle_colls(deviceHandle.get_device_path());
    return deviceInfo;
}

vector<DeviceInfo> ScsiFun::filter_device(vector<DeviceInfo> deviceInfo, CheckFun filterFun) {
    DeviceHandle deviceHandle;
    if (filterFun) {
        deviceInfo = deviceHandle.filter(deviceInfo, filterFun);
    }
    return deviceInfo;
}

vector<DeviceInfo> ScsiFun::scan_device(CheckFun filterFun) {
    vector<DeviceInfo> deviceInfo = filter_device(scan_all_device(), filterFun);
    return deviceInfo;
}
#else
vector<DeviceInfo> ScsiFun::scan_device(CheckFun filterFun) {
    vector<DeviceInfo> deviceInfo;
    DeviceInfo di;

    di.handle = (HANDLE)FAKE_HANDLE;
    di.devicePath = _ET("FAKE_C");
    di.busType = BusTypeUsb;

    deviceInfo.push_back(di);


    di.devicePath = _ET("FAKE_D");
    deviceInfo.push_back(di);
    return deviceInfo;
}
#endif

bool ScsiFun::is_usb_bus_type(DeviceInfo& deviceInfo) {
    if (deviceInfo.busType != BusTypeUsb) {
        return false;
    }
    return true;
}

void ScsiFun::setup_singleton(vector<DeviceInfo> deviceInfos) {
    Singleton<ScsiIf>* s = Singleton<ScsiIf>::get_instance();
    s->clear();

    for (auto item : deviceInfos) {
        ScsiIf obj(item.handle, item.devicePath);
        s->push_back(obj);
    }
}

ScsiIf ScsiFun::get_form_singleton(eu32 num) {
    Singleton<ScsiIf>* s = Singleton<ScsiIf>::get_instance();
    eu32 cnt = s->size();
    if (cnt == 0) {
        THROW_MYEXCEPTION(0, _ET("No Device Found"));
    }
    if (num >= cnt) {
        THROW_MYEXCEPTION(0, _ET("select num ofb, num=%X, sizecnt=%X"), num, cnt);
    }

    return s->get_item(num);
}

void ScsiFun::release() {
    Singleton<ScsiIf>* s = Singleton<ScsiIf>::get_instance();
    s->release();
}

vector<estring> ScsiFun::get_device_name() {
    Singleton<ScsiIf>* s = Singleton<ScsiIf>::get_instance();

    vector<estring> colls;
    eu32 cnt = s->size();
    for (eu32 i = 0; i < cnt; i++) {
        ScsiIf obj = s->get_item(i);
        colls.push_back(obj.m_deviceName);
    }

    return colls;
}

