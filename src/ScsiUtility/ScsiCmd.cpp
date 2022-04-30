
#include "stdafx.h"
#include "ScsiCmd.h"


ScsiCmd::ScsiCmd(void){
	memset(this->cdb, 00, sizeof(this->cdb));
	this->length = 0;
	this->direction = ScsiIoDir::NON_DATA;
}

ScsiCmd::~ScsiCmd(void)
{
}

bool ScsiCmd::operator ==(const ScsiCmd& rhs) const {
	int result = memcmp(this->cdb, rhs.cdb, sizeof(rhs.cdb));
	if(result != 0) { return false; }
	if(this->length != rhs.length) { return false; }
	if(this->direction != rhs.direction) { return false; }
	if(this->description.compare(rhs.description) != 0) {
		return false;
	}
	return true;
}

ScsiCmdSet ScsiCmd::get_all_command_set(void) {
	ScsiCmdSet cmdSet;
	cmdSet.push_back(read_10(0, 1));
	cmdSet.push_back(write_10(0, 1));
	cmdSet.push_back(inquiry());
	cmdSet.push_back(test_unit_ready());
	cmdSet.push_back(read_capacity());
	cmdSet.push_back(read_format_capacity());
	cmdSet.push_back(request_sense());
    cmdSet.push_back(prevent_allow_medium_removal());
    cmdSet.push_back(ata_pass_through_12());
	return cmdSet;
}

ScsiCmd ScsiCmd::request_sense(void) {
	ScsiCmd cmd;
	cmd.cdb[0] = UFI_OP_REQUEST_SENSE;
	cmd.cdb[1] = 0x00;
	cmd.cdb[2] = 0x00;
	cmd.cdb[3] = 0x00;
	cmd.cdb[4] = 0x12;
	cmd.length = 18;
	cmd.direction = ScsiIoDir::DATA_IN;
	cmd.description = _ET("SCSI: Request Sense");
	return cmd;
}

ScsiCmd ScsiCmd::inquiry(void) {
	ScsiCmd cmd;
	cmd.cdb[0] = UFI_OP_INQUIRY;
	cmd.cdb[1] = 0x00;
	cmd.cdb[2] = 0x00;
	cmd.cdb[3] = 0x00;
	cmd.cdb[4] = 0x24;
	cmd.length = 0x24;
	cmd.direction = ScsiIoDir::DATA_IN;
	cmd.description = _ET("SCSI: Inquiry");
	return cmd;
}

ScsiCmd ScsiCmd::read_capacity(void) {
	ScsiCmd cmd;
	cmd.cdb[0] = UFI_OP_READ_CAPACITY;
	cmd.cdb[1] = 0x00;
	cmd.length = 8;
	cmd.direction = ScsiIoDir::DATA_IN;
	cmd.description = _ET("SCSI: Read Capacity");
	return cmd;
}

ScsiCmd ScsiCmd::test_unit_ready(void) {
	ScsiCmd cmd;
	cmd.cdb[0] = UFI_OP_TEST_UNIT_READY;
	cmd.length = 0;
	cmd.direction = ScsiIoDir::NON_DATA;
	cmd.description = _ET("SCSI: Test UnitReady");
	return cmd;
}

ScsiCmd ScsiCmd::read_format_capacity(void) {
	ScsiCmd cmd;
	cmd.cdb[0] = UFI_OP_READ_FORMAT_CAPACITY;
	cmd.length = 12;
	cmd.direction = ScsiIoDir::DATA_IN;
	cmd.description = _ET("SCSI: Read Format Capacity");
	return cmd;
}

ScsiCmd ScsiCmd::prevent_allow_medium_removal(void) {
	ScsiCmd cmd;
	cmd.cdb[0] = 0x1e;
	cmd.length = 0;
	cmd.direction = ScsiIoDir::NON_DATA;
	cmd.description = _ET("SCSI: Prevent-Allow Medium Removal");
	return cmd;
}

ScsiCmd ScsiCmd::read_10(eu32 lba, eu16 secCnt) {
	ScsiCmd cmd;

    cmd.cdb[0] = UFI_OP_READ_10;
	cmd.cdb[2] = (eu8)((lba >> 0x18) & 0xFF);
	cmd.cdb[3] = (eu8)((lba >> 0x10) & 0xFF);
	cmd.cdb[4] = (eu8)((lba >> 0x08) & 0xFF);
	cmd.cdb[5] = (eu8)((lba >> 0x00) & 0xFF);
	cmd.cdb[7] = (secCnt >> 0x08) & 0xFF;
	cmd.cdb[8] = (secCnt >> 0x00) & 0xFF;
	
    cmd.length = SEC_TO_BYTE(secCnt);
    cmd.direction = ScsiIoDir::DATA_IN;
    cmd.description = _ET("SCSI: Read(10)");
	return cmd;
}

ScsiCmd ScsiCmd::write_10(eu32 lba, eu16 secCnt) {
	ScsiCmd cmd;
    cmd.cdb[0] = UFI_OP_WRITE_10;
	cmd.cdb[2] = (eu8)((lba >> 0x18) & 0xFF);
	cmd.cdb[3] = (eu8)((lba >> 0x10) & 0xFF);
	cmd.cdb[4] = (eu8)((lba >> 0x08) & 0xFF);
	cmd.cdb[5] = (eu8)((lba >> 0x00) & 0xFF);
	cmd.cdb[7] = (secCnt >> 0x08) & 0xFF;
	cmd.cdb[8] = (secCnt >> 0x00) & 0xFF;
	
	cmd.length = SEC_TO_BYTE(secCnt);
    cmd.direction = ScsiIoDir::DATA_OUT;
    cmd.description = _ET("SCSI: Write(10)");

	return cmd;
}

ScsiCmd ScsiCmd::ata_pass_through_12() {
    ScsiCmd cmd;
    cmd.cdb[0] = SCSI_OP_SAT12;
    cmd.description = _ET("SCSI: ata_pass_through(12)");
    return cmd;
}
