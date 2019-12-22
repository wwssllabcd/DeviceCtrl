//Copyright © 2006~ EricWang(wwssllabcd@gmail.com). All rights reserved

#include "stdafx.h"
#include ".\driveservice.h"
#include "Utility/EricException.h"
#include <algorithm>
#include "UsbDrive.h"

using namespace std;
using namespace EricCore;

//因為也只有 A~Z 24個裝置名稱
#define	 MAX_DEVICE		32

DriveService*   DriveService::_instance = 0;// initialize pointer
HANDLE hMutex;


DriveService::DriveService(int deviceType, estring filterName)
{
	UsbDrive objDrive;
	vector<pair<HANDLE, estring>> colls;

	if(deviceType == 3) {
		objDrive.UsbStorGetDiskHandle(colls, filterName);
	} else {
		objDrive.scanByDriveletter(deviceType, colls);
	}

	for(size_t i = 0; i < colls.size(); i++) {
		pair<HANDLE, estring> d = colls[i];
		Drive dr;
		dr.address = d.first;
		dr.name = d.second;
		dr.testNo = this->_driveCollection.size();
		dr.status = IDEL;
		this->_driveCollection.push_back(dr);
	}
}

DriveService* DriveService::getInstance(int deviceType, const estring& filterName) {
	hMutex = CreateMutex(NULL, true, _ET("DriveService"));
	WaitForSingleObject(hMutex, INFINITE);
	if(_instance == 0) {
		_instance = new DriveService(deviceType, filterName);
	}
	ReleaseMutex(hMutex);
	return _instance;
}

DriveService::~DriveService(void)
{
}

void DriveService::exception(int num, const echar* str){
    THROW_MYEXCEPTION(num, str);
}

Drive DriveService::getIdelDevice() {
	Drive d;
	vector<Drive>::iterator iter;
	WaitForSingleObject(hMutex, INFINITE);
	for(iter = this->_driveCollection.begin(); iter != this->_driveCollection.end(); ++iter) {
		if(iter->status == IDEL) {
			iter->status = BUSY;
			d = (*iter);
			break;
		}
	}
	ReleaseMutex(hMutex);
	//驗證Drive是否合法，會丟exceptioin
	_checkDrive(d);
	return d;
}

Drive DriveService::getIdelDeviceByName(const estring& deviceName) {
	Drive d;
	vector<Drive>::iterator iter;
	WaitForSingleObject(hMutex, INFINITE);
	estring deviceNameUp = deviceName;
	for(iter = this->_driveCollection.begin(); iter != this->_driveCollection.end(); ++iter) {
		transform(iter->name.begin(), iter->name.end(), iter->name.begin(), tolower);
		if(iter->name == deviceNameUp) {
			iter->status = BUSY;
			d = (*iter);
			break;
		}
	}
	ReleaseMutex(hMutex);
	_checkDrive(d);
	return d;
}

Drive DriveService::getIdelDeviceById(int testId) {
	Drive d;
	vector<Drive>::iterator iter;
	WaitForSingleObject(hMutex, INFINITE);
	for(iter = this->_driveCollection.begin(); iter != this->_driveCollection.end(); ++iter) {
		if(iter->testNo == testId) {
			d = (*iter);
			iter->status = BUSY;
			break;
		}
	}
	ReleaseMutex(hMutex);

	d.testNo = testId;
	_checkDrive(d);
	return d;
}

void DriveService::releaseDevice(HANDLE handle) {
	vector<Drive>::iterator iter;
	WaitForSingleObject(hMutex, INFINITE);

	for(iter = this->_driveCollection.begin(); iter != this->_driveCollection.end(); ++iter) {
		if(iter->address == handle) {
			if(iter->status == BUSY) {
				iter->status = IDEL;
			}
		}
	}
	ReleaseMutex(hMutex);
}

void DriveService::_checkDrive(const Drive& objDrive) {
	if(objDrive.address == 0) {
		estring msg, testNo;
		echar tmp;
        _itot_s(objDrive.testNo, &tmp, 1, 10);

		testNo = tmp;
		msg += _ET("Can not Detect No.") + testNo + _ET(" Tester!\n\r");
		msg += _ET("Please Check Tester ID or Reset Tester");
        exception(DS_CHECK_DRIVE_FAIL, msg.c_str());
	}
}

int DriveService::count(void) {
	return (int)this->_driveCollection.size();
}

void DriveService::releaseSingleton(void) {
	if (_instance) {
		delete _instance;
	}
	_instance = 0;
}

void DriveService::removeDevice(const Drive& objDrive) {
	_driveCollection.erase(remove(_driveCollection.begin(), _driveCollection.end(), objDrive), _driveCollection.end());
}

Drive DriveService::getAt(int i) {
	size_t cnt = _driveCollection.size();
	if(cnt == 0) {
        exception(DS_NO_HANDLE, _ET("DS_NO_HANDLE"));
	}
	return _driveCollection[i];
}

void DriveService::getInterfaceString(vector<estring>& colls)
{
	UsbDrive objDrive;
	objDrive.getInterfaceString(colls);
}

vector<estring>& DriveService::getDeviceNameList(vector<estring>& colls) {
	colls.clear();
	Drive d;
	for(int i = 0; i < this->count(); i++) {
		d = this->getIdelDeviceById(i);
		colls.push_back(d.name);
	}
	return colls;
}