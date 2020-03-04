//Copyright © 2006~ EricWang(wwssllabcd@gmail.com). All rights reserved

#include "Stdafx.h"
#include "./UsbCommand.h"

#include "UsbDrive.h"
#include "Utility/EricException.h"
#include "Utility/Utility.h"



ScsiSpecailFun UsbCommand::specialCmdHandle = 0;
ScsiSpecailFun UsbCommand::posSendCmd = 0;
ErrorHandleFun UsbCommand::errHandleFun = 0;

UsbCommand::UsbCommand(HANDLE deviceHandle)
	:m_dvrHandle(deviceHandle)
{
}

UsbCommand::~UsbCommand(void)
{
}

void UsbCommand::static_sendCommand(eu8* cdb, eu8* buffer, int Length, eu8 direction) {
	if(UsbCommand::specialCmdHandle) {
		bool res = UsbCommand::specialCmdHandle(cdb, buffer, Length, direction);
	}
}

void UsbCommand::sendCommand(eu8* cdb, eu8* buffer, int Length, eu8 direction, estring desc) {
	UsbDrive usbDrive;
    Utility su;
	bool status = false;
	if(this->specialCmdHandle) {
		status = this->specialCmdHandle(cdb, buffer, Length, direction);
		if(status == false) {
			status = usbDrive.sendCommand(m_dvrHandle, cdb, buffer, Length, direction);
		}
	} else {
		status = usbDrive.sendCommand(m_dvrHandle, cdb, buffer, Length, direction);
	}
	
	if(this->posSendCmd) {
		this->posSendCmd(cdb, buffer, Length, direction);
	}

	if (status == false) {
		// errorcode: "https://docs.microsoft.com/zh-tw/windows/desktop/Debug/system-error-codes"
		eu32 errorCode = GetLastError();

		if(this->errHandleFun) {
			this->errHandleFun();
		}
		
		estring msg = _ET("Usb Command Fail:") + su.arrayToHexString(cdb, 12);
		msg += su.strFormat(_ET(".len=%X, Dir=%X, errorCode = %X"), Length, direction, errorCode) + _ET(", desc=") + desc;
		THROW_MYEXCEPTION(USBC_SCSI_CMD_FAIL, msg);
	}
}

