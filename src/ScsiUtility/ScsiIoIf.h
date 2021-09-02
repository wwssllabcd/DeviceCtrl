#pragma once

#include "DefineFiles/DefineFile.h"
#include <Windows.h> //HANDLE


class ScsiIoIf
{
public:
    ScsiIoIf();
    ~ScsiIoIf();

    void scsi_io_cmd(HANDLE handle, BYTE cdb[16], ULONG dataXferLen, BYTE direction, eu8_p buffer);
    void init_disk();
private:
    eu8 send_scsi_cmd_base(HANDLE handle, BYTE cdb[16], ULONG dataXferLen, BYTE direction, eu8_p buffer);

};

