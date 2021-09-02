
#include "stdafx.h"
#include "ScsiIf.h"
#include "winioctl.h" //STORAGE_BUS_TYPE


ScsiIf::ScsiIf(HANDLE handle, estring deviceName)
    :m_handle(handle)
    ,m_deviceName(deviceName)
{
}

ScsiIf::ScsiIf()
{
}

ScsiIf::~ScsiIf(void)
{
}

//---------------------------------

void ScsiIf::init_disk() {
    m_scsiIoIf.init_disk();
}

void ScsiIf::send_cmd(BYTE cdb[16], ULONG dataXferLen, BYTE direction, eu8_p buffer) {
    m_scsiIoIf.scsi_io_cmd(m_handle, cdb, dataXferLen, direction, buffer);
}

void ScsiIf::send_cmd(ScsiCmd cmd, eu8_p buffer) {
    send_cmd(cmd.cdb, cmd.length, (BYTE)cmd.direction, buffer);
}

void ScsiIf::write_10(eu32 lba, eu16 secCnt, eu8_p buffer) {
    ScsiCmd cmd;
    send_cmd(cmd.write_10(lba, secCnt), buffer);
}

void ScsiIf::read_10(eu32 lba, eu16 secCnt, eu8_p buffer) {
    ScsiCmd cmd;
    send_cmd(cmd.read_10(lba, secCnt), buffer);
}

void ScsiIf::inquiry(eu8_p buffer) {
    ScsiCmd cmd;
    send_cmd(cmd.inquiry(), buffer);
}

void ScsiIf::read_capacity(eu8_p buffer) {
    ScsiCmd cmd;
    send_cmd(cmd.read_capacity(), buffer);
}
