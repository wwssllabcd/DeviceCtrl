//Copyright © 2006~ EricWang(wwssllabcd@gmail.com). All rights reserved
 
#include "stdafx.h"
#include "./UsbCmdStruct.h"



#define	FLAG_DATA_IN			0x00000002
#define	FLAG_DATA_OUT			0x00000004

UsbCmdStruct::UsbCmdStruct(void)
{
	memset(this->cdb, 00, sizeof(this->cdb));
	this->length = 0;
	this->direction = FLAG_DATA_IN;
}

UsbCmdStruct::~UsbCmdStruct(void)
{
}

bool UsbCmdStruct::operator ==(const UsbCmdStruct& rhs) const {

	int result = memcmp(this->cdb, rhs.cdb, sizeof(rhs.cdb));
	if(result != 0) { return false; }
	if(this->length != rhs.length) { return false; }
	if(this->direction != rhs.direction) { return false; }
	if(this->description.compare(rhs.description) != 0) {
		return false;
	}

	return true;
}

UsbCmdStruct::UsbCmdSet UsbCmdStruct::getAllCommandSet(void) {
	UsbCmdSet cmdSet;
	cmdSet.push_back(this->read10());
	cmdSet.push_back(this->write10());
	cmdSet.push_back(this->inquiry());
	cmdSet.push_back(this->testUnitReady());
	cmdSet.push_back(this->readCapacity());
	cmdSet.push_back(this->readFormatCapacity());
	cmdSet.push_back(this->requestSense());
	cmdSet.push_back(this->preventAllowMediumRemoval());
	return cmdSet;
}

UsbCmdStruct UsbCmdStruct::requestSense(void) {
	UsbCmdStruct usbCmdSet;
	usbCmdSet.cdb[0] = 0x03;
	usbCmdSet.cdb[1] = 0x00;
	usbCmdSet.cdb[2] = 0x00;
	usbCmdSet.cdb[3] = 0x00;
	usbCmdSet.cdb[4] = 0x12;
	usbCmdSet.length = 18;
	usbCmdSet.direction = FLAG_DATA_IN;
	usbCmdSet.description = _ET("UFI: Request Sense");
	return usbCmdSet;
}

UsbCmdStruct UsbCmdStruct::inquiry(void) {
	UsbCmdStruct usbCmdSet;
	usbCmdSet.cdb[0] = 0x12;
	usbCmdSet.cdb[1] = 0x00;
	usbCmdSet.cdb[2] = 0x00;
	usbCmdSet.cdb[3] = 0x00;
	usbCmdSet.cdb[4] = 0x24;
	usbCmdSet.length = 0x24;
	usbCmdSet.direction = FLAG_DATA_IN;
	usbCmdSet.description = _ET("UFI: Inquiry");
	return usbCmdSet;
}

UsbCmdStruct UsbCmdStruct::readCapacity(void) {
	UsbCmdStruct usbCmdSet;
	usbCmdSet.cdb[0] = 0x25;
	usbCmdSet.cdb[1] = 0x00;
	usbCmdSet.length = 8;
	usbCmdSet.direction = FLAG_DATA_IN;
	usbCmdSet.description = _ET("UFI: Read Capacity");
	return usbCmdSet;
}

UsbCmdStruct UsbCmdStruct::testUnitReady(void) {
	UsbCmdStruct usbCmdSet;
	usbCmdSet.cdb[0] = 0x00;
	usbCmdSet.length = 0;
	usbCmdSet.direction = FLAG_DATA_IN;
	usbCmdSet.description = _ET("UFI: Test UnitReady");
	return usbCmdSet;
}

UsbCmdStruct UsbCmdStruct::readFormatCapacity(void) {
	UsbCmdStruct usbCmdSet;
	usbCmdSet.cdb[0] = 0x23;
	usbCmdSet.length = 12;
	usbCmdSet.direction = FLAG_DATA_IN;
	usbCmdSet.description = _ET("UFI: Read Format Capacity");
	return usbCmdSet;
}

UsbCmdStruct UsbCmdStruct::preventAllowMediumRemoval(void) {
	UsbCmdStruct usbCmdSet;
	usbCmdSet.cdb[0] = 0x1e;
	usbCmdSet.length = 0;
	usbCmdSet.direction = FLAG_DATA_IN;
	usbCmdSet.description = _ET("UFI: Prevent-Allow Medium Removal");
	return usbCmdSet;
}

UsbCmdStruct UsbCmdStruct::read10(void) {
	UsbCmdStruct usbCmdSet;
	usbCmdSet.cdb[0] = 0x28;
	usbCmdSet.cdb[8] = 1;
	usbCmdSet.length = 512;
	usbCmdSet.direction = FLAG_DATA_IN;
	usbCmdSet.description = _ET("UFI: Read(10)");
	return usbCmdSet;
}

UsbCmdStruct UsbCmdStruct::read10(eu32 lba, eu16 secCnt) {
	UsbCmdStruct usbCmdSet;
	usbCmdSet = usbCmdSet.read10();
	usbCmdSet.cdb[2] = (eu8)((lba >> 0x18) & 0xFF);
	usbCmdSet.cdb[3] = (eu8)((lba >> 0x10) & 0xFF);
	usbCmdSet.cdb[4] = (eu8)((lba >> 0x08) & 0xFF);
	usbCmdSet.cdb[5] = (eu8)((lba >> 0x00) & 0xFF);
	usbCmdSet.cdb[7] = (secCnt >> 0x08) & 0xFF;
	usbCmdSet.cdb[8] = (secCnt >> 0x00) & 0xFF;
	usbCmdSet.length = secCnt * 0x200;
	return usbCmdSet;
}

UsbCmdStruct UsbCmdStruct::write10(void) {
	UsbCmdStruct usbCmdSet;
	usbCmdSet.cdb[0] = 0x2a;
	usbCmdSet.cdb[8] = 1;
	usbCmdSet.length = 512;
	usbCmdSet.direction = FLAG_DATA_OUT;
	usbCmdSet.description = _ET("UFI: Write(10)");
	return usbCmdSet;
}

UsbCmdStruct UsbCmdStruct::write10(eu32 lba, eu16 secCnt) {
	UsbCmdStruct usbCmdSet;
	usbCmdSet = usbCmdSet.write10();
	usbCmdSet.cdb[2] = (eu8)((lba >> 0x18) & 0xFF);
	usbCmdSet.cdb[3] = (eu8)((lba >> 0x10) & 0xFF);
	usbCmdSet.cdb[4] = (eu8)((lba >> 0x08) & 0xFF);
	usbCmdSet.cdb[5] = (eu8)((lba >> 0x00) & 0xFF);
	usbCmdSet.cdb[7] = (secCnt >> 0x08) & 0xFF;
	usbCmdSet.cdb[8] = (secCnt >> 0x00) & 0xFF;
	usbCmdSet.length = secCnt * 0x200;
	return usbCmdSet;
}

