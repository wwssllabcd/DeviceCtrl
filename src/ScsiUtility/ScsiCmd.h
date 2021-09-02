#pragma once

#include "DefineFiles/DefineFile.h"
#include "DefineScsi.h"

#include <vector>

using namespace std;

class ScsiCmd;

typedef vector<ScsiCmd> ScsiCmdSet;

//the same with "SCSI_IOCTL_DATA_OUT"
//#define SCSI_IOCTL_DATA_OUT          0
//#define SCSI_IOCTL_DATA_IN           1
//#define SCSI_IOCTL_DATA_UNSPECIFIED  2
enum class ScsiIoDir {
    DATA_OUT = 0,
    DATA_IN = 1,
    NON_DATA = 2
};

class ScsiCmd
{
public:
    ScsiCmd(void);
	~ScsiCmd(void);

	eu8 cdb[16];
	eu32 length;
    ScsiIoDir direction;
	estring description;

	bool operator ==(const ScsiCmd& rhs) const;


    ScsiCmdSet get_all_command_set(void);

    ScsiCmd request_sense(void);
    ScsiCmd inquiry(void);
    ScsiCmd read_capacity(void);
    ScsiCmd test_unit_ready(void);
    ScsiCmd read_format_capacity(void);
	
    ScsiCmd prevent_allow_medium_removal(void);

    ScsiCmd read_10(eu32 lba, eu16 secCnt);
    ScsiCmd write_10(eu32 lba, eu16 secCnt);

    ScsiCmd ata_pass_through_12(void);
};

