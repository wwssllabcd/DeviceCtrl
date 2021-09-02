#pragma once

#include "DefineFiles/DefineFile.h"

class SimpleDisk
{
public:

	SimpleDisk(void);
	~SimpleDisk(void);

	eu8 send_scsi_cmd(eu8 cdb[16], eu32 dataXferLen, eu8 direction, eu8_p buffer);

	void lba_read(eu32 lba, eu32 secCnt, eu8_p buffer);

	void lba_write(eu32 lba, eu32 secCnt, eu8_p buffer);

	void get_ufi_capacity(eu8_p buffer);
	void get_inquiry(eu8_p buffer);

	void init_disk();


private:
	eu8 m_fakeDevice[_256K];
};

