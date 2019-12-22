#pragma once
#include "DefineFiles\DefineFile.h"
#include <vector>
#include <initguid.h>  //GUID
#include <windows.h>   //Handle

using namespace std;


namespace EricCore {
	class UsbDrive
	{
	public:
		typedef pair<HANDLE, estring> HandleAndName;

	
		UsbDrive(void);
		~UsbDrive(void);
		bool sendCommand(HANDLE hUsb, PUCHAR cdb, PUCHAR DataBuffer, eu32  DataTransferLen, UCHAR  fDirection);
		void scanByDriveletter(int deviceType, vector<HandleAndName>& colls);

		vector<estring>& getInterfaceString(vector<estring>& colls);
		void UsbStorGetDiskHandle(vector<HandleAndName>& colls, estring DevStringNeeded);
        void lock_volumn(HANDLE handle);
	private:
		bool SendSptiCommand(HANDLE drive_handle, eu8* cdb, eu8* iobuf, DWORD buflen, eu8 flags, eu16 timeout);
		
		bool GetDeviceViaInterface(GUID* pGuid, eu32 instance, echar* const DevicePath);
	};
};