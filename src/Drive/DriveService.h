#pragma once
#include <vector>
#include "Drive.h"
#include <string>
#include "DriveConfig.h"

#include "DefineFiles/DefineFile.h"

using namespace std;

#define DRIVE_SERVICE_TYPE (1)
#define FILTER_NAME _ET("")

class DriveService
{
public:
	//----------------------- function ---------------------------------
	~DriveService(void);
	Drive getIdelDevice();
	Drive getIdelDeviceByName(const estring&  deviceName);
	Drive getIdelDeviceById(int testId);

	static DriveService* getInstance(int deviceType=DRIVE_SERVICE_TYPE, const estring& filterName = FILTER_NAME);

	void releaseDevice(HANDLE driveAddr);

	int count(void);
	static void releaseSingleton(void);
	void removeDevice(const Drive& objDrive);
	Drive getAt(int i);
	void getInterfaceString(vector<estring>& colls);
    vector<estring>& getDeviceNameList(vector<estring>& colls);
	
private :
	DriveService(int deviceType, estring filterName);
	DriveService(const DriveService&);
	vector<Drive> _driveCollection;
	static DriveService* _instance;
	void _checkDrive(const Drive& d);
    void exception(int num, const echar* str);
};

#ifdef EXPORT_DLL
#define DLLEXPORT extern "C" __declspec(dllexport)
DLLEXPORT DriveService* __stdcall ds_get_ds(int type, echar* str) {
	estring hndleName = str;
	return DriveService::getInstance(type, hndleName);
}

DLLEXPORT void __stdcall ds_release(DriveService* ds) {
	ds->releaseSingleton();
}

DLLEXPORT int __stdcall ds_get_cnt(DriveService* ds) {
	return ds->count();
}

DLLEXPORT HANDLE __stdcall ds_get_handle(DriveService* ds, int no) {
	return ds->getIdelDeviceById(no).address;
}

DLLEXPORT const echar* __stdcall ds_get_name(DriveService* ds, int no, char* p) {
	estring name = ds->getIdelDeviceById(no).name;
	memcpy(p, name.c_str(), name.length());
	return p;
}
#endif