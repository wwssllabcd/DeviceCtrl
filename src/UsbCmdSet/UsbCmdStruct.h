#pragma once
#include "DefineFiles/DefineFile.h"
#include <vector>

using namespace std;

class UsbCmdStruct
{
public:
	UsbCmdStruct(void);
	~UsbCmdStruct(void);

	eu8 cdb[16];
	int length;
	eu8 direction;
	estring description;

	typedef std::vector<UsbCmdStruct> UsbCmdSet;

	bool operator ==(const UsbCmdStruct& rhs) const;


	UsbCmdSet getAllCommandSet(void);

	UsbCmdStruct requestSense(void);
	UsbCmdStruct inquiry(void);
	UsbCmdStruct readCapacity(void);
	UsbCmdStruct testUnitReady(void);
	UsbCmdStruct readFormatCapacity(void);
	
	UsbCmdStruct preventAllowMediumRemoval(void);

	UsbCmdStruct read10(void);
	UsbCmdStruct read10(eu32 lba, eu16 secCnt);
	UsbCmdStruct write10(void);
	UsbCmdStruct write10(eu32 lba, eu16 secCnt);

	//UsbCmdStruct vdrReboot(void);

};
