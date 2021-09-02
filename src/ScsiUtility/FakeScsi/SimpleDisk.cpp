#include "stdafx.h"
#include "SimpleDisk.h"
#include "../DefineScsi.h"
#include "Utility/Utility.h"


SimpleDisk::SimpleDisk(void) {

}

SimpleDisk::~SimpleDisk(void) {
}

void SimpleDisk::init_disk() {

}

void SimpleDisk::lba_read(eu32 lba, eu32 secCnt, eu8_p buffer) {
	memcpy(buffer, m_fakeDevice, SECTOR_TO_BYTE(secCnt));
}

void SimpleDisk::lba_write(eu32 lba, eu32 secCnt, eu8_p buffer) {
	memcpy(buffer, m_fakeDevice, SECTOR_TO_BYTE(secCnt));
}

void SimpleDisk::get_ufi_capacity(eu8_p buffer) {
	eu32 cap = 0xF0000000;

	//MSB format
	buffer[0] = ((cap >> 0x18) & 0xFF);
	buffer[1] = ((cap >> 0x10) & 0xFF);
	buffer[2] = ((cap >> 0x08) & 0xFF);
	buffer[3] = ((cap >> 0x00) & 0xFF);
}

void SimpleDisk::get_inquiry(eu8_p buffer) {
	buffer[0] = 0x46;
	buffer[1] = 0x41;
	buffer[2] = 0x4B;
	buffer[3] = 0x45;
}

eu8 SimpleDisk::send_scsi_cmd(eu8 cdb[16], eu32 dataXferLen, eu8 direction, eu8_p buffer) {
	//Lba read
	eu8 opCode = cdb[0];
	Utility u;
	if (opCode == UFI_OP_READ_10) {
		eu32 lba = u.toU32(cdb + 2);
		eu32 secCnt = u.toU16(cdb + 7);
		lba_read(lba, secCnt, buffer);
		return 0;
	}
	if (opCode == UFI_OP_WRITE_10) {
		eu32 lba = u.toU32(cdb + 2);
		eu32 secCnt = u.toU16(cdb + 7);
		lba_write(lba, secCnt, buffer);
		return 0;
	}

	//UFI: capacity
	if (opCode == UFI_OP_READ_CAPACITY) {
		get_ufi_capacity(buffer);
		return 0;
	}

	if (opCode == UFI_OP_INQUIRY) {
		get_inquiry(buffer);
		return 0;
	}


	return 0;
}