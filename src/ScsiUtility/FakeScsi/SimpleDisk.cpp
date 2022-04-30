#include "stdafx.h"
#include "SimpleDisk.h"
#include "../DefineScsi.h"
#include "Utility/Utility.h"


//#define _ENABLE_FAKE_DISK_FTL_
using namespace EricCore;

SimpleDisk::SimpleDisk(void) {

}

SimpleDisk::~SimpleDisk(void) {
}

#ifdef _ENABLE_FAKE_DISK_FTL_
extern "C" {
#include "ftl.h"
}

void SimpleDisk::init_disk() {
	ftl_init();
}

void SimpleDisk::lba_read(eu32 lba, eu32 secCnt, eu8_p buffer) {
	ftl_lba_read(lba, secCnt, buffer);
}

void SimpleDisk::lba_write(eu32 lba, eu32 secCnt, eu8_p buffer) {
	ftl_lba_write(lba, secCnt, buffer);
}

void SimpleDisk::get_ufi_capacity(eu8_p buffer) {
	ftl_get_capacity(buffer);
}

#else

void SimpleDisk::init_disk() {
}

void SimpleDisk::lba_read(eu32 lba, eu32 secCnt, eu8_p buffer) {
	memcpy(buffer, m_fakeDevice, SEC_TO_BYTE(secCnt));
}

void SimpleDisk::lba_write(eu32 lba, eu32 secCnt, eu8_p buffer) {
	memcpy(buffer, m_fakeDevice, SEC_TO_BYTE(secCnt));
}

void SimpleDisk::get_ufi_capacity(eu8_p buffer) {
	eu32 cap = 0xF0000000;
	//MSB format
	buffer[0] = ((cap >> 0x18) & 0xFF);
	buffer[1] = ((cap >> 0x10) & 0xFF);
	buffer[2] = ((cap >> 0x08) & 0xFF);
	buffer[3] = ((cap >> 0x00) & 0xFF);
}
#endif

void SimpleDisk::get_inquiry(eu8_p buffer) {
	buffer[0] = 'F';
	buffer[1] = 'A';
	buffer[2] = 'K';
	buffer[3] = 'E';
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