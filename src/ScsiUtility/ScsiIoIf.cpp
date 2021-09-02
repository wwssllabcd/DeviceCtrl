#include "stdafx.h"
#include "ScsiIoIf.h"
#include "DefineScsi.h"

#define SCSI_CMD_TIMEOUT                (30)

ScsiIoIf::ScsiIoIf()
{
}

ScsiIoIf::~ScsiIoIf() {
}


#ifdef _ENABLE_PHYSICAL_DEVICE

#include "Device/DeviceIoUtility.h"
eu8 ScsiIoIf::send_scsi_cmd_base(HANDLE handle, BYTE cdb[16], ULONG dataXferLen, BYTE direction, eu8_p buffer) {
    DeviceIoUtility diu;
    return diu.scsi_pass_through_direct(handle, cdb, buffer, dataXferLen, direction, SCSI_CMD_TIMEOUT);
}

#else

#include "FakeScsi/SimpleDisk.h"
#include "Utility/Utility.h"

SimpleDisk m_sd;

eu8 ScsiIoIf::send_scsi_cmd_base(HANDLE handle, BYTE cdb[16], ULONG dataXferLen, BYTE direction, eu8_p buffer) {
    return m_sd.send_scsi_cmd(cdb, dataXferLen, direction, buffer);
}

void ScsiIoIf::init_disk() {
    return m_sd.init_disk();
}

#endif

void ScsiIoIf::scsi_io_cmd(HANDLE handle, BYTE cdb[16], ULONG dataXferLen, BYTE direction, eu8_p buffer) {
    send_scsi_cmd_base(handle, cdb, dataXferLen, direction, buffer);
}
