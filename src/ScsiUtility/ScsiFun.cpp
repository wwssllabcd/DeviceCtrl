#include "stdafx.h"
#include "ScsiFun.h"
#include "DefineScsi.h"

#include "Utility/Singleton.h"
#include "Utility/Utility.h"

#include "winioctl.h"          //BusTypeUsb

using namespace EricCore;

ScsiFun::ScsiFun()
{
}

ScsiFun::~ScsiFun() {
}

#ifdef _ENABLE_PHYSICAL_DEVICE_


vector<DeviceInfo> ScsiFun::scan_all_device(ScanType scanType) {
    DeviceHandle deviceHandle;
    vector<DeviceInfo> deviceInfo = deviceHandle.get_device_handle_colls(deviceHandle.get_device_path(scanType));
    return deviceInfo;
}

vector<DeviceInfo> ScsiFun::filter_device(vector<DeviceInfo> deviceInfo, CheckFun filterFun) {
    DeviceHandle deviceHandle;
    if (filterFun) {
        deviceInfo = deviceHandle.filter(deviceInfo, filterFun);
    }
    return deviceInfo;
}

vector<DeviceInfo> ScsiFun::scan_device(ScanType scanType, CheckFun filterFun) {
    vector<DeviceInfo> deviceInfo = filter_device(scan_all_device(scanType), filterFun);
    return deviceInfo;
}
#else
#define FAKE_HANDLE                     (0xFFFF1234)
vector<DeviceInfo> ScsiFun::scan_device(ScanType scanType, CheckFun filterFun) {
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
        ScsiIf obj(item.handle, item.devicePath, item.description);
        s->push_back(obj);
    }
}

estring ScsiFun::get_device_info_string(vector<DeviceInfo> deviceInfos) {
    estring msg;
    Utility u;
    for (auto item : deviceInfos) {
        msg += _ET("path = ") + item.devicePath + ECRLF;
        msg += _ET("bus type = ") + u.toHexString(item.busType) + ECRLF;
        msg += _ET("handle = ") + u.toHexString((eu32)item.handle) + ECRLF;
        msg += _ET("description = ") + item.description + ECRLF;
    }
    return msg;
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

vector<estring> ScsiFun::get_device_description() {
    Singleton<ScsiIf>* s = Singleton<ScsiIf>::get_instance();

    vector<estring> colls;
    eu32 cnt = s->size();
    for (eu32 i = 0; i < cnt; i++) {
        ScsiIf obj = s->get_item(i);
        colls.push_back(obj.m_deviceName + _ET(" -- ") + obj.m_description);
    }

    return colls;
}

