#pragma once

#include "windows.h" // for HANDLE Type
#include "DefineFiles/Definefile.h"
#include "DriveConfig.h"

#include "Utility\Utility.h"

using namespace EricCore;

#define	FLAG_DATA_IN			0x00000002
#define	FLAG_DATA_OUT			0x00000004

typedef bool(*ScsiSpecailFun)(eu8* cdb, eu8* buffer, int Length, eu8 direction);
typedef void(*ErrorHandleFun)(void);

class UsbCommand
{
public:
	UsbCommand() {};
	UsbCommand(HANDLE deviceHandle);
	~UsbCommand(void);

	static ScsiSpecailFun specialCmdHandle;

	static ErrorHandleFun errHandleFun;
	static ScsiSpecailFun posSendCmd;
	

	static void static_sendCommand(eu8* cdb, eu8* buffer, int Length, eu8 direction);
	void sendCommand(eu8* cdb, eu8* buffer, int Length, eu8 direction, estring desc) ;

    Utility m_u;

	//property
	HANDLE m_dvrHandle;

private:
	
};


#ifdef EXPORT_DLL
#define DLLEXPORT extern "C" __declspec(dllexport)

DLLEXPORT void __stdcall usbcmd_sendCommand(HANDLE handle, eu8* cdb, eu8* buffer, int Length, eu8 direction) {
	UsbCommand cmd(handle);
	cmd.sendCommand(cdb, buffer, Length, direction);
}
#endif
